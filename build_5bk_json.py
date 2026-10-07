"""
Генератор конфигурационных JSON-файлов для многозондового цифрового прибора 5БК
с математической фокусировкой (по материалам статьи Клименко В.А. и др., 2020).

Поддерживает диаметры корпуса 90 мм и 120 мм, а также масштабирование длины.
"""

import json
import os

def create_5bk_config(tool_diameter_mm=90, scale_length=1.0, output_filename=None):
    r_tool = (tool_diameter_mm / 1000.0) / 2.0
    
    # -------------------------------------------------------------------------
    # Расстояния электродов от центрального электрода A0 (в метрах)
    # -------------------------------------------------------------------------
    # Измерительная база M-N строго зафиксирована (вертикальное разрешение 0.38 м):
    d_M = 0.190   # Расстояние между центрами M_top и M_bot = 0.38 м
    d_N = 0.380   # Расстояние между центрами N_top и N_bot = 0.76 м (база M-N = 0.19 м)
    
    # Токовые экранирующие электроды A1..A6 и удаленный Ny (масштабируются по длине):
    d_A1 = 0.650 * scale_length
    d_A2 = 1.050 * scale_length
    d_A3 = 1.550 * scale_length
    d_A4 = 2.150 * scale_length
    d_A5 = 2.850 * scale_length
    d_A6 = 3.650 * scale_length
    d_Ny = 4.400 * scale_length
    
    # Смещение центра, чтобы нижний электрод Ny_bot начинался на безопасном z >= 0.5 м
    half_span = d_Ny + 0.30
    z_center = half_span + 0.50
    
    # Длины электродов (в метрах)
    l_A0 = 0.050
    l_M  = 0.040
    l_N  = 0.040
    l_A1 = 0.060
    l_A2 = 0.060
    l_A3 = 0.080
    l_A4 = 0.100
    l_A5 = 0.120
    l_A6 = 0.400
    l_Ny = 0.200
    
    # -------------------------------------------------------------------------
    # Определение 19 электродов зондовой установки 5БК
    # -------------------------------------------------------------------------
    electrodes = [
        # Центральный токовый электрод A0
        {"name": "A0",     "z_start": round(z_center - l_A0 / 2.0, 4), "length": l_A0},
        
        # Мониторинговые пары M и N (верх/низ)
        {"name": "M_bot",  "z_start": round(z_center - d_M - l_M / 2.0, 4), "length": l_M},
        {"name": "M_top",  "z_start": round(z_center + d_M - l_M / 2.0, 4), "length": l_M},
        {"name": "N_bot",  "z_start": round(z_center - d_N - l_N / 2.0, 4), "length": l_N},
        {"name": "N_top",  "z_start": round(z_center + d_N - l_N / 2.0, 4), "length": l_N},
        
        # Токовые экранирующие электроды A1 .. A6 (верх/низ)
        {"name": "A1_bot", "z_start": round(z_center - d_A1 - l_A1 / 2.0, 4), "length": l_A1},
        {"name": "A1_top", "z_start": round(z_center + d_A1 - l_A1 / 2.0, 4), "length": l_A1},
        
        {"name": "A2_bot", "z_start": round(z_center - d_A2 - l_A2 / 2.0, 4), "length": l_A2},
        {"name": "A2_top", "z_start": round(z_center + d_A2 - l_A2 / 2.0, 4), "length": l_A2},
        
        {"name": "A3_bot", "z_start": round(z_center - d_A3 - l_A3 / 2.0, 4), "length": l_A3},
        {"name": "A3_top", "z_start": round(z_center + d_A3 - l_A3 / 2.0, 4), "length": l_A3},
        
        {"name": "A4_bot", "z_start": round(z_center - d_A4 - l_A4 / 2.0, 4), "length": l_A4},
        {"name": "A4_top", "z_start": round(z_center + d_A4 - l_A4 / 2.0, 4), "length": l_A4},
        
        {"name": "A5_bot", "z_start": round(z_center - d_A5 - l_A5 / 2.0, 4), "length": l_A5},
        {"name": "A5_top", "z_start": round(z_center + d_A5 - l_A5 / 2.0, 4), "length": l_A5},
        
        {"name": "A6_bot", "z_start": round(z_center - d_A6 - l_A6 / 2.0, 4), "length": l_A6},
        {"name": "A6_top", "z_start": round(z_center + d_A6 - l_A6 / 2.0, 4), "length": l_A6},
        
        # Удаленные приемные электроды Ny (опорные потенциалы)
        {"name": "Ny_bot", "z_start": round(z_center - d_Ny - l_Ny / 2.0, 4), "length": l_Ny},
        {"name": "Ny_top", "z_start": round(z_center + d_Ny - l_Ny / 2.0, 4), "length": l_Ny},
    ]
    
    # -------------------------------------------------------------------------
    # 6 Элементарных дипольных режимов возбуждения (Рис. 1 в BKdigit.pdf)
    # -------------------------------------------------------------------------
    dipole_definitions = [
        ("Mode_A0_A2", [("A0", 1.0)], [("A2_bot", 0.0), ("A2_top", 0.0)]),
        ("Mode_A1_A2", [("A1_bot", 1.0), ("A1_top", 1.0)], [("A2_bot", 0.0), ("A2_top", 0.0)]),
        ("Mode_A2_A3", [("A2_bot", 1.0), ("A2_top", 1.0)], [("A3_bot", 0.0), ("A3_top", 0.0)]),
        ("Mode_A3_A4", [("A3_bot", 1.0), ("A3_top", 1.0)], [("A4_bot", 0.0), ("A4_top", 0.0)]),
        ("Mode_A4_A5", [("A4_bot", 1.0), ("A4_top", 1.0)], [("A5_bot", 0.0), ("A5_top", 0.0)]),
        ("Mode_A5_A6", [("A5_bot", 1.0), ("A5_top", 1.0)], [("A6_bot", 0.0), ("A6_top", 0.0)]),
    ]
    
    modes = []
    for mode_name, pos_list, neg_list in dipole_definitions:
        pos_dict = dict(pos_list)
        neg_dict = dict(neg_list)
        states = []
        for el in electrodes:
            el_name = el["name"]
            if el_name in pos_dict:
                states.append({"electrode": el_name, "state": "CONNECTED", "voltage": pos_dict[el_name]})
            elif el_name in neg_dict:
                states.append({"electrode": el_name, "state": "CONNECTED", "voltage": neg_dict[el_name]})
            else:
                states.append({"electrode": el_name, "state": "FLOATING"})
        modes.append({"mode_name": mode_name, "states": states})
    
    z_min = min(e["z_start"] for e in electrodes)
    z_max = max(e["z_start"] + e["length"] for e in electrodes)
    total_len = round(z_max - z_min, 3)
    
    config = {
        "probe_name": f"5BK_Digital_{tool_diameter_mm}mm_L{total_len}m",
        "tool_radius_m": r_tool,
        "tool_diameter_mm": tool_diameter_mm,
        "total_array_length_m": total_len,
        "electrodes": electrodes,
        "modes": modes
    }
    
    if output_filename:
        s = json.dumps(config, indent=2, ensure_ascii=False)
        with open(output_filename, "w", encoding="utf-8") as f:
            f.write(s)
        print(f"-> Файл сохранен: {output_filename} (Диаметр {tool_diameter_mm} мм, длина решетки {total_len} м)")
    
    return config

if __name__ == "__main__":
    base_dir = os.path.dirname(os.path.abspath(__file__))
    
    # 1. Базовый прибор 90 мм (габарит решетки ~9.0 м)
    f_90 = os.path.join(base_dir, "sonde_5bk_90mm.json")
    create_5bk_config(tool_diameter_mm=90, scale_length=1.0, output_filename=f_90)
    
    # 2. Базовый прибор 120 мм (габарит решетки ~9.0 м)
    f_120 = os.path.join(base_dir, "sonde_5bk_120mm.json")
    create_5bk_config(tool_diameter_mm=120, scale_length=1.0, output_filename=f_120)
    
    # 3. Укороченная версия (scale=0.6, длина решетки ~5.5 м)
    f_compact = os.path.join(base_dir, "sonde_5bk_90mm_compact5m.json")
    create_5bk_config(tool_diameter_mm=90, scale_length=0.6, output_filename=f_compact)
