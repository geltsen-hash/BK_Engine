# Спецификация C API библиотеки прямой задачи БК (BK_Engine.dll) для MATLAB

Данный документ описывает интерфейс динамической библиотеки `BK_Engine.dll` (C API) для высокопроизводительного решения прямой задачи бокового каротажа (БК / Laterolog) и интеграции в среду **MATLAB**.

---

## 1. Физическая модель и архитектура

- **Физическая модель**: 3-слойная цилиндрически-симметричная среда Дахнова с бесконечной мощностью пласта вдоль оси $Z$.
  - 2 радиальные границы: радиус скважины $r_{bh}$ и радиус зоны проникновения $r_{pz}$.
  - 3 электрических параметра: УЭС раствора $\rho_{bh}$, УЭС зоны проникновения $\rho_{pz}$, УЭС неизмененного пласта $\rho_{пл}$.
  - Всего 5 параметров среды: $(r_{bh}, \rho_{bh}, r_{pz}, \rho_{pz}, \rho_{пл})$.
- **Единицы измерения**: Строго **СИ**:
  - Линейные размеры ($Z, R, D$): **метры $[m]$**
  - Удельное электрическое сопротивление ($\rho$): **Ом·м $[\Omega \cdot m]$**
  - Разности потенциалов и напряжения ($U, V$): **Вольты $[V]$**
  - Электрические токи ($I$): **Амперы $[A]$**
- **Оптимизированный 2-уровневый расчет палеток**:
  - **Внешний цикл (геометрия $r_{bh}, r_{pz}$)**: функция `BK_SetMediumGeometry` перестраивает радиальную сетку с точной посадкой узлов на кромки $r_{bh}$ и $r_{pz}$ и фиксирует структуру СЛАУ (выполняется редко).
  - **Внутренний цикл (3 УЭС: $\rho_{bh}, \rho_{pz}, \rho_{пл}$)**: функция `BK_SolveResistivity` (или векторный батч `BK_SolveResistivityBatch`) мгновенно обновляет только численные проводимости резисторов без перевыделения памяти и решает СЛАУ сразу для всех рабочих режимов зонда.
- **Потокобезопасность**: Полная (`BKSolverHandle` автономен, расчеты масштабируются через `parfor` в MATLAB).

---

## 2. Заголовочный C-файл (`bk_engine_api.h`)

```c
#ifndef BK_ENGINE_API_H
#define BK_ENGINE_API_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #define BK_API __declspec(dllexport) __stdcall
#else
  #define BK_API
#endif

/* --- Коды возврата --- */
typedef enum {
    BK_STATUS_OK                  =  0,
    BK_ERR_INVALID_HANDLE         = -1,
    BK_ERR_INVALID_PARAMS         = -2,
    BK_ERR_CONFIG_FILE_NOT_FOUND  = -3,
    BK_ERR_CONFIG_PARSE_FAILED    = -4,
    BK_ERR_SINGULAR_MATRIX        = -5,
    BK_ERR_BUFFER_TOO_SMALL       = -6,
    BK_ERR_SOLVER_FAILED          = -7
} BK_Status;

/* --- Состояния электродов в режиме --- */
typedef enum {
    BK_ELECTRODE_CONNECTED = 0, /* Подключен к источнику U (при U=0 земля). На выходе: Ток I [А] */
    BK_ELECTRODE_FLOATING  = 1  /* Плавающий проводник (sum(I)=0).      На выходе: Потенциал V [В] */
} BK_ElectrodeState;

/* --- Геометрия электрода --- */
typedef struct {
    double z_start;       /* Начало электрода по оси Z [м] */
    double length;        /* Длина электрода [м]           */
    const char* name;     /* Имя электрода (например, "A0", "M1", "B0") */
} BK_ElectrodeDef;

/* --- Состояние электрода в режиме --- */
typedef struct {
    BK_ElectrodeState state;
    double fixed_voltage; /* Напряжение [В] (при state == BK_ELECTRODE_CONNECTED) */
} BK_ElectrodeModeState;

/* --- Описание режима работы --- */
typedef struct {
    const char* mode_name;               /* Имя режима ("Deep_AL_B1", "Medium_AM_B0") */
    const BK_ElectrodeModeState* states; /* Массив состояний размером [num_electrodes] */
} BK_WorkModeDef;

/* --- Параметры сеточной дискретизации --- */
typedef struct {
    int auto_grid;        /* 1 = автоматический расчет сетки, 0 = ручной */
    double dr_start;      /* Начальный радиальный шаг [м] */
    double q_r;           /* Множитель нарастания шага по r */
    double r_max;         /* Внешняя граница расчетной области [м] */
    double dz_tool;       /* Шаг по оси z в области прибора [м] */
    double dz_fine;       /* Сгущенный шаг по z на границах электродов [м] */
} BK_GridParams;

/* --- Полные параметры 3-слойной среды Дахнова --- */
typedef struct {
    double r_bh;          /* Радиус скважины [м] */
    double rho_bh;        /* УЭС бурового раствора [Ом*м] */
    double r_pz;          /* Радиус зоны проникновения [м] */
    double rho_pz;        /* УЭС зоны проникновения [Ом*м] */
    double rho_stratum;   /* УЭС неизмененной части пласта [Ом*м] */
} BK_MediumParams;

/* Непрозрачный указатель на экземпляр солвера */
typedef void* BKSolverHandle;

/* ========================================================================= */
/*                         ФУНКЦИИ API                                       */
/* ========================================================================= */

/* --- 1. Инициализация и жизненный цикл --- */
BK_Status BK_API BK_CreateSolver(
    const BK_ElectrodeDef* electrodes,
    int num_electrodes,
    const BK_WorkModeDef* modes,
    int num_modes,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
);

BK_Status BK_API BK_CreateSolverFromConfigFile(
    const char* config_filepath,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
);

BK_Status BK_API BK_DestroySolver(BKSolverHandle handle);

/* --- 2. Двухуровневый расчет палеток (High-Performance API) --- */

/* Внешний цикл: фиксация радиальных границ и адаптация сетки */
BK_Status BK_API BK_SetMediumGeometry(
    BKSolverHandle handle,
    double r_bh,
    double r_pz
);

/* Внутренний цикл: быстрый расчет откликов при изменении только 3 УЭС
   out_values: одномерный буфер размера [num_modes * num_electrodes]
   Для каждого режима и электрода:
     - если CONNECTED -> вычисленный ток I [А]
     - если FLOATING  -> вычисленный потенциал V [В]
*/
BK_Status BK_API BK_SolveResistivity(
    BKSolverHandle handle,
    double rho_bh,
    double rho_pz,
    double rho_stratum,
    double* out_values,
    int out_values_count
);

/* Векторный пакетный расчет пачки точек (УЭС) для зафиксированной геометрии
   out_matrix_flat: одномерный буфер размера [num_points * num_modes * num_electrodes]
*/
BK_Status BK_API BK_SolveResistivityBatch(
    BKSolverHandle handle,
    const double* rho_bh_arr,
    const double* rho_pz_arr,
    const double* rho_stratum_arr,
    int num_points,
    double* out_matrix_flat
);

/* --- 3. Атомарный расчет единичной точки среды (Stateless-style) --- */
BK_Status BK_API BK_Solve(
    BKSolverHandle handle,
    const BK_MediumParams* medium,
    double* out_values,
    int out_values_count
);

/* --- 4. Информация о зонде и сетке --- */
BK_Status BK_API BK_GetProbeInfo(
    BKSolverHandle handle,
    int* out_num_electrodes,
    int* out_num_modes
);

BK_Status BK_API BK_GetElectrodeInfo(
    BKSolverHandle handle,
    int electrode_index,
    char* out_name,
    int max_name_len,
    double* out_z_start,
    double* out_length
);

BK_Status BK_API BK_GetGridDimensions(BKSolverHandle handle, int* out_nr, int* out_nz);
BK_Status BK_API BK_GetGridNodes(
    BKSolverHandle handle,
    double* out_r_nodes,
    int nr,
    double* out_z_nodes,
    int nz
);

/* --- 5. Выгрузка 2D-полей потенциалов и токов (Caller-Allocated) --- */
BK_Status BK_API BK_GetFieldMaps(
    BKSolverHandle handle,
    int mode_index,
    double* out_potentials_2d,
    double* out_currents_r_2d,
    double* out_currents_z_2d
);

/* --- 6. Диагностика ошибок --- */
BK_Status BK_API BK_GetLastErrorMessage(BKSolverHandle handle, char* buffer, int max_len);

#ifdef __cplusplus
}
#endif

#endif /* BK_ENGINE_API_H */
```

---

## 3. Примеры использования в MATLAB

### Вариант А: Использование нативного MEX-модуля (`bk_engine_mex.mexw64`) — Рекомендуется

MEX-обертка компилируется вместе с библиотекой и предоставляет максимально удобный и быстрый синтаксис (без указателей `libpointer` и накладных расходов):

```matlab
% 1. Создание экземпляра солвера из конфигурационного JSON
hSolver = bk_engine_mex('create', 'sonde_config.json');

% 2. Сетки параметров среды для палетки
r_bh_arr   = [0.108, 0.124];
r_pz_arr   = [0.20, 0.35, 0.50, 0.80, 1.20];
rho_bh_arr = [0.1, 1.0];
rho_pz_arr = logspace(-1, 2, 10);
rho_t_arr  = logspace(0, 3, 20);

% 3. ВНЕШНИЙ ЦИКЛ ПО ГЕОМЕТРИИ (перестройка сетки выполняется только здесь)
for ir_bh = 1:length(r_bh_arr)
    r_bh = r_bh_arr(ir_bh);
    for ir_pz = 1:length(r_pz_arr)
        r_pz = r_pz_arr(ir_pz);
        
        % Фиксация геометрии в солвере
        bk_engine_mex('set_geom', hSolver, r_bh, r_pz);
        
        % 4. ВНУТРЕННИЙ БЫСТРЫЙ ЦИКЛ ПО УЭС
        for irho_bh = 1:length(rho_bh_arr)
            rho_bh = rho_bh_arr(irho_bh);
            for irho_pz = 1:length(rho_pz_arr)
                rho_pz = rho_pz_arr(irho_pz);
                for irho_t = 1:length(rho_t_arr)
                    rho_t = rho_t_arr(irho_t);
                    
                    % Мгновенный расчет точки (возвращает стандартную матрицу MATLAB [N_modes x N_electrodes])
                    res = bk_engine_mex('solve', hSolver, rho_bh, rho_pz, rho_t);
                    
                    % В res содержатся:
                    % - токи I [А] для CONNECTED электродов
                    % - потенциалы V [В] для FLOATING электродов
                end
            end
        end
    end
end

% 5. Уничтожение солвера
bk_engine_mex('destroy', hSolver);
```

---

### Вариант Б: Пакетный векторный расчет (Batch) через MEX

Для максимальной скорости можно передавать целые векторы параметров за один вызов:

```matlab
bk_engine_mex('set_geom', hSolver, 0.108, 0.400);

% Векторы параметров одинаковой длины N_points
rho_bh_vec = [1.0; 1.0; 1.0];
rho_pz_vec = [5.0; 10.0; 20.0];
rho_t_vec  = [10.0; 50.0; 100.0];

% Возвращает 3D-матрицу размера [N_points x N_modes x N_electrodes]
results_batch = bk_engine_mex('solve_batch', hSolver, rho_bh_vec, rho_pz_vec, rho_t_vec);
```

---

### Вариант В: Использование стандартного `calllib` (C API)

```matlab
% Загрузка библиотеки
if ~libisloaded('BK_Engine')
    loadlibrary('BK_Engine.dll', 'bk_engine_api.h');
end

gridParams = struct('auto_grid', int32(1), 'dr_start', 0.005, 'q_r', 1.15, ...
                    'r_max', 30.0, 'dz_tool', 0.05, 'dz_fine', 0.005);
pHandle = libpointer('voidPtr');
calllib('BK_Engine', 'BK_CreateSolverFromConfigFile', 'sonde_config.json', gridParams, pHandle);
hSolver = pHandle.Value;

pNumEls = libpointer('int32Ptr', 0);
pNumModes = libpointer('int32Ptr', 0);
calllib('BK_Engine', 'BK_GetProbeInfo', hSolver, pNumEls, pNumModes);
nElectrodes = double(pNumEls.Value);
nModes = double(pNumModes.Value);
totalValues = nModes * nElectrodes;

outBuf = zeros(totalValues, 1);
pOut = libpointer('doublePtr', outBuf);

% Установка геометрии
calllib('BK_Engine', 'BK_SetMediumGeometry', hSolver, 0.108, 0.400);

% Расчет УЭС
calllib('BK_Engine', 'BK_SolveResistivity', hSolver, 1.0, 10.0, 50.0, pOut, int32(totalValues));
results_matrix = reshape(pOut.Value, [nElectrodes, nModes])';

calllib('BK_Engine', 'BK_DestroySolver', hSolver);
```

---

### Пример 3. Извлечение 2D-полей $V(r, z)$ и построение изолиний в MATLAB

```matlab
% Через MEX:
[V_2D, Jr_2D, Jz_2D, r_nodes, z_nodes] = bk_engine_mex('get_field_maps', hSolver, int32(0));

figure;
pcolor(z_nodes, r_nodes, log10(abs(V_2D) + 1e-6));
shading interp; colorbar;
xlabel('Z, м'); ylabel('R, м');
title('Распределение электрического потенциала log10(V)');
```

---

## 4. Конфигурация зонда (`sonde_config.json`)

```json
{
  "probe_name": "BK3_Standard_120M",
  "tool_radius_m": 0.045,
  "electrodes": [
    { "name": "A0", "z_start": 0.00, "length": 0.20 },
    { "name": "M1", "z_start": 0.35, "length": 0.05 },
    { "name": "N1", "z_start": 0.60, "length": 0.05 },
    { "name": "M2", "z_start": 0.90, "length": 0.05 },
    { "name": "N2", "z_start": 1.20, "length": 0.05 },
    { "name": "A1", "z_start": 1.50, "length": 0.50 },
    { "name": "B0", "z_start": 3.00, "length": 1.00 }
  ],
  "modes": [
    {
      "mode_name": "Mode_AL_B1",
      "states": [
        { "electrode": "A0", "state": "CONNECTED", "voltage": 1.0 },
        { "electrode": "M1", "state": "FLOATING" },
        { "electrode": "N1", "state": "FLOATING" },
        { "electrode": "M2", "state": "FLOATING" },
        { "electrode": "N2", "state": "FLOATING" },
        { "electrode": "A1", "state": "CONNECTED", "voltage": 1.0 },
        { "electrode": "B0", "state": "CONNECTED", "voltage": 0.0 }
      ]
    }
  ]
}
```

---

## 5. Чек-лист согласования с разработчиком MATLAB

1. **2-уровневая схема (`set_geom` + `solve` / `solve_batch`)**: согласована.
2. **Прямая поддержка MEX-файла (`bk_engine_mex.mexw64`)**: согласована и включена в проект.
3. **Формат JSON для конфигурации зондов**: согласован.
