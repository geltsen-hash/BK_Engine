"""
Генерация наглядной концептуальной схемы параметрического сканирования длины зонда 5БК (120 мм).
Организация: НПФ «АМК Горизонт»
Разработчик: зам. начальника ОП Смирнов С. Г.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

def create_concept_figure(output_path="parametric_scan_concept_120mm.png"):
    fig = plt.figure(figsize=(17, 11), dpi=160)
    fig.patch.set_facecolor('#ffffff')

    # Заголовок всего полотна
    fig.suptitle(
        "НПФ «АМК ГОРИЗОНТ» | КОНЦЕПЦИЯ ПАРАМЕТРИЧЕСКОГО СКАНИРОВАНИЯ ДЛИНЫ ЗОНДА 5БК (Ø 120 мм)\n"
        "Поиск инженерного компромисса между технологичностью изготовления (L = 3.0 ... 7.0 м) и радиальной глубинностью",
        fontsize=13, fontweight='bold', color='#1a252f', y=0.975
    )
    plt.figtext(0.5, 0.915, "Разработал: Заместитель начальника ОП Смирнов С. Г. | Октябрь 2026 г.",
                ha='center', fontsize=10.5, style='italic', color='#444444')

    gs = fig.add_gridspec(2, 2, height_ratios=[1.05, 1.35], hspace=0.38, wspace=0.24,
                           left=0.10, right=0.95, top=0.87, bottom=0.07)

    ax_top = fig.add_subplot(gs[0, :])
    ax_layout = fig.add_subplot(gs[1, 0])
    ax_physics = fig.add_subplot(gs[1, 1])

    # =========================================================================
    # ПАНЕЛЬ 1: Аппаратурная схема зонда 5БК и принцип масштабирования
    # =========================================================================
    ax_top.set_title("1. Архитектура 19-электродной решетки 5БК: фиксированная измерительная база и масштабируемые экраны",
                     fontsize=11, fontweight='bold', color='#1f497d', pad=10)
    ax_top.set_xlim(-4.0, 4.0)
    ax_top.set_ylim(-1.8, 2.0)
    ax_top.axis('off')

    # Корпус зонда
    body_h = 0.5
    ax_top.add_patch(patches.Rectangle((-3.7, -body_h/2), 7.4, body_h, facecolor='#eaecee', edgecolor='#7f8c8d', lw=1.5, zorder=1))
    ax_top.text(-3.65, 0.35, "Корпус прибора Ø 120 мм (стеклопластик)", fontsize=9, color='#5d6d7e', fontweight='bold')

    # Осевая линия
    ax_top.plot([-3.9, 3.9], [0, 0], color='#bdc3c7', linestyle='--', lw=1.0, zorder=2)

    # Центральный электрод A0
    ax_top.add_patch(patches.Rectangle((-0.1, -body_h/2), 0.2, body_h, facecolor='#e74c3c', edgecolor='#922b21', lw=1.5, zorder=3))
    ax_top.text(0, -0.05, "A0", ha='center', va='center', color='white', fontweight='bold', fontsize=10, zorder=4)
    ax_top.text(0, 0.45, "A0 (ток)\nl = 50 мм", ha='center', fontsize=8, color='#c0392b', fontweight='bold')

    # Мониторные пары M и N (фиксированная база)
    m_coords = [-0.4, 0.4]
    n_coords = [-0.8, 0.8]
    for c, name in zip(m_coords, ["M_bot", "M_top"]):
        ax_top.add_patch(patches.Rectangle((c-0.08, -body_h/2), 0.16, body_h, facecolor='#f39c12', edgecolor='#b9770e', lw=1.2, zorder=3))
        ax_top.text(c, 0, "M", ha='center', va='center', color='white', fontweight='bold', fontsize=9, zorder=4)
    for c, name in zip(n_coords, ["N_bot", "N_top"]):
        ax_top.add_patch(patches.Rectangle((c-0.08, -body_h/2), 0.16, body_h, facecolor='#f1c40f', edgecolor='#b7950b', lw=1.2, zorder=3))
        ax_top.text(c, 0, "N", ha='center', va='center', color='black', fontweight='bold', fontsize=9, zorder=4)

    # Фиксированная скоба базы 0.38 м
    ax_top.plot([-0.8, -0.8, 0.8, 0.8], [-0.5, -0.7, -0.7, -0.5], color='#d35400', lw=1.5)
    ax_top.text(0, -0.95, "ФИКСИРОВАННЫЙ ИЗМЕРИТЕЛЬНЫЙ БЛОК M-N (база 0.38 м)\nВертикальное разрешение строго = 0.38 м (не зависит от длины L)",
                ha='center', fontsize=8.5, color='#a04000', fontweight='bold',
                bbox=dict(boxstyle="round,pad=0.3", fc="#fbeee6", ec="#e59866", lw=1))

    # Экранирующие электроды A1 .. A6 и Ny
    screens_pos = [1.2, 1.6, 2.1, 2.6, 3.1, 3.6]
    screens_neg = [-x for x in screens_pos]
    for sp, sn, lbl in zip(screens_pos, screens_neg, ["A1", "A2", "A3", "A4", "A5", "A6/Ny"]):
        ax_top.add_patch(patches.Rectangle((sp-0.09, -body_h/2), 0.18, body_h, facecolor='#2980b9', edgecolor='#1b4f72', lw=1.2, zorder=3))
        ax_top.add_patch(patches.Rectangle((sn-0.09, -body_h/2), 0.18, body_h, facecolor='#2980b9', edgecolor='#1b4f72', lw=1.2, zorder=3))
        ax_top.text(sp, 0, lbl.split('/')[0], ha='center', va='center', color='white', fontweight='bold', fontsize=7.5, zorder=4)
        ax_top.text(sn, 0, lbl.split('/')[0], ha='center', va='center', color='white', fontweight='bold', fontsize=7.5, zorder=4)

    # Стрелки масштабирования (телескопический эффект)
    ax_top.annotate('', xy=(3.7, 0.75), xytext=(1.1, 0.75),
                    arrowprops=dict(arrowstyle="<->", color='#27ae60', lw=2.5))
    ax_top.text(2.4, 0.95, "МАСШТАБИРОВАНИЕ ЭКРАНОВ A1..A6\n(вариация габаритной длины от 3.0 до 7.0 м)",
                ha='center', fontsize=8.5, color='#1e8449', fontweight='bold')

    ax_top.annotate('', xy=(-3.7, 0.75), xytext=(-1.1, 0.75),
                    arrowprops=dict(arrowstyle="<->", color='#27ae60', lw=2.5))
    ax_top.text(-2.4, 0.95, "СИММЕТРИЧНОЕ СЖАТИЕ/РАЗДВИЖЕНИЕ\n(шаг сканирования ΔL = 0.25 м, 17 вариантов)",
                ha='center', fontsize=8.5, color='#1e8449', fontweight='bold')

    # =========================================================================
    # ПАНЕЛЬ 2: Сопоставление компоновок электродов при L = 3.0 м, 5.0 м, 7.0 м
    # =========================================================================
    ax_layout.set_title("2. Сравнение габаритов зондовой части\nпри крайних и оптимальных длинах",
                        fontsize=10.5, fontweight='bold', color='#1f497d', pad=10)
    ax_layout.set_xlim(-4.2, 5.2)
    ax_layout.set_ylim(-0.5, 3.5)
    ax_layout.set_xlabel("Продольная координата от центра A0, м", fontsize=9, fontweight='bold')
    ax_layout.set_yticks([0.5, 1.7, 2.9])
    ax_layout.set_yticklabels([
        "L = 3.0 м\n(Компактный,\nвысокая жесткость)",
        "L = 5.0 м\n(Промежуточный\nкомпромисс)",
        "L = 7.0 м\n(Макс. технологич.\nпредел цеха)"
    ], fontsize=8.5, fontweight='bold')
    ax_layout.grid(True, axis='x', linestyle=':', color='#bdc3c7', alpha=0.7)

    lengths = [3.0, 5.0, 7.0]
    y_offsets = [0.5, 1.7, 2.9]
    colors_bar = ['#e67e22', '#3498db', '#2ecc71']

    for L, y, col in zip(lengths, y_offsets, colors_bar):
        half_L = L / 2.0
        # Тело зонда
        ax_layout.add_patch(patches.Rectangle((-half_L, y-0.2), L, 0.4, facecolor='#f2f4f4', edgecolor='#7f8c8d', lw=1.2, zorder=1))

        # A0
        ax_layout.add_patch(patches.Rectangle((-0.025, y-0.2), 0.05, 0.4, facecolor='#e74c3c', edgecolor='#922b21', zorder=3))

        # M & N
        for sign in [-1, 1]:
            ax_layout.add_patch(patches.Rectangle((sign*0.19-0.02, y-0.2), 0.04, 0.4, facecolor='#f39c12', edgecolor='#b9770e', zorder=3))
            ax_layout.add_patch(patches.Rectangle((sign*0.38-0.02, y-0.2), 0.04, 0.4, facecolor='#f1c40f', edgecolor='#b7950b', zorder=3))

        # Screens layout calculation
        t = (L - 3.0) / 4.0
        weights = np.array([1.0, 1.3, 1.6, 2.0, 2.5, 3.2, 2.8])
        weights = weights / np.sum(weights)
        l_Ny = 0.080 + 0.100 * t
        c_Ny = half_L - l_Ny / 2.0
        dist_total = c_Ny - 0.400
        cum_w = np.cumsum(weights)

        screens = [
            0.400 + dist_total * cum_w[0],
            0.400 + dist_total * cum_w[1],
            0.400 + dist_total * cum_w[2],
            0.400 + dist_total * cum_w[3],
            0.400 + dist_total * cum_w[4],
            0.400 + dist_total * cum_w[5],
            c_Ny
        ]
        s_lengths = [0.04+0.02*t, 0.04+0.02*t, 0.05+0.03*t, 0.06+0.04*t, 0.08+0.04*t, 0.15+0.2*t, l_Ny]

        for sc, sl in zip(screens, s_lengths):
            for sign in [-1, 1]:
                ax_layout.add_patch(patches.Rectangle((sign*sc - sl/2, y-0.2), sl, 0.4, facecolor='#2980b9', edgecolor='#1b4f72', zorder=3))

        # Текстовые подписи
        ax_layout.text(half_L + 0.1, y, f"L = {L:.1f} м (полуразмах ±{half_L:.2f} м)", va='center', fontsize=8, fontweight='bold', color='#2c3e50')

    # =========================================================================
    # ПАНЕЛЬ 3: Физическая суть дифференциальной чувствительности g(r) и DOI
    # =========================================================================
    ax_physics.set_title("3. Физика дифференциальной радиальной чувствительности g(r)\nи точка инженерного компромисса",
                         fontsize=10.5, fontweight='bold', color='#1f497d', pad=10)

    r_axis = np.linspace(0.108, 2.5, 300)

    # Модельные кривые g(r) для 5 зондов (колокола)
    # Зонд 1 (мелкий), Зонд 2, Зонд 3, Зонд 4, Зонд 5 (глубокий)
    peaks = [0.22, 0.38, 0.60, 0.95, 1.45]
    sigmas = [0.06, 0.10, 0.16, 0.25, 0.38]
    colors_probes = ['#9b59b6', '#3498db', '#1abc9c', '#f39c12', '#e74c3c']

    for i, (pk, sg, col) in enumerate(zip(peaks, sigmas, colors_probes), 1):
        g = np.exp(-((r_axis - pk)**2) / (2 * sg**2))
        g = g / (np.sum(g) * (r_axis[1] - r_axis[0]))  # нормировка
        ax_physics.plot(r_axis, g, label=f"5БК-{i} (r_peak={pk:.2f} м)", color=col, lw=2.0)
        if i == 5:
            # Заливка для 5-го зонда
            r_50_idx = np.where(r_axis >= pk)[0][0]
            ax_physics.fill_between(r_axis[:r_50_idx], g[:r_50_idx], color=col, alpha=0.25,
                                    label="50% энергии (R_50% DOI)")
            ax_physics.axvline(pk, color='#c0392b', linestyle='--', lw=1.5)
            ax_physics.text(pk+0.04, np.max(g)*0.85, f"Пик 5БК-5\nr_peak={pk} м\nR_50%≈1.4 м",
                            fontsize=8, color='#c0392b', fontweight='bold')

    # Стенка скважины
    ax_physics.axvline(0.108, color='#7f8c8d', linestyle='-', lw=1.8)
    ax_physics.text(0.12, 2.2, "Стенка скважины\n(r_bh = 0.108 м, Ø 216 мм)", fontsize=7.5, color='#555555', fontweight='bold')

    ax_physics.set_xlim(0.05, 2.3)
    ax_physics.set_ylim(0, 3.2)
    ax_physics.set_xlabel("Радиальное расстояние от оси скважины r, м", fontsize=9, fontweight='bold')
    ax_physics.set_ylabel("Дифференциальная чувствительность g(r)", fontsize=9, fontweight='bold')
    ax_physics.grid(True, linestyle=':', color='#bdc3c7', alpha=0.7)
    ax_physics.legend(loc='upper right', fontsize=8, framealpha=0.9)

    plt.savefig(output_path, dpi=180)
    plt.close()
    print(f"-> Рисунок успешно сгенерирован: {output_path}")

if __name__ == "__main__":
    create_concept_figure()
