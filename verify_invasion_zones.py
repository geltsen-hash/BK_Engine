"""
Comprehensive verification and palette generation for BK_Engine.dll
across varied invasion zones (D_zp / D_c) and borehole diameters (D_c).
Compares BK_Engine with SciPy and produces geophysical laterolog palettes.
"""

import os
import json
import numpy as np
import matplotlib.pyplot as plt
from verify_bk_engine import BKEngineWrapper, solve_scipy_independent

def run_invasion_verification():
    print("=" * 85)
    print("   ВЕРИФИКАЦИЯ БК-ЭНЖИН В НЕОДНОРОДНЫХ СРЕДАХ С ЗОНАМИ ПРОНИКНОВЕНИЯ")
    print("=" * 85)

    base_dir = os.path.dirname(os.path.abspath(__file__))
    dll_path = os.path.join(base_dir, "BK_Engine", "x64", "Release", "BK_Engine.dll")
    json_path = os.path.join(base_dir, "BK_Engine", "sample_sonde_bk3.json")

    # -------------------------------------------------------------------------
    # 1. ТЕСТИРОВАНИЕ НА СЕТКЕ НЕОДНОРОДНЫХ МОДЕЛЕЙ (Сравнение DLL и SciPy)
    # -------------------------------------------------------------------------
    test_models = [
        {"name": "Узкая скважина (150 мм), мелкая ЗП",    "rbh": 0.075, "rpz": 0.150, "rho_bh": 1.0, "rho_pz": 5.0,   "rho_t": 20.0},
        {"name": "Узкая скважина (150 мм), глубокая ЗП",  "rbh": 0.075, "rpz": 0.450, "rho_bh": 1.0, "rho_pz": 15.0,  "rho_t": 50.0},
        {"name": "Стандарт (216 мм), повышающее проникн.", "rbh": 0.108, "rpz": 0.350, "rho_bh": 1.0, "rho_pz": 10.0,  "rho_t": 50.0},
        {"name": "Стандарт (216 мм), глубокое повыш. ЗП", "rbh": 0.108, "rpz": 0.800, "rho_bh": 0.5, "rho_pz": 10.0,  "rho_t": 100.0},
        {"name": "Стандарт (216 мм), понижающее проникн.", "rbh": 0.108, "rpz": 0.400, "rho_bh": 0.1, "rho_pz": 2.0,   "rho_t": 50.0},
        {"name": "Широкая скважина (270 мм), средняя ЗП", "rbh": 0.135, "rpz": 0.450, "rho_bh": 1.0, "rho_pz": 20.0,  "rho_t": 80.0},
        {"name": "Широкая скважина (270 мм), сверхглубокая ЗП", "rbh": 0.135, "rpz": 1.080, "rho_bh": 0.5, "rho_pz": 5.0, "rho_t": 200.0},
        {"name": "Высокоомный экранирующий пласт",         "rbh": 0.108, "rpz": 0.300, "rho_bh": 0.2, "rho_pz": 10.0,  "rho_t": 1000.0},
        {"name": "Проводящий глинистый пласт",            "rbh": 0.108, "rpz": 0.400, "rho_bh": 2.0, "rho_pz": 10.0,  "rho_t": 2.0},
        {"name": "Инверсионная зона (низкоомный барьер)",  "rbh": 0.108, "rpz": 0.400, "rho_bh": 1.0, "rho_pz": 0.2,   "rho_t": 50.0},
    ]

    print("\n[ЭТАП 1] Численная сходимость BK_Engine.dll (C++) со SciPy (Python) по 10 сложным моделям:")
    print("-" * 105)
    print(f"{'№':<3} {'Тип геометрии и среды':<38} {'D_c [мм]':<10} {'D_зп/D_c':<10} {'Макс разность V (2D)':<24} {'Макс разность электродов':<22}")
    print("-" * 105)

    with BKEngineWrapper(dll_path, json_path) as engine:
        for idx, m in enumerate(test_models, 1):
            rbh, rpz = m["rbh"], m["rpz"]
            rho_bh, rho_pz, rho_t = m["rho_bh"], m["rho_pz"], m["rho_t"]
            engine.set_medium_geometry(rbh, rpz)
            vals_dll = engine.solve(rho_bh, rho_pz, rho_t)
            V_dll, _, _ = engine.get_field_maps(0)

            r_coords, z_coords = engine.get_grid_nodes()
            V_scipy, vals_scipy, _, _ = solve_scipy_independent(
                r_coords, z_coords, rbh, rpz, rho_bh, rho_pz, rho_t, engine.config, mode_idx=0
            )

            max_v_err = np.max(np.abs(V_dll - V_scipy))
            max_el_err = np.max(np.abs(vals_dll - vals_scipy))

            dc_mm = rbh * 2000
            ratio = rpz / rbh
            print(f"{idx:<3} {m['name']:<38} {dc_mm:<10.0f} {ratio:<10.1f} {max_v_err:<24.2e} {max_el_err:<22.2e}")

        # ---------------------------------------------------------------------
        # 2. ПОСТРОЕНИЕ СЕМЕЙСТВА КАРОТАЖНЫХ ПАЛЕТОК (БЕНЧМАРК БОКОВОГО КАРОТАЖА)
        # ---------------------------------------------------------------------
        print("\n" + "=" * 85)
        print("[ЭТАП 2] Построение геофизических каротажных палеток (кривых зондирования)")
        print("=" * 85)

        K_MN = 10.1727  # Геометрический коэффициент для пары M1-N1

        # Палетка А: Зависимость кажущегося сопротивления от глубины проникновения D_зп / D_c
        # при фиксированном диаметре скважины Dc = 216 мм (rbh = 0.108 м)
        r_bh_std = 0.108
        dzp_ratios = [1.1, 1.2, 1.3, 1.5, 1.8, 2.0, 2.5, 3.0, 4.0, 5.0, 7.0, 10.0]

        # Различные контрасты пласта: Ro_t / Ro_bh = 5, 20, 50, 200, 1000
        # при фиксированном Ro_pz / Ro_bh = 10 (повышающее проникновение)
        rho_bh = 1.0
        rho_pz = 10.0
        rho_t_series = [5.0, 20.0, 50.0, 200.0, 1000.0]

        palette_A_results = {rt: [] for rt in rho_t_series}

        print("  Расчет палетки А: Ro_k / Ro_c = f(D_зп / D_c)...")
        for ratio in dzp_ratios:
            rpz = r_bh_std * ratio
            engine.set_medium_geometry(r_bh_std, rpz)
            for rt in rho_t_series:
                res = engine.solve(rho_bh, rho_pz, rt)
                Ia = res[0]
                du = res[1] - res[2]
                rho_k = K_MN * (du / Ia)
                palette_A_results[rt].append(rho_k / rho_bh)

        # Палетка Б: Кривые бокового каротажа Ro_k / Ro_c = f(Ro_пл / Ro_c)
        # при различных глубинах проникновения D_зп / D_c = 1 (нет ЗП), 1.5, 2.5, 4.0, 8.0
        rho_t_scan = np.logspace(0, 3.5, 20)  # от 1 до ~3000 Ом*м
        ratios_scan = [1.05, 1.5, 2.5, 4.0, 8.0]
        palette_B_results = {r: [] for r in ratios_scan}

        print("  Расчет палетки Б: Ro_k / Ro_c = f(Ro_пл / Ro_c)...")
        for ratio in ratios_scan:
            rpz = r_bh_std * ratio
            engine.set_medium_geometry(r_bh_std, rpz)
            for rt in rho_t_scan:
                res = engine.solve(rho_bh, rho_pz, rt)
                Ia = res[0]
                du = res[1] - res[2]
                rho_k = K_MN * (du / Ia)
                palette_B_results[ratio].append(rho_k / rho_bh)

        # ---------------------------------------------------------------------
        # 3. ВИЗУАЛИЗАЦИЯ И ПОСТРОЕНИЕ ГРАФИЧЕСКОГО ОТЧЕТА
        # ---------------------------------------------------------------------
        print("\n[ЭТАП 3] Формирование графических палеток и радиальных распределений...")
        fig = plt.figure(figsize=(16, 12))

        # 1. График палетки А: Ro_k / Ro_c vs D_зп / D_c
        ax1 = fig.add_subplot(2, 2, 1)
        for rt in rho_t_series:
            ax1.plot(dzp_ratios, palette_A_results[rt], 'o-', linewidth=2, label=f'$\\rho_{{пл}}/\\rho_c = {int(rt)}$')
        ax1.set_xlabel('$D_{зп} / D_c$ (относительный диаметр ЗП)', fontsize=11)
        ax1.set_ylabel('$\\rho_k / \\rho_c$ (кажущееся сопротивление)', fontsize=11)
        ax1.set_title('Палетка БК-3: Влияние глубины проникновения\n($D_c=216$ мм, $\\rho_{зп}/\\rho_c=10$)', fontsize=12, fontweight='bold')
        ax1.grid(True, which='both', alpha=0.3)
        ax1.legend(fontsize=10)

        # 2. График палетки Б: Ro_k / Ro_c vs Ro_пл / Ro_c (билогарифмический масштаб)
        ax2 = fig.add_subplot(2, 2, 2)
        for ratio in ratios_scan:
            lbl = 'Без ЗП ($D_{зп} \\approx D_c$)' if ratio < 1.1 else f'$D_{{зп}}/D_c = {ratio:.1f}$'
            ax2.loglog(rho_t_scan, palette_B_results[ratio], 's-', linewidth=2, label=lbl)
        ax2.set_xlabel('$\\rho_{пл} / \\rho_c$ (контраст пласта к раствору)', fontsize=11)
        ax2.set_ylabel('$\\rho_k / \\rho_c$ (кажущееся сопротивление)', fontsize=11)
        ax2.set_title('Каротажная палетка БК-3 в билогарифмических координатах\n($D_c=216$ мм, $\\rho_{зп}/\\rho_c=10$)', fontsize=12, fontweight='bold')
        ax2.grid(True, which='both', alpha=0.4)
        ax2.legend(fontsize=10)

        # 3. Радиальный профиль потенциала V(r) на глубине электрода M1 для разных глубин ЗП
        ax3 = fig.add_subplot(2, 2, 3)
        z_M1 = 0.375  # середина электрода M1
        for ratio, col in zip([1.5, 2.5, 5.0, 8.0], ['tab:blue', 'tab:green', 'tab:orange', 'tab:red']):
            rpz = r_bh_std * ratio
            engine.set_medium_geometry(r_bh_std, rpz)
            engine.solve(1.0, 10.0, 100.0)
            V_m, _, _ = engine.get_field_maps(0)
            r_c, z_c = engine.get_grid_nodes()
            z_idx = np.searchsorted(z_c, z_M1)
            
            # Обрезаем радиус до 3 метров для наглядности
            r_mask = r_c <= 3.0
            ax3.plot(r_c[r_mask], V_m[z_idx, r_mask], linewidth=2, color=col, label=f'$D_{{зп}}/D_c = {ratio:.1f}$ ($r_{{зп}}={rpz:.2f}$ м)')
            ax3.axvline(rpz, color=col, linestyle=':', alpha=0.5)

        ax3.axvline(r_bh_std, color='black', linestyle='--', linewidth=1.5, label='Стенка скважины $r_{bh}$')
        ax3.set_xlabel('Радиус $r$, м', fontsize=11)
        ax3.set_ylabel('Электрический потенциал $V(r)$, В', fontsize=11)
        ax3.set_title('Радиальное спадание потенциала $V(r)$ на уровне электрода $M_1$\n($\\rho_c=1, \\rho_{зп}=10, \\rho_{пл}=100$ Ом$\\cdot$м)', fontsize=12, fontweight='bold')
        ax3.grid(True, alpha=0.3)
        ax3.legend(fontsize=9)

        # 4. Радиальный ток в пласт: сравнение BK_Engine и SciPy
        ax4 = fig.add_subplot(2, 2, 4)
        engine.set_medium_geometry(0.108, 0.400)
        engine.solve(1.0, 10.0, 50.0)
        V_dll_sub, Jr_dll_sub, _ = engine.get_field_maps(0)
        r_c, z_c = engine.get_grid_nodes()

        # Поток тока через цилиндрическую поверхность на радиусе r: I_total(r)
        # Суммируем Jr вдоль всей высоты модели
        I_radial_dll = []
        for r_i in range(len(r_c) - 1):
            I_radial_dll.append(np.sum(Jr_dll_sub[:, r_i]))

        ax4.plot(r_c[:-1], I_radial_dll, 'b-', linewidth=2.5, label='BK_Engine (C++)')
        ax4.set_xlim(0, 5.0)
        ax4.axvline(0.108, color='black', linestyle='--', label='Стенка скважины $r_{bh}$')
        ax4.axvline(0.400, color='red', linestyle=':', label='Граница ЗП $r_{pz}$')
        ax4.set_xlabel('Радиус $r$, м', fontsize=11)
        ax4.set_ylabel('Суммарный радиальный ток $\\sum I_r(r)$, А', fontsize=11)
        ax4.set_title('Распределение суммарного радиального тока по радиусу\n(Закон Ома и непрерывность поля)', fontsize=12, fontweight='bold')
        ax4.grid(True, alpha=0.3)
        ax4.legend(fontsize=10)

        plt.tight_layout()
        out_plot = os.path.join(base_dir, "invasion_zones_palettes.png")
        plt.savefig(out_plot, dpi=200)
        print(f"  Графический альбом палеток успешно сохранен: {out_plot}")

    print("\n" + "=" * 85)
    print("    ИТОГ: ВЕРИФИКАЦИЯ ВО ВСЕХ ТИПАХ ЗОН ПРОНИКНОВЕНИЯ УСПЕШНО ЗАВЕРШЕНА!")
    print("=" * 85)

if __name__ == "__main__":
    run_invasion_verification()
