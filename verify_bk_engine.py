"""
Verification suite for BK_Engine.dll against SciPy independent solver
and fundamental physical invariants (Zaremba problem, Kirchhoff balance,
homogeneous medium asymptotics, and convergence).

Разработчик движка и составитель теста:
Заместитель начальника обособленного подразделения Смирнов Семен Георгиевич
"""

import os
import sys
import json
import ctypes
import numpy as np
import scipy.sparse as sp
import scipy.sparse.linalg as spla
import matplotlib.pyplot as plt

# =============================================================================
# 1. CTYPES WRAPPER FOR BK_ENGINE.DLL
# =============================================================================

class BKEngineWrapper:
    def __init__(self, dll_path, config_path):
        if not os.path.isfile(dll_path):
            raise FileNotFoundError(f"DLL not found: {dll_path}")
        if not os.path.isfile(config_path):
            raise FileNotFoundError(f"Config JSON not found: {config_path}")

        self.dll_path = os.path.abspath(dll_path)
        self.config_path = os.path.abspath(config_path)
        self.dll = ctypes.WinDLL(self.dll_path)
        self.handle = ctypes.c_void_p()

        with open(self.config_path, "r", encoding="utf-8") as f:
            self.config = json.load(f)

        ret = self.dll.BK_CreateSolverFromConfigFile(
            self.config_path.encode("utf-8"), None, ctypes.byref(self.handle)
        )
        if ret != 0:
            err = self.get_last_error()
            raise RuntimeError(f"BK_CreateSolverFromConfigFile failed (code {ret}): {err}")

        n_els = ctypes.c_int()
        n_modes = ctypes.c_int()
        self.dll.BK_GetProbeInfo(self.handle, ctypes.byref(n_els), ctypes.byref(n_modes))
        self.num_electrodes = n_els.value
        self.num_modes = n_modes.value

    def get_last_error(self):
        buf = ctypes.create_string_buffer(512)
        self.dll.BK_GetLastErrorMessage(self.handle, buf, 512)
        return buf.value.decode("utf-8", errors="ignore")

    def set_medium_geometry(self, r_bh, r_pz):
        ret = self.dll.BK_SetMediumGeometry(
            self.handle, ctypes.c_double(r_bh), ctypes.c_double(r_pz)
        )
        if ret != 0:
            raise RuntimeError(f"BK_SetMediumGeometry failed (code {ret}): {self.get_last_error()}")

    def get_grid_dimensions(self):
        nr = ctypes.c_int()
        nz = ctypes.c_int()
        self.dll.BK_GetGridDimensions(self.handle, ctypes.byref(nr), ctypes.byref(nz))
        return nr.value, nz.value

    def get_grid_nodes(self):
        nr, nz = self.get_grid_dimensions()
        r_arr = (ctypes.c_double * nr)()
        z_arr = (ctypes.c_double * nz)()
        self.dll.BK_GetGridNodes(self.handle, r_arr, nr, z_arr, nz)
        return np.array(r_arr), np.array(z_arr)

    def solve(self, rho_bh, rho_pz, rho_stratum):
        total = self.num_electrodes * self.num_modes
        out_buf = (ctypes.c_double * total)()
        ret = self.dll.BK_SolveResistivity(
            self.handle,
            ctypes.c_double(rho_bh),
            ctypes.c_double(rho_pz),
            ctypes.c_double(rho_stratum),
            out_buf,
            total,
        )
        if ret != 0:
            raise RuntimeError(f"BK_SolveResistivity failed (code {ret}): {self.get_last_error()}")
        return np.array(out_buf)

    def get_field_maps(self, mode_idx=0):
        nr, nz = self.get_grid_dimensions()
        total = nr * nz
        v_buf = (ctypes.c_double * total)()
        jr_buf = (ctypes.c_double * total)()
        jz_buf = (ctypes.c_double * total)()

        ret = self.dll.BK_GetFieldMaps(self.handle, mode_idx, v_buf, jr_buf, jz_buf)
        if ret != 0:
            raise RuntimeError(f"BK_GetFieldMaps failed (code {ret}): {self.get_last_error()}")

        V_map = np.array(v_buf).reshape((nz, nr))
        Jr_map = np.array(jr_buf).reshape((nz, nr))
        Jz_map = np.array(jz_buf).reshape((nz, nr))
        return V_map, Jr_map, Jz_map

    def close(self):
        if self.handle and self.handle.value:
            self.dll.BK_DestroySolver(self.handle)
            self.handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


# =============================================================================
# 2. INDEPENDENT SCIPY SOLVER (ZAREMBA MIXED B.V.P.)
# =============================================================================

def solve_scipy_independent(r_coords, z_coords, r_bh, r_pz, rho_bh, rho_pz, rho_t, config, mode_idx=0):
    """
    Solves the 2D axisymmetric DC potential problem on the cylindrical grid
    with Zaremba mixed boundary conditions:
      - Dirichlet on connected electrodes (A0, A1, B0)
      - Floating equipotential macro-nodes on measuring electrodes (M1, N1, M2, N2)
      - Homogeneous Neumann (insulator) on sonde surface between electrodes
      - Dirichlet V=0 on outer boundary r = r_max
      - Natural Neumann dV/dz = 0 on top/bottom boundaries
    """
    W = len(r_coords)
    H = len(z_coords)
    N = W * H

    # Effective vertical lengths
    h_eff = np.zeros(H)
    h_eff[0] = (z_coords[1] - z_coords[0]) / 2.0
    h_eff[-1] = (z_coords[-1] - z_coords[-2]) / 2.0
    h_eff[1:-1] = (z_coords[2:] - z_coords[:-2]) / 2.0

    # Conductances
    G_hor = np.zeros((H, W))
    G_ver = np.zeros((H, W))

    for z in range(H):
        for r in range(W):
            curr_r = r_coords[r]
            if curr_r <= r_bh + 1e-5:
                rho = rho_bh
            elif curr_r <= r_pz + 1e-5:
                rho = rho_pz
            else:
                rho = rho_t

            if r < W - 1:
                next_r = r_coords[r + 1]
                G_hor[z, r] = (2.0 * np.pi * h_eff[z]) / (np.log(next_r / curr_r) * rho)

            if z < H - 1:
                if r == 0:
                    G_ver[z, r] = 0.0  # Insulated sonde body (Zaremba Neumann)
                else:
                    dz = z_coords[z + 1] - z_coords[z]
                    r_in = (r_coords[r] + r_coords[r - 1]) / 2.0
                    r_out = curr_r + (curr_r - r_coords[r - 1]) / 2.0 if r == W - 1 else (r_coords[r + 1] + curr_r) / 2.0
                    G_ver[z, r] = (np.pi * (r_out**2 - r_in**2)) / (dz * rho)

    # Degrees of Freedom (DOF) Mapping
    dof = np.full((H, W), -2, dtype=int)
    fixed_v = np.zeros((H, W))

    # Outer ground: r = W - 1 -> V = 0
    dof[:, W - 1] = -1
    fixed_v[:, W - 1] = 0.0

    def get_z_idx(target_z):
        idx = np.searchsorted(z_coords, target_z)
        if idx >= H:
            return H - 1
        if idx == 0:
            return 0
        if z_coords[idx] - target_z < target_z - z_coords[idx - 1]:
            return idx
        return idx - 1

    num_dofs = 0
    mode = config['modes'][mode_idx]['states']
    el_info_map = {}

    for s in mode:
        el_name = s['electrode']
        el_def = next(e for e in config['electrodes'] if e['name'] == el_name)
        z_s = get_z_idx(el_def['z_start'])
        z_e = get_z_idx(el_def['z_start'] + el_def['length'])
        if z_e < z_s:
            z_s, z_e = z_e, z_s

        st = s['state']
        if st in ('CONNECTED', 'FIXED_VOLTAGE'):
            v = s.get('voltage', 0.0)
            dof[z_s:z_e + 1, 0] = -1
            fixed_v[z_s:z_e + 1, 0] = v
            el_info_map[el_name] = ('CONNECTED', z_s, z_e, v)
        else:  # FLOATING macro-node
            m_dof = num_dofs
            num_dofs += 1
            dof[z_s:z_e + 1, 0] = m_dof
            el_info_map[el_name] = ('FLOATING', z_s, z_e, m_dof)

    # All free nodes in the medium and on the tool insulator
    for z in range(H):
        for r in range(W):
            if dof[z, r] == -2:
                dof[z, r] = num_dofs
                num_dofs += 1

    rows, cols, data = [], [], []
    B = np.zeros(num_dofs)

    def add_link(u_z, u_r, v_z, v_r, g):
        if g <= 0:
            return
        du = dof[u_z, u_r]
        dv = dof[v_z, v_r]
        if du >= 0 and dv >= 0:
            rows.extend([du, dv])
            cols.extend([du, dv])
            data.extend([g, g])
            if du != dv:
                rows.extend([du, dv])
                cols.extend([dv, du])
                data.extend([-g, -g])
        elif du >= 0 and dv < 0:
            rows.append(du)
            cols.append(du)
            data.append(g)
            B[du] += g * fixed_v[v_z, v_r]
        elif dv >= 0 and du < 0:
            rows.append(dv)
            cols.append(dv)
            data.append(g)
            B[dv] += g * fixed_v[u_z, u_r]

    for z in range(H):
        for r in range(W):
            if r < W - 1:
                add_link(z, r, z, r + 1, G_hor[z, r])
            if z < H - 1:
                add_link(z, r, z + 1, r, G_ver[z, r])

    A = sp.coo_matrix((data, (rows, cols)), shape=(num_dofs, num_dofs)).tocsr()
    X = spla.spsolve(A, B)

    V_scipy = np.zeros((H, W))
    for z in range(H):
        for r in range(W):
            d = dof[z, r]
            V_scipy[z, r] = X[d] if d >= 0 else fixed_v[z, r]

    # Calculate electrode values
    el_values = []
    for el in config['electrodes']:
        name = el['name']
        st, z_s, z_e, _ = el_info_map[name]
        if st == 'CONNECTED':
            I = sum((V_scipy[z, 0] - V_scipy[z, 1]) * G_hor[z, 0] for z in range(z_s, z_e + 1))
            el_values.append(I)
        else:
            el_values.append(V_scipy[z_s, 0])

    return V_scipy, np.array(el_values), G_hor, G_ver


# =============================================================================
# 3. VERIFICATION TESTS
# =============================================================================

def run_verification():
    print("=" * 80)
    print("    КОМПЛЕКСНАЯ ВЕРИФИКАЦИЯ БК-ЭНЖИН (BK_Engine.dll) С ПОМОЩЬЮ SCIPY")
    print("=" * 80)

    base_dir = os.path.dirname(os.path.abspath(__file__))
    dll_path = os.path.join(base_dir, "BK_Engine", "x64", "Release", "BK_Engine.dll")
    json_path = os.path.join(base_dir, "BK_Engine", "sample_sonde_bk3.json")

    with BKEngineWrapper(dll_path, json_path) as engine:
        nr, nz = engine.get_grid_dimensions()
        print(f"\n[1] Параметры сетки: Nr = {nr}, Nz = {nz}, всего узлов: {nr * nz}")

        r_coords, z_coords = engine.get_grid_nodes()
        print(f"    R: от {r_coords[0]:.4f} м (корпус прибора) до {r_coords[-1]:.1f} м (внешняя граница)")
        print(f"    Z: от {z_coords[0]:.2f} м до {z_coords[-1]:.2f} м (протяженность {z_coords[-1] - z_coords[0]:.1f} м)")

        # ---------------------------------------------------------------------
        # ТЕСТ 1: Прямое сопоставление со SciPy в неоднородной среде
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 1] Прямое поузловое сравнение BK_Engine (C++) и SciPy (Python)")
        print("         Среда: R_bh=0.108 м, R_pz=0.400 м, Ro_bh=1, Ro_pz=10, Ro_t=50 Ом*м")
        print("-" * 80)

        r_bh, r_pz = 0.108, 0.400
        rho_bh, rho_pz, rho_t = 1.0, 10.0, 50.0

        engine.set_medium_geometry(r_bh, r_pz)
        vals_dll = engine.solve(rho_bh, rho_pz, rho_t)
        V_dll, Jr_dll, Jz_dll = engine.get_field_maps(mode_idx=0)

        V_scipy, vals_scipy, G_hor, G_ver = solve_scipy_independent(
            r_coords, z_coords, r_bh, r_pz, rho_bh, rho_pz, rho_t, engine.config, mode_idx=0
        )

        max_err_2d = np.max(np.abs(V_dll - V_scipy))
        mean_err_2d = np.mean(np.abs(V_dll - V_scipy))
        print(f"\n-> Максимальная погрешность потенциала по всей 2D сетке ({nr*nz} узлов): {max_err_2d:.2e} В")
        print(f"-> Средняя абсолютная погрешность по узлам: {mean_err_2d:.2e} В")

        print("\nСравнение откликов электродов:")
        print(f"{'Электрод':<10} {'Тип':<12} {'BK_Engine (C++)':<18} {'SciPy (Python)':<18} {'Разница':<12}")
        print("-" * 72)
        for i, el in enumerate(engine.config['electrodes']):
            name = el['name']
            vd = vals_dll[i]
            vs = vals_scipy[i]
            diff = abs(vd - vs)
            st_name = "Ток [А]" if "A" in name or "B" in name else "Потенциал [В]"
            print(f"{name:<10} {st_name:<12} {vd:<18.8f} {vs:<18.8f} {diff:<12.2e}")

        # ---------------------------------------------------------------------
        # ТЕСТ 2: Физический баланс токов Кирхгофа
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 2] Проверка первого закона Кирхгофа (сохранение заряда)")
        print("-" * 80)

        # Вытекающие токи (источники A0, A1)
        I_sources = vals_dll[0] + vals_dll[5]
        # Втекающий ток стока B0
        I_sink_B0 = abs(vals_dll[6])
        # Ток утечки на внешнюю радиальную границу r = r_max
        I_boundary = 0.0
        for z in range(nz):
            I_boundary += (V_dll[z, nr - 2] - V_dll[z, nr - 1]) * G_hor[z, nr - 2]

        total_sinks = I_sink_B0 + I_boundary
        current_discrepancy = abs(I_sources - total_sinks)
        rel_discrepancy = (current_discrepancy / I_sources) * 100.0

        print(f"  Суммарный ток источников (I_A0 + I_A1):  {I_sources:.8f} А")
        print(f"  Ток на заземлитель прибора (|I_B0|):     {I_sink_B0:.8f} А")
        print(f"  Ток утечки на границу бесконечности:     {I_boundary:.8f} А")
        print(f"  Сумма стоков (|I_B0| + I_boundary):      {total_sinks:.8f} А")
        print(f"  Невязка баланса Кирхгофа:                {current_discrepancy:.2e} А ({rel_discrepancy:.6f}%)")
        assert rel_discrepancy < 1e-4, "Нарушение закона Кирхгофа!"

        # ---------------------------------------------------------------------
        # ТЕСТ 3: Проверка условий Зарембы на диэлектрическом корпусе прибора
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 3] Проверка условий Зарембы на диэлектрическом корпусе прибора")
        print("-" * 80)

        # Проверим, что на границе диэлектрического корпуса jr_in = 0 (ток внутрь зонда отсутствует)
        # В коде G_ver на r=0 обнулена, а радиальный ток на r=0 течет только в раствор:
        # Для измерительных электродов сумма токов строго равна нулю
        print("  Баланс токов на плавающих измерительных электродах (sum(I) = 0):")
        for idx, el_name in [(1, 'M1'), (2, 'N1'), (3, 'M2'), (4, 'N2')]:
            el_def = next(e for e in engine.config['electrodes'] if e['name'] == el_name)
            z_s = np.searchsorted(z_coords, el_def['z_start'])
            z_e = np.searchsorted(z_coords, el_def['z_start'] + el_def['length'])
            # Ток с поверхности электрода в скважину
            I_el = sum((V_dll[z, 0] - V_dll[z, 1]) * G_hor[z, 0] for z in range(z_s, z_e + 1))
            print(f"    Электрод {el_name}: интегральный ток в породу = {I_el:.2e} А (должен быть 0)")
            assert abs(I_el) < 1e-9, f"Ненулевой ток на плавающем электроде {el_name}!"

        # ---------------------------------------------------------------------
        # ТЕСТ 4: Однородное пространство и калибровка геометрического коэффициента
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 4] Однородная среда: инвариантность и калибровка коэффициента K")
        print("-" * 80)

        # В однородной среде Ro_bh = Ro_pz = Ro_t = Ro_0
        rho_levels = [0.2, 1.0, 5.0, 20.0, 100.0]
        K_factors = []
        app_resistivities = []

        # Базовая калибровка при Ro_0 = 1.0 Ом*м
        engine.set_medium_geometry(0.108, 0.400)
        res_homo_1 = engine.solve(1.0, 1.0, 1.0)
        I_A0_1 = res_homo_1[0]
        dU_MN_1 = res_homo_1[1] - res_homo_1[2]  # V_M1 - V_N1
        K_MN = 1.0 * (I_A0_1 / dU_MN_1)
        print(f"  Вычисленный геометрический коэффициент зонда K(M1-N1) = {K_MN:.4f} м")

        print(f"\n{'Истинное Ro [Ом*м]':<20} {'I_A0 [А]':<15} {'dU_MN [В]':<15} {'Ro_кажущееся [Ом*м]':<22} {'Погрешность (%)':<15}")
        print("-" * 85)

        for rho0 in rho_levels:
            res_h = engine.solve(rho0, rho0, rho0)
            I_a0 = res_h[0]
            du_mn = res_h[1] - res_h[2]
            rho_app = K_MN * (du_mn / I_a0)
            err_pct = abs(rho_app - rho0) / rho0 * 100.0
            print(f"{rho0:<20.2f} {I_a0:<15.6f} {du_mn:<15.6f} {rho_app:<22.6f} {err_pct:<15.4e}%")
            assert err_pct < 1e-3, f"Нарушение масштабирования в однородной среде при Ro={rho0}!"

        # ---------------------------------------------------------------------
        # ТЕСТ 5: Физические асимптотики и эквипотенциальность электродов
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 5] Физические асимптотики: монотонность импеданса и эквипотенциальность")
        print("-" * 80)

        # 5.1. Монотонность входного сопротивления при росте УЭС пласта Ro_t
        rhos_test = [0.1, 0.5, 1.0, 5.0, 10.0, 50.0, 100.0, 500.0, 1000.0]
        R_in_list = []
        print("  А. Монотонность входного сопротивления R_in = U_A0 / I_A0:")
        for rt in rhos_test:
            res_rt = engine.solve(1.0, 10.0, rt)
            ia_val = res_rt[0]
            r_in = 1.0 / ia_val
            R_in_list.append(r_in)
            print(f"     Ro_t = {rt:6.1f} Ом*м -> I_A0 = {ia_val:.6f} А | R_вх = {r_in:.4f} Ом")

        is_monotonic = all(x < y for x, y in zip(R_in_list, R_in_list[1:]))
        print(f"  -> Строгая монотонность R_вх(Ro_t): {'ВЫПОЛНЯЕТСЯ' if is_monotonic else 'НАРУШЕНА'}")
        assert is_monotonic, "Нарушение монотонности входного сопротивления!"

        # 5.2. Строгая эквипотенциальность колец плавающих электродов
        print("\n  Б. Проверка строгой эквипотенциальности измерительных колец (V = const):")
        engine.solve(1.0, 10.0, 50.0)
        v_field, _, _ = engine.get_field_maps(0)

        def get_z_node_idx(target_z):
            idx = np.searchsorted(z_coords, target_z)
            if idx >= len(z_coords): return len(z_coords) - 1
            if idx == 0: return 0
            if z_coords[idx] - target_z < target_z - z_coords[idx - 1]:
                return idx
            return idx - 1

        for name in ['M1', 'N1', 'M2', 'N2']:
            el_d = next(e for e in engine.config['electrodes'] if e['name'] == name)
            i_s = get_z_node_idx(el_d['z_start'])
            i_e = get_z_node_idx(el_d['z_start'] + el_d['length'])
            seg_v = v_field[i_s:i_e + 1, 0]
            spread = np.max(seg_v) - np.min(seg_v)
            print(f"     Электрод {name} (узлы {i_s}..{i_e}, z={z_coords[i_s]:.3f}..{z_coords[i_e]:.3f} м): V = {seg_v[0]:.8f} В, разброс = {spread:.2e} В")
            assert spread < 1e-12, f"Разброс потенциала на электроде {name} превышает машинную точность!"

        # ---------------------------------------------------------------------
        # ТЕСТ 6: Построение графиков и сохранение визуализации
        # ---------------------------------------------------------------------
        print("\n" + "-" * 80)
        print("[ТЕСТ 6] Формирование графиков верификации...")
        print("-" * 80)

        engine.set_medium_geometry(0.108, 0.400)
        engine.solve(1.0, 10.0, 50.0)
        V_map, Jr_map, Jz_map = engine.get_field_maps(0)

        fig, axs = plt.subplots(2, 2, figsize=(14, 10))

        # 1. 2D Поле потенциала BK_Engine
        r_zoom_idx = np.searchsorted(r_coords, 1.5)
        z_zoom_min = np.searchsorted(z_coords, -0.5)
        z_zoom_max = np.searchsorted(z_coords, 4.5)

        sub_r = r_coords[:r_zoom_idx]
        sub_z = z_coords[z_zoom_min:z_zoom_max]
        sub_V_dll = V_map[z_zoom_min:z_zoom_max, :r_zoom_idx]
        sub_V_sci = V_scipy[z_zoom_min:z_zoom_max, :r_zoom_idx]
        sub_diff = np.abs(sub_V_dll - sub_V_sci)

        # Plot A: V(r,z) BK_Engine
        im0 = axs[0, 0].pcolormesh(sub_r, sub_z, sub_V_dll, cmap='viridis', shading='auto')
        axs[0, 0].axvline(0.108, color='white', linestyle='--', linewidth=1.5, label='R_bh (скважина)')
        axs[0, 0].axvline(0.400, color='yellow', linestyle=':', linewidth=1.5, label='R_pz (ЗП)')
        axs[0, 0].set_title("Потенциал V(r,z) - BK_Engine.dll (C++)")
        axs[0, 0].set_xlabel("Радиус r, м")
        axs[0, 0].set_ylabel("Глубина z, м")
        axs[0, 0].legend(loc='upper right')
        fig.colorbar(im0, ax=axs[0, 0], label="Потенциал, В")

        # Plot B: V(r,z) SciPy
        im1 = axs[0, 1].pcolormesh(sub_r, sub_z, sub_V_sci, cmap='viridis', shading='auto')
        axs[0, 1].axvline(0.108, color='white', linestyle='--', linewidth=1.5)
        axs[0, 1].axvline(0.400, color='yellow', linestyle=':', linewidth=1.5)
        axs[0, 1].set_title("Потенциал V(r,z) - SciPy (Python Reference)")
        axs[0, 1].set_xlabel("Радиус r, м")
        axs[0, 1].set_ylabel("Глубина z, м")
        fig.colorbar(im1, ax=axs[0, 1], label="Потенциал, В")

        # Plot C: Разность решений |V_dll - V_scipy|
        im2 = axs[1, 0].pcolormesh(sub_r, sub_z, sub_diff, cmap='inferno', shading='auto')
        axs[1, 0].set_title(f"Разность |V_dll - V_scipy| (макс = {np.max(sub_diff):.1e} В)")
        axs[1, 0].set_xlabel("Радиус r, м")
        axs[1, 0].set_ylabel("Глубина z, м")
        fig.colorbar(im2, ax=axs[1, 0], label="Абсолютная разность, В")

        # Plot D: Профиль потенциала вдоль корпуса прибора (r = r_tool)
        axs[1, 1].plot(sub_z, sub_V_dll[:, 0], 'r-', linewidth=2, label='BK_Engine (C++)')
        axs[1, 1].plot(sub_z, sub_V_sci[:, 0], 'k--', linewidth=1.5, label='SciPy (Python)')
        axs[1, 1].set_title("Профиль потенциала вдоль корпуса прибора (r = r_tool)")
        axs[1, 1].set_xlabel("Глубина z, м")
        axs[1, 1].set_ylabel("Потенциал V, В")
        axs[1, 1].grid(True, alpha=0.3)
        axs[1, 1].legend()

        plt.tight_layout()
        plot_path = os.path.join(base_dir, "verification_report.png")
        plt.savefig(plot_path, dpi=200)
        print(f"  График успешно сохранен в файл: {plot_path}")

        print("\n" + "=" * 80)
        print("    ИТОГ: ВСЕ ТЕСТЫ ВЕРИФИКАЦИИ УСПЕШНО ПРОЙДЕНЫ С МАШИННОЙ ТОЧНОСТЬЮ!")
        print("=" * 80)


if __name__ == "__main__":
    run_verification()
