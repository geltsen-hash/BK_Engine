# Руководство по интеграции BK_Engine.dll в MATLAB

Данный документ содержит полное описание API библиотеки **`BK_Engine.dll`**, инструкции по сборке MEX-модуля **`bk_engine_mex`**, а также готовые примеры использования в среде **MATLAB** через два способа:
1. **Нативный MEX-модуль (`bk_engine_mex`)** — максимальное быстродействие и простой синтаксис (без указателей).
2. **Прямой вызов C DLL (`loadlibrary` / `calllib`)** — не требует компилятора MATLAB MEX.

---

## 1. Состав дистрибутива

- **`BK_Engine.dll`** — 64-битная динамическая библиотека расчета прямой задачи БК.
- **`BK_Engine.lib`** — библиотека импорта для линковки MEX-файла.
- **`bk_engine_api.h`** — C-заголовочный файл API.
- **`bk_engine_mex.cpp`** — исходный код MEX-обертки для MATLAB.
- **`sample_sonde_bk3.json`** — пример конфигурации прибора БК.

---

## 2. Способ №1: Использование через MEX-модуль (`bk_engine_mex`) — Рекомендуется

### 2.1. Компиляция MEX-модуля в MATLAB

В командной строке MATLAB перейдите в папку с файлами и выполните команду `mex`:

```matlab
% Компиляция MEX-файла (требуется наличие сконфигурированного C++ компилятора, например MSVC)
mex -O -I. -L. -lBK_Engine bk_engine_mex.cpp -output bk_engine_mex
```

После выполнения будет создан файл **`bk_engine_mex.mexw64`**.

---

### 2.2. Команды MEX-интерфейса

| Команда | Синтаксис MATLAB | Описание |
| :--- | :--- | :--- |
| **`create`** | `hSolver = bk_engine_mex('create', 'sonde.json');` | Создает экземпляр солвера из JSON конфигурации прибора |
| **`set_geom`** | `bk_engine_mex('set_geom', hSolver, r_bh, r_pz);` | Задает геометрию скважины и зоны проникновения (перестраивает сетку) |
| **`solve`** | `res = bk_engine_mex('solve', hSolver, rho_bh, rho_pz, rho_t);` | Рассчитывает одну точку среды. Возвращает матрицу `[N_els x N_modes]` |
| **`batch_solve`** | `res = bk_engine_mex('batch_solve', hSolver, rho_bh, rho_pz, rho_t);` | Пакетный расчет массива точек. Возвращает `[N_els x N_modes x N_points]` |
| **`probe_info`** | `[n_els, n_modes] = bk_engine_mex('probe_info', hSolver);` | Возвращает количество электродов и режимов |
| **`grid_dims`** | `[nr, nz] = bk_engine_mex('grid_dims', hSolver);` | Возвращает размеры сетки $N_r \times N_z$ |
| **`grid_nodes`**| `[r_nodes, z_nodes] = bk_engine_mex('grid_nodes', hSolver);` | Возвращает векторы координат узлов сетки |
| **`field_maps`**| `[V_2D, J_2D] = bk_engine_mex('field_maps', hSolver, mode_idx);` | Возвращает матрицы 2D полей потенциала $V(r,z)$ и плотности тока $J(r,z)$ |
| **`destroy`** | `bk_engine_mex('destroy', hSolver);` | Освобождает память солвера |

---

### 2.3. Полный пример скрипта MATLAB с использованием MEX

```matlab
%% Пример расчета палетки через MEX-модуль bk_engine_mex
clear; clc;

% 1. Создание солвера
hSolver = bk_engine_mex('create', 'sample_sonde_bk3.json');

[n_els, n_modes] = bk_engine_mex('probe_info', hSolver);
fprintf('Инициализирован прибор: %d электродов, %d режимов питания.\n', n_els, n_modes);

% 2. Задание параметров среды
r_bh   = 0.108;  % Радиус скважины [м]
r_pz   = 0.400;  % Радиус зоны проникновения [м]
rho_bh = 1.0;    % УЭС промывочной жидкости [Ом*м]
rho_pz = 10.0;   % УЭС зоны проникновения [Ом*м]
rho_t  = 50.0;   % УЭС неизмененного пласта [Ом*м]

% 3. Установка геометрии (выполняется только при смене радиусов)
bk_engine_mex('set_geom', hSolver, r_bh, r_pz);

% 4. Одиночный расчет
tic;
results = bk_engine_mex('solve', hSolver, rho_bh, rho_pz, rho_t);
t_single = toc;

fprintf('Одиночный расчет выполнен за %.2f мс.\n', t_single * 1000);
disp('Результаты по электродам (Токи [А] для токовых, Потенциалы [В] для измерительных):');
disp(results);

% 5. Пакетный расчет сетки УЭС (Batch Solve)
N_pts = 1000;
rho_bh_vec = ones(1, N_pts) * 1.0;
rho_pz_vec = logspace(-1, 2, N_pts);
rho_t_vec  = logspace(0, 3, N_pts);

tic;
batch_results = bk_engine_mex('batch_solve', hSolver, rho_bh_vec, rho_pz_vec, rho_t_vec);
t_batch = toc;

fprintf('Пакетный расчет %d точек выполнен за %.3f с (%.2f мс/точка, %.1f точек/сек).\n', ...
    N_pts, t_batch, (t_batch / N_pts) * 1000, N_pts / t_batch);

% 6. Получение и визуализация 2D полей
[r_nodes, z_nodes] = bk_engine_mex('grid_nodes', hSolver);
[V_2D, J_2D] = bk_engine_mex('field_maps', hSolver, 0);

figure('Name', '2D Потенциал V(r,z)');
imagesc(r_nodes, z_nodes, V_2D);
colorbar; colormap jet;
xlabel('Радиус r, м'); ylabel('Глубина z, м');
title('Распределение электрического потенциала V(r,z)');

% 7. Освобождение ресурсов
bk_engine_mex('destroy', hSolver);
```

---

## 3. Способ №2: Прямой вызов через C API (`loadlibrary` / `calllib`)

Если в MATLAB не настроен компилятор MEX, можно вызывать функции `BK_Engine.dll` напрямую:

```matlab
%% Пример работы через loadlibrary / calllib
clear; clc;

% 1. Загрузка библиотеки
if ~libisloaded('BK_Engine')
    loadlibrary('BK_Engine.dll', 'bk_engine_api.h');
end

% 2. Создание экземпляра солвера
config_file = 'sample_sonde_bk3.json';
pHandle = libpointer('voidPtr');
pNullGrid = libpointer('BK_GridParamsPtr');

status = calllib('BK_Engine', 'BK_CreateSolverFromConfigFile', config_file, pNullGrid, pHandle);
if ~strcmp(status, 'BK_STATUS_OK')
    err_buf = blanks(512);
    calllib('BK_Engine', 'BK_GetLastErrorMessage', libpointer('voidPtr'), err_buf, 512);
    error(['Ошибка создания солвера: ', strtrim(err_buf)]);
end
hSolver = pHandle.Value;

% 3. Получение конфигурации прибора
pNumEls = libpointer('int32Ptr', 0);
pNumModes = libpointer('int32Ptr', 0);
calllib('BK_Engine', 'BK_GetProbeInfo', hSolver, pNumEls, pNumModes);
N_els = double(pNumEls.Value);
N_modes = double(pNumModes.Value);

% 4. Задание геометрии среды
r_bh = 0.108;
r_pz = 0.400;
calllib('BK_Engine', 'BK_SetMediumGeometry', hSolver, r_bh, r_pz);

% 5. Расчет точки среды
rho_bh = 1.0; rho_pz = 10.0; rho_t = 50.0;
total_vals = N_els * N_modes;
out_vals = zeros(1, total_vals);
pOut = libpointer('doublePtr', out_vals);

calllib('BK_Engine', 'BK_SolveResistivity', hSolver, rho_bh, rho_pz, rho_t, pOut, int32(total_vals));
results = reshape(pOut.Value, [N_els, N_modes]);

% 6. Освобождение памяти
calllib('BK_Engine', 'BK_DestroySolver', hSolver);
```

---

## 4. Формат выходных данных

- Для электродов в состоянии **`CONNECTED`** (токовые/заземляющие электроды $A, B$): возвращается **полный ток электрода $I$ в Амперах [А]** (положительный для источника, отрицательный для стока).
- Для электродов в состоянии **`FLOATING`** (измерительные электроды $M, N$): возвращается **средний потенциал электрода $V$ в Вольтах [В]**.
- Разность потенциалов между парными измерительными электродами: $\Delta U_{MN} = V_M - V_N$.
- Кажущееся сопротивление зонда:
  $$\rho_k = K \cdot \frac{V_M - V_N}{I_A}$$
  где $K$ — геометрический коэффициент зонда, $I_A$ — ток соответствующего питающего электрода.

---

## 5. Формат JSON-конфигурации зонда

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
