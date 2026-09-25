%% Демонстрационный скрипт использования MEX-интерфейса BK_Engine
clear; clc; close all;

fprintf('=== Тестирование MEX-модуля bk_engine_mex ===\n');

% 1. Создание экземпляра солвера
config_file = 'sample_sonde_bk3.json';
hSolver = bk_engine_mex('create', config_file);

% 2. Информация о приборе
[n_els, n_modes] = bk_engine_mex('probe_info', hSolver);
fprintf('Прибор успешно загружен: %d электродов, %d режимов питания.\n', n_els, n_modes);

% 3. Геометрия среды Дахнова
r_bh   = 0.108;  % Радиус скважины, м
r_pz   = 0.400;  % Радиус зоны проникновения, м
rho_bh = 1.0;    % УЭС промывочной жидкости, Ом*м
rho_pz = 10.0;   % УЭС зоны проникновения, Ом*м
rho_t  = 50.0;   % УЭС неизмененного пласта, Ом*м

% Установка геометрии (перестроение сетки)
bk_engine_mex('set_geom', hSolver, r_bh, r_pz);

% 4. Одиночный расчет
tic;
results = bk_engine_mex('solve', hSolver, rho_bh, rho_pz, rho_t);
t_single_ms = toc * 1000.0;

fprintf('Одиночный расчет выполнен за %.2f мс.\n', t_single_ms);
fprintf('Результаты (Токи [А] / Потенциалы [В]):\n');
disp(results);

% 5. Пакетный расчет 500 точек (Batch Solve)
N_pts = 500;
rho_bh_vec = ones(1, N_pts) * 1.0;
rho_pz_vec = logspace(-1, 2, N_pts);
rho_t_vec  = logspace(0, 3, N_pts);

tic;
batch_results = bk_engine_mex('batch_solve', hSolver, rho_bh_vec, rho_pz_vec, rho_t_vec);
t_batch_s = toc;

fprintf('Пакетный расчет %d точек выполнен за %.3f с (%.2f мс/точка, %.1f точек/сек).\n', ...
    N_pts, t_batch_s, (t_batch_s / N_pts) * 1000.0, N_pts / t_batch_s);

% 6. Получение координат сетки и 2D полей
[r_nodes, z_nodes] = bk_engine_mex('grid_nodes', hSolver);
[V_2D, J_2D] = bk_engine_mex('field_maps', hSolver, 0);

figure('Name', '2D Поле потенциала БК', 'NumberTitle', 'off');
imagesc(r_nodes, z_nodes, V_2D);
colorbar; colormap jet;
xlabel('Радиус r, м'); ylabel('Глубина z, м');
title('Распределение потенциала V(r,z)');

% 7. Уничтожение солвера и освобождение памяти
bk_engine_mex('destroy', hSolver);
fprintf('Ресурсы успешно освобождены.\n');
