"""
НПФ «АМК ГОРИЗОНТ»
Разработал: Заместитель начальника ОП Смирнов С. Г.
Дата: Октябрь 2026 г.

Чертеж компоновки электродной решетки прибора 5БК (Ø 120 мм, L = 5.00 м)
с точными расстояниями от центра прибора до каждого электрода,
размерами колец и межэлектродными изоляционными зазорами.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

def draw_blueprint(output_path="sonde_5bk_120mm_5m_blueprint.png"):
    fig = plt.figure(figsize=(19, 12), dpi=180)
    fig.patch.set_facecolor('#ffffff')

    # Основной титульный заголовок
    fig.suptitle(
        "НПФ «АМК ГОРИЗОНТ» | СБОРОЧНЫЙ ЧЕРТЕЖ ЭЛЕКТРОДНОЙ РЕШЕТКИ 5БК (Ø 120 мм, L = 5.00 м)\n"
        "Спецификация геометрических расстояний от центра прибора (A0), длин электродов и изоляционных промежутков",
        fontsize=13, fontweight='bold', color='#1a252f', y=0.975
    )
    plt.figtext(0.5, 0.930, "Разработал: Заместитель начальника ОП Смирнов С. Г. | Октябрь 2026 г. | Исполнение: кольцевое (неазимутальное)",
                ha='center', fontsize=10.5, style='italic', color='#444444')

    gs = fig.add_gridspec(2, 2, height_ratios=[1.3, 1.1], hspace=0.38, wspace=0.22,
                           left=0.06, right=0.95, top=0.89, bottom=0.06)

    ax_main = fig.add_subplot(gs[0, :])
    ax_zoom = fig.add_subplot(gs[1, 0])
    ax_spec = fig.add_subplot(gs[1, 1])

    # Данные геометрии электродов (для верхней половины от центра A0 = 0.0)
    # Имя, расстояние от центра (мм), длина (мм), цвет, назначение
    electrodes_data = [
        ("A0", 0.0, 50.0, "#e74c3c", "Центральный токовый питающий"),
        ("M", 190.0, 40.0, "#f39c12", "Внутренний мониторный (градиент)"),
        ("N", 380.0, 40.0, "#f1c40f", "Внешний мониторный (градиент)"),
        ("A1", 541.3, 50.0, "#3498db", "Экранирующий диполь 1 (малый зонд)"),
        ("A2", 725.0, 50.0, "#2980b9", "Экранирующий диполь 2"),
        ("A3", 951.1, 65.0, "#1f618d", "Экранирующий диполь 3 (средний зонд)"),
        ("A4", 1233.8, 80.0, "#1a5276", "Экранирующий диполь 4"),
        ("A5", 1587.1, 100.0, "#154360", "Экранирующий диполь 5"),
        ("A6", 2039.3, 250.0, "#0b5345", "Концевой токовый заземлитель"),
        ("Ny", 2435.0, 130.0, "#7d3c98", "Удаленный опорный потенциальный"),
    ]

    # =========================================================================
    # ПАНЕЛЬ 1: Главный вид 5-метровой решетки (Масштабный чертеж)
    # =========================================================================
    ax_main.set_title("1. Главный вид: Схема расположения 19 электродов (полная длина L = 5000 мм, диаметр Ø 120 мм)",
                      fontsize=11, fontweight='bold', color='#1f497d', pad=12)
    ax_main.set_xlim(-2750, 2750)
    ax_main.set_ylim(-3.2, 4.0)
    ax_main.axis('off')

    tool_diam = 120.0
    body_h = 1.0  # визуальная высота тела зонда

    # Корпус зонда (стеклопластиковая труба Ø 120 мм, длина 5000 мм: от -2500 до +2500)
    ax_main.add_patch(patches.Rectangle((-2500, -body_h/2), 5000, body_h,
                                        facecolor='#eaeded', edgecolor='#7f8c8d', lw=1.5, zorder=1))

    # Осевая линия (штрихпунктирная)
    ax_main.plot([-2650, 2650], [0, 0], color='#c0392b', linestyle='-.', lw=1.2, zorder=2)
    ax_main.text(2680, 0, "Ось", va='center', fontsize=8.5, color='#c0392b', fontweight='bold')

    # Отрисовка электродов (симметрично относительно центра 0)
    for name, dist, length, col, _ in electrodes_data:
        signs = [0] if dist == 0 else [-1, 1]
        for s in signs:
            x_center = s * dist
            x_start = x_center - length / 2.0
            ax_main.add_patch(patches.Rectangle((x_start, -body_h/2), length, body_h,
                                                facecolor=col, edgecolor='#17202a', lw=1.2, zorder=3))
            # Подпись имени электрода внутри/снаружи
            suffix = "_в" if s == 1 else ("_н" if s == -1 else "")
            lbl = f"{name}{suffix}" if name != "A0" else "A0"
            ax_main.text(x_center, 0, lbl, ha='center', va='center', color='white',
                         fontweight='bold', fontsize=7.5, rotation=90 if length < 80 else 0, zorder=4)

    # -------------------------------------------------------------------------
    # Размерные линии сверху: Расстояния от центра A0 до центров электродов
    # -------------------------------------------------------------------------
    # Центральный маркер A0
    ax_main.plot([0, 0], [body_h/2, 3.6], color='#e74c3c', linestyle=':', lw=1.2)
    ax_main.plot([0], [body_h/2 + 0.1], marker='v', color='#e74c3c', markersize=6)

    dim_levels = [
        ("M", 190.0, 1.1, "#d35400"),
        ("N", 380.0, 1.45, "#b7950b"),
        ("A1", 541.3, 1.8, "#2980b9"),
        ("A2", 725.0, 2.15, "#2471a3"),
        ("A3", 951.1, 2.5, "#1f618d"),
        ("A4", 1233.8, 2.85, "#1a5276"),
        ("A5", 1587.1, 3.2, "#154360"),
        ("A6", 2039.3, 3.55, "#0b5345"),
        ("Ny", 2435.0, 3.9, "#7d3c98"),
    ]

    for name, dist, y_lvl, col in dim_levels:
        # Выносная линия от электрода вверх
        ax_main.plot([dist, dist], [body_h/2, y_lvl], color=col, linestyle=':', lw=1.0)
        # Стрелка от 0 до dist
        ax_main.annotate('', xy=(dist, y_lvl), xytext=(0, y_lvl),
                         arrowprops=dict(arrowstyle="<->", color=col, lw=1.2))
        # Текстовый размер
        ax_main.text(dist / 2.0, y_lvl + 0.08, f"До {name}: {dist:.1f} мм ({dist/1000:.3f} м)",
                     ha='center', va='bottom', fontsize=7.5, color=col, fontweight='bold',
                     bbox=dict(boxstyle="round,pad=0.15", fc="#ffffff", ec=col, lw=0.6, alpha=0.9))

    # Габаритный размер решетки снизу (L = 5000 мм)
    ax_main.plot([-2500, -2500], [-body_h/2, -1.8], color='#2c3e50', linestyle=':', lw=1.2)
    ax_main.plot([2500, 2500], [-body_h/2, -1.8], color='#2c3e50', linestyle=':', lw=1.2)
    ax_main.annotate('', xy=(2500, -1.7), xytext=(-2500, -1.7),
                     arrowprops=dict(arrowstyle="<->", color='#2c3e50', lw=1.8))
    ax_main.text(0, -1.95, "ПОЛНАЯ ДЛИНА ЗОНДОВОЙ РЕШЕТКИ: L = 5000 мм (5.00 м)\nПолуразмах от центра A0: ±2500 мм",
                 ha='center', va='top', fontsize=9.5, color='#2c3e50', fontweight='bold',
                 bbox=dict(boxstyle="square,pad=0.3", fc="#eaf2f8", ec="#2980b9", lw=1.2))

    # Маркировка диаметра
    ax_main.annotate('', xy=(-2520, body_h/2), xytext=(-2520, -body_h/2),
                     arrowprops=dict(arrowstyle="<->", color='#555555', lw=1.2))
    ax_main.text(-2550, 0, "Ø 120 мм", ha='right', va='center', fontsize=8.5, fontweight='bold', color='#333333', rotation=90)

    # =========================================================================
    # ПАНЕЛЬ 2: Увеличенный вид измерительного блока M-A0-N (Масштаб 1:5)
    # =========================================================================
    ax_zoom.set_title("2. Выносной элемент А: Центральный измерительный узел (Масштаб 1:5)\nСтрого фиксированная база M-N = 0.38 м (вертикальное разрешение 0.38 м)",
                      fontsize=9.5, fontweight='bold', color='#1f497d', pad=8)
    ax_zoom.set_xlim(-600, 600)
    ax_zoom.set_ylim(-1.6, 2.2)
    ax_zoom.axis('off')

    zh = 0.8
    # Тело зонда в увеличенном масштабе
    ax_zoom.add_patch(patches.Rectangle((-580, -zh/2), 1160, zh, facecolor='#eaeded', edgecolor='#7f8c8d', lw=1.5, zorder=1))
    ax_zoom.plot([-590, 590], [0, 0], color='#c0392b', linestyle='-.', lw=1.2, zorder=2)

    # Электроды: A0, M_bot, M_top, N_bot, N_top, A1_bot, A1_top
    local_els = [
        ("A0", 0.0, 50.0, "#e74c3c"),
        ("M_н", -190.0, 40.0, "#f39c12"),
        ("M_в", 190.0, 40.0, "#f39c12"),
        ("N_н", -380.0, 40.0, "#f1c40f"),
        ("N_в", 380.0, 40.0, "#f1c40f"),
        ("A1_н", -541.3, 50.0, "#3498db"),
        ("A1_в", 541.3, 50.0, "#3498db"),
    ]
    for lbl, xc, l, col in local_els:
        ax_zoom.add_patch(patches.Rectangle((xc - l/2.0, -zh/2), l, zh, facecolor=col, edgecolor='#17202a', lw=1.2, zorder=3))
        ax_zoom.text(xc, 0, lbl, ha='center', va='center', color='white' if 'N' not in lbl else 'black',
                     fontweight='bold', fontsize=8.5, zorder=4)

    # Размеры измерительного узла сверху:
    # 1. Длина A0
    ax_zoom.annotate('', xy=(25, 0.6), xytext=(-25, 0.6), arrowprops=dict(arrowstyle="<->", color='#c0392b', lw=1.2))
    ax_zoom.text(0, 0.75, "l(A0)=50 мм", ha='center', fontsize=7.5, color='#c0392b', fontweight='bold')

    # 2. База M-N = 190 мм
    ax_zoom.annotate('', xy=(380, 0.95), xytext=(190, 0.95), arrowprops=dict(arrowstyle="<->", color='#d35400', lw=1.2))
    ax_zoom.text(285, 1.1, "База M-N = 190 мм", ha='center', fontsize=7.5, color='#d35400', fontweight='bold')

    # 3. Полная база мониторов M_н - M_в = 380 мм
    ax_zoom.annotate('', xy=(190, 1.5), xytext=(-190, 1.5), arrowprops=dict(arrowstyle="<->", color='#a04000', lw=1.5))
    ax_zoom.text(0, 1.65, "ФИКСИРОВАННАЯ ИЗМЕРИТЕЛЬНАЯ БАЗА M_н - M_в = 380 мм (0.38 м)",
                 ha='center', fontsize=8, color='#a04000', fontweight='bold',
                 bbox=dict(boxstyle="round,pad=0.2", fc="#fbeee6", ec="#e59866", lw=1))

    # Размеры снизу (изоляционные зазоры):
    # Зазор A0 - M = 145 мм
    ax_zoom.annotate('', xy=(170, -0.65), xytext=(25, -0.65), arrowprops=dict(arrowstyle="<->", color='#27ae60', lw=1.2))
    ax_zoom.text(97.5, -0.9, "Изолятор:\n145 мм", ha='center', fontsize=7, color='#1e8449', fontweight='bold')

    # Зазор M - N = 150 мм
    ax_zoom.annotate('', xy=(360, -0.65), xytext=(210, -0.65), arrowprops=dict(arrowstyle="<->", color='#27ae60', lw=1.2))
    ax_zoom.text(285, -0.9, "Изолятор:\n150 мм", ha='center', fontsize=7, color='#1e8449', fontweight='bold')

    # Зазор N - A1 = 116.3 мм (минимальный)
    ax_zoom.annotate('', xy=(516.3, -0.65), xytext=(400, -0.65), arrowprops=dict(arrowstyle="<->", color='#e74c3c', lw=1.2))
    ax_zoom.text(458, -0.9, "Мин. зазор:\n116.3 мм", ha='center', fontsize=7, color='#c0392b', fontweight='bold')

    # =========================================================================
    # ПАНЕЛЬ 3: Спецификация геометрических параметров 19 электродов
    # =========================================================================
    ax_spec.set_title("3. Спецификация электродов и геометрические расстояния (5БК Ø 120 мм, L = 5.0 м)",
                      fontsize=9.5, fontweight='bold', color='#1f497d', pad=8)
    ax_spec.axis('off')

    table_data = [
        ["Электрод", "Назначение", "Расст. от центра A0", "Длина кольца", "Зазор до след."],
        ["A0", "Центральный токовый", "0.0 мм", "50.0 мм", "145.0 мм (до M)"],
        ["M (в/н)", "Мониторный внутренний", "190.0 мм (0.19 м)", "40.0 мм", "150.0 мм (до N)"],
        ["N (в/н)", "Мониторный внешний", "380.0 мм (0.38 м)", "40.0 мм", "116.3 мм (до A1)"],
        ["A1 (в/н)", "Экран зонда 1 (малый)", "541.3 мм (0.54 м)", "50.0 мм", "133.7 мм (до A2)"],
        ["A2 (в/н)", "Экран зонда 2", "725.0 мм (0.73 м)", "50.0 мм", "168.6 мм (до A3)"],
        ["A3 (в/н)", "Экран зонда 3 (средний)", "951.1 мм (0.95 м)", "65.0 мм", "210.2 мм (до A4)"],
        ["A4 (в/н)", "Экран зонда 4", "1233.8 мм (1.23 м)", "80.0 мм", "263.3 мм (до A5)"],
        ["A5 (в/н)", "Экран зонда 5 (глубокий)", "1587.1 мм (1.59 м)", "100.0 мм", "277.2 мм (до A6)"],
        ["A6 (в/н)", "Концевой токовый заземлитель", "2039.3 мм (2.04 м)", "250.0 мм", "205.7 мм (до Ny)"],
        ["Ny (в/н)", "Опорный потенциальный", "2435.0 мм (2.44 м)", "130.0 мм", "0.0 мм (торец)"],
    ]

    col_widths = [0.15, 0.35, 0.22, 0.14, 0.18]
    table = ax_spec.table(cellText=table_data, colWidths=col_widths, loc='center', cellLoc='center')
    table.auto_set_font_size(False)
    table.set_fontsize(7.8)
    table.scale(1.0, 1.45)

    # Форматирование шапки и ячеек таблицы
    for (row, col), cell in table.get_celld().items():
        if row == 0:
            cell.set_facecolor('#1f497d')
            cell.set_text_props(color='white', fontweight='bold')
        else:
            if row % 2 == 1:
                cell.set_facecolor('#f8f9f9')
            if col == 0:
                cell.set_text_props(fontweight='bold')
            if col == 2:
                cell.set_text_props(color='#1b4f72', fontweight='bold')
            if row == 3 and col == 4:
                cell.set_facecolor('#fadbd8')
                cell.set_text_props(color='#922b21', fontweight='bold')  # мин. зазор 116.3 мм (N-A1)

    # Конструктивные примечания внизу
    notes_text = (
        "КОНСТРУКТИВНЫЕ ТРЕБОВАНИЯ:\n"
        "1. Корпус прибора: высокопрочный диэлектрический стеклопластик Ø 120 мм. Полная длина решетки: 5000 мм (±2500 мм от A0).\n"
        "2. Электроды: сплошные металлические кольца из немагнитной нержавеющей стали. Минимальный изоляционный зазор: 116.3 мм (N-A1).\n"
        "3. Вертикальное разрешение прибора строго зафиксировано базой мониторных электродов M-N (0.38 м) и неизменно для всех 5 зондов.\n"
        "4. Радиальная глубинность исследования: 5БК-1 (R50%=0.29 м) ... 5БК-5 (R50%=0.86 м, R90%=2.03 м)."
    )
    plt.figtext(0.06, 0.015, notes_text, fontsize=8, color='#333333',
                bbox=dict(boxstyle="square,pad=0.4", fc="#eaeded", ec="#bdc3c7", lw=1))

    plt.savefig(output_path, dpi=200)
    plt.close()
    print(f"-> Чертеж успешно сгенерирован: {output_path}")

if __name__ == "__main__":
    draw_blueprint()
