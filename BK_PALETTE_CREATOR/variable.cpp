#include "pch.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include "variable.h"

using namespace std;


/////////////////////////////////////////////////////////////////

//Целевая функция с нормализацией на стандартное отклонение аппаратной погрешности(Data covariance weighting):
//F = sum(W_a * ((Ia_calc[i] - Ia_meas[i]) / sigma_Ia[i]) ^ 2 + W_b * ((Ib_calc[i] - Ib_meas[i]) / sigma_Ib[i]) ^ 2 + W_u * ((dU_calc[i] - dU_meas[i]) / sigma_dU[i]) ^ 2)где sigma = abs(meas[i]) * rel_err + abs_err
double calculate_snr_misfit(const BK& calc, const BK& meas, const BK_Errors& errors, const BK_Weights& weights) {
	double total_error = 0.0;
	for (int idx = 0; idx < 4; ++idx) {
		// Ожидаемое отклонение (sigma) на основе аппаратной точности: шумовая полка + % от сигнала
		double sigma_Ia = fabs(meas.Ia[idx]) * errors.Ia.rel_err + errors.Ia.abs_err;
		double sigma_Ib = fabs(meas.Ib[idx]) * errors.Ib.rel_err + errors.Ib.abs_err;
		double sigma_dU = fabs(meas.dU[idx]) * errors.dU.rel_err + errors.dU.abs_err;
		// Нормировка разницы на аппаратную погрешность
		double err_Ia = (calc.Ia[idx] - meas.Ia[idx]) / sigma_Ia;
		double err_Ib = (calc.Ib[idx] - meas.Ib[idx]) / sigma_Ib;
		double err_dU = (calc.dU[idx] - meas.dU[idx]) / sigma_dU;
		// Суммируем квадраты взвешенных невязок
		total_error += weights.W_a * (err_Ia * err_Ia) +
			weights.W_b * (err_Ib * err_Ib) +
			weights.W_u * (err_dU * err_dU);
	}
	return total_error;
}

//Целевая логарифмическая функция с учетом аппаратной погрешности : F = sum(W_a * (delta_log_Ia / sigma_log_Ia) ^ 2 + W_b * (delta_log_Ib / sigma_log_Ib) ^ 2 + W_u * (delta_log_dU / sigma_log_dU) ^ 2)
//где отклонение : delta_log_P = log10(abs(P_calc) + eps) - log10(abs(P_meas) + eps)
//локальная линейная погрешность : sigma_P = abs(P_meas) * rel_err + abs_err
//перевод погрешности в логарифмический масштаб : sigma_log_P = log10(abs(P_meas) + sigma_P + eps) - log10(abs(P_meas) + eps)
double calculate_log_snr_misfit(const BK& calc, const BK& meas, const BK_Errors& errors, const BK_LocalWeights& local_weights) {
	double total_error = 0.0;
	const double eps = 1e-12;
	for (int idx = 0; idx < 4; ++idx) {
		double sigma_Ia = fabs(meas.Ia[idx]) * errors.Ia.rel_err + errors.Ia.abs_err;
		double sigma_Ib = fabs(meas.Ib[idx]) * errors.Ib.rel_err + errors.Ib.abs_err;
		double sigma_dU = fabs(meas.dU[idx]) * errors.dU.rel_err + errors.dU.abs_err;
		double sigma_log_Ia = log10(fabs(meas.Ia[idx]) + sigma_Ia + eps) - log10(fabs(meas.Ia[idx]) + eps);
		double sigma_log_Ib = log10(fabs(meas.Ib[idx]) + sigma_Ib + eps) - log10(fabs(meas.Ib[idx]) + eps);
		double sigma_log_dU = log10(fabs(meas.dU[idx]) + sigma_dU + eps) - log10(fabs(meas.dU[idx]) + eps);
		double delta_log_Ia = log10(fabs(calc.Ia[idx]) + eps) - log10(fabs(meas.Ia[idx]) + eps);
		double delta_log_Ib = log10(fabs(calc.Ib[idx]) + eps) - log10(fabs(meas.Ib[idx]) + eps);
		double delta_log_dU = log10(fabs(calc.dU[idx]) + eps) - log10(fabs(meas.dU[idx]) + eps);
		double err_Ia = delta_log_Ia / (sigma_log_Ia + eps);
		double err_Ib = delta_log_Ib / (sigma_log_Ib + eps);
		double err_dU = delta_log_dU / (sigma_log_dU + eps);
		// Используем индивидуальный вес для каждого зонда (idx) и каждого типа сигнала
		total_error += local_weights.W_a[idx] * (err_Ia * err_Ia) +
			local_weights.W_b[idx] * (err_Ib * err_Ib) +
			local_weights.W_u[idx] * (err_dU * err_dU);
	}
	return total_error;
}


// Структуры для задания границ

// Вспомогательная функция перевода физических границ в индексы массива
void get_index_bounds(const vector<double>& nodes, const Limit& limit, size_t max_idx, size_t& out_start, size_t& out_end) {
	if (nodes.empty()) {
		out_start = 0; out_end = max_idx;
		return;
	}
	// Находим первый элемент >= min_val
	auto it_start = lower_bound(nodes.begin(), nodes.end(), limit.min_val);
	out_start = distance(nodes.begin(), it_start);

	// Находим первый элемент > max_val, и берем предыдущий
	auto it_end = upper_bound(nodes.begin(), nodes.end(), limit.max_val);
	out_end = distance(nodes.begin(), it_end);
	if (out_end > 0) out_end--; // Включаем верхнюю границу

	// Защита от выхода за пределы
	if (out_start > max_idx) out_start = max_idx;
	if (out_end > max_idx) out_end = max_idx;
	if (out_start > out_end) out_end = out_start; // Если диапазон пуст
}


// Функция поиска минимума с интегрированной локальной 5D-интерполяцией
SearchResult find_best_match(const Vector_BK_& db, const BK& meas, MisfitType mt, WeightType wt, const SearchConstraints& bounds) {
	SearchResult result;
	result.min_error = numeric_limits<double>::infinity();
	const double eps = 1e-12;
	// 1. Перевод физических ограничений в индексы сетки
	size_t m_start = 0, m_end = 0, i_start = 0, i_end = 0, j_start = 0, j_end = 0, k_start = 0, k_end = 0, l_start = 0, l_end = 0;

	get_index_bounds(db.Ro_BH_nodes, bounds.Ro_BH, db.n_Ro_BH > 0 ? db.n_Ro_BH - 1 : 0, m_start, m_end);
	get_index_bounds(db.R_BH_nodes, bounds.R_BH, db.n_R_BH > 0 ? db.n_R_BH - 1 : 0, i_start, i_end);
	get_index_bounds(db.R_PZ_nodes, bounds.R_PZ, db.n_R_PZ > 0 ? db.n_R_PZ - 1 : 0, j_start, j_end);
	get_index_bounds(db.Ro_PZ_nodes, bounds.Ro_PZ, db.n_Ro_PZ > 0 ? db.n_Ro_PZ - 1 : 0, k_start, k_end);
	get_index_bounds(db.Ro_stratum_nodes, bounds.Ro_stratum, db.n_Ro_stratum > 0 ? db.n_Ro_stratum - 1 : 0, l_start, l_end);
	size_t best_idx = 0;
	// 2. Поиск только по разрешенным границам
	size_t slice_3d = db.n_R_PZ * db.n_Ro_PZ * db.n_Ro_stratum;
	size_t slice_4d = db.n_R_BH * slice_3d;
	for (size_t m = m_start; m <= m_end; ++m) {
		for (size_t i = i_start; i <= i_end; ++i) {
			for (size_t j = j_start; j <= j_end; ++j) {
				for (size_t k = k_start; k <= k_end; ++k) {
					for (size_t l = l_start; l <= l_end; ++l) {

						size_t idx = m * slice_4d + i * slice_3d + j * (db.n_Ro_PZ * db.n_Ro_stratum) + k * db.n_Ro_stratum + l;
						const BK& calc = db.data_5d[idx];
						double current_error = 0.0;
						for (int ch = 0; ch < 4; ++ch) {
							double w_a = (wt == WeightType::Local) ? db.weights_5d[idx].W_a[ch] : db.weights.W_a;
							double w_b = (wt == WeightType::Local) ? db.weights_5d[idx].W_b[ch] : db.weights.W_b;
							double w_u = (wt == WeightType::Local) ? db.weights_5d[idx].W_u[ch] : db.weights.W_u;
							double sig_a = fabs(meas.Ia[ch]) * db.errors.Ia.rel_err + db.errors.Ia.abs_err;
							double sig_b = fabs(meas.Ib[ch]) * db.errors.Ib.rel_err + db.errors.Ib.abs_err;
							double sig_u = fabs(meas.dU[ch]) * db.errors.dU.rel_err + db.errors.dU.abs_err;
							double err_a, err_b, err_u;
							if (mt == MisfitType::Simple) {
								err_a = (calc.Ia[ch] - meas.Ia[ch]) / sig_a;
								err_b = (calc.Ib[ch] - meas.Ib[ch]) / sig_b;
								err_u = (calc.dU[ch] - meas.dU[ch]) / sig_u;
							}
							else {
								double sl_a = log10(fabs(meas.Ia[ch]) + sig_a + eps) - log10(fabs(meas.Ia[ch]) + eps);
								double sl_b = log10(fabs(meas.Ib[ch]) + sig_b + eps) - log10(fabs(meas.Ib[ch]) + eps);
								double sl_u = log10(fabs(meas.dU[ch]) + sig_u + eps) - log10(fabs(meas.dU[ch]) + eps);
								err_a = (log10(fabs(calc.Ia[ch]) + eps) - log10(fabs(meas.Ia[ch]) + eps)) / (sl_a + eps);
								err_b = (log10(fabs(calc.Ib[ch]) + eps) - log10(fabs(meas.Ib[ch]) + eps)) / (sl_b + eps);
								err_u = (log10(fabs(calc.dU[ch]) + eps) - log10(fabs(meas.dU[ch]) + eps)) / (sl_u + eps);
							}
							current_error += w_a * (err_a * err_a) + w_b * (err_b * err_b) + w_u * (err_u * err_u);
						}
						if (current_error < result.min_error) {
							result.min_error = current_error;
							best_idx = idx;
							result.best_match = calc;
							result.m = m; result.i = i; result.j = j; result.k = k; result.l = l;
						}
					}
				}
			}
		}
	}
	// Если ничего не найдено (диапазон ошибочный), возвращаем бесконечность
	if (result.min_error == numeric_limits<double>::infinity()) {
		return result;
	}
	// ==========================================
	// ЭТАП 2: Локальная 5D-интерполяция (Refinement)
	// ==========================================
	// Лямбда-функция для оценки невязки в заданных узлах (с защитой границ)
	auto eval_misfit = [&](int m, int i, int j, int k, int l) -> double {
		m = clamp(m, 0, (int)db.n_Ro_BH - 1);
		i = clamp(i, 0, (int)db.n_R_BH - 1);
		j = clamp(j, 0, (int)db.n_R_PZ - 1);
		k = clamp(k, 0, (int)db.n_Ro_PZ - 1);
		l = clamp(l, 0, (int)db.n_Ro_stratum - 1);

		size_t idx = m * slice_4d + i * slice_3d + j * (db.n_Ro_PZ * db.n_Ro_stratum) + k * db.n_Ro_stratum + l;
		const BK& c = db.data_5d[idx];
		double err_sum = 0.0;

		for (int ch = 0; ch < 4; ++ch) {
			double w_a = (wt == WeightType::Local) ? db.weights_5d[idx].W_a[ch] : db.weights.W_a;
			double w_b = (wt == WeightType::Local) ? db.weights_5d[idx].W_b[ch] : db.weights.W_b;
			double w_u = (wt == WeightType::Local) ? db.weights_5d[idx].W_u[ch] : db.weights.W_u;
			double sig_a = fabs(meas.Ia[ch]) * db.errors.Ia.rel_err + db.errors.Ia.abs_err;
			double sig_b = fabs(meas.Ib[ch]) * db.errors.Ib.rel_err + db.errors.Ib.abs_err;
			double sig_u = fabs(meas.dU[ch]) * db.errors.dU.rel_err + db.errors.dU.abs_err;
			double err_a, err_b, err_u;
			if (mt == MisfitType::Simple) {
				err_a = (c.Ia[ch] - meas.Ia[ch]) / sig_a;
				err_b = (c.Ib[ch] - meas.Ib[ch]) / sig_b;
				err_u = (c.dU[ch] - meas.dU[ch]) / sig_u;
			}
			else {
				double sl_a = log10(fabs(meas.Ia[ch]) + sig_a + eps) - log10(fabs(meas.Ia[ch]) + eps);
				double sl_b = log10(fabs(meas.Ib[ch]) + sig_b + eps) - log10(fabs(meas.Ib[ch]) + eps);
				double sl_u = log10(fabs(meas.dU[ch]) + sig_u + eps) - log10(fabs(meas.dU[ch]) + eps);
				err_a = (log10(fabs(c.Ia[ch]) + eps) - log10(fabs(meas.Ia[ch]) + eps)) / (sl_a + eps);
				err_b = (log10(fabs(c.Ib[ch]) + eps) - log10(fabs(meas.Ib[ch]) + eps)) / (sl_b + eps);
				err_u = (log10(fabs(c.dU[ch]) + eps) - log10(fabs(meas.dU[ch]) + eps)) / (sl_u + eps);
			}
			err_sum += w_a * (err_a * err_a) + w_b * (err_b * err_b) + w_u * (err_u * err_u);
		}
		return err_sum;
	};
	// Лямбда для расчета дробного смещения по вершине параболы
	auto get_offset = [&](double E_L, double E_C, double E_R) -> double {
		double denom = E_L - 2.0 * E_C + E_R;
		if (fabs(denom) < eps) return 0.0;
		return clamp(0.5 * (E_L - E_R) / denom, -0.5, 0.5);
	};
	// Лямбда для обычной (линейной) интерполяции (хорошо подходит для радиусов R_BH, R_PZ)
	auto interp_linear = [](const vector<double>& grid, double f_idx) -> double {
		if (grid.empty()) return 0.0;
		if (f_idx <= 0.0) return grid.front();
		if (f_idx >= grid.size() - 1) return grid.back();
		int base = static_cast<int>(floor(f_idx));
		double fraction = f_idx - base;
		return grid[base] + fraction * (grid[base + 1] - grid[base]);
	};
	// Лямбда для экспоненциальной (логарифмической) интерполяции (ОБЯЗАТЕЛЬНО для удельных сопротивлений Ro)
	auto interp_log = [](const vector<double>& grid, double f_idx) -> double {
		if (grid.empty()) return 0.0;
		if (f_idx <= 0.0) return grid.front();
		if (f_idx >= grid.size() - 1) return grid.back();
		int base = static_cast<int>(floor(f_idx));
		double fraction = f_idx - base;
		// Математика: y = y0 * (y1 / y0)^fraction
		return grid[base] * pow(grid[base + 1] / grid[base], fraction);
	};

	int cm = result.m, ci = result.i, cj = result.j, ck = result.k, cl = result.l;
	double EC = result.min_error;
	// Вычисление смещений (оценка соседних узлов по гиперкубу)
	result.m_float = cm + get_offset(eval_misfit(cm - 1, ci, cj, ck, cl), EC, eval_misfit(cm + 1, ci, cj, ck, cl));
	result.i_float = ci + get_offset(eval_misfit(cm, ci - 1, cj, ck, cl), EC, eval_misfit(cm, ci + 1, cj, ck, cl));
	result.j_float = cj + get_offset(eval_misfit(cm, ci, cj - 1, ck, cl), EC, eval_misfit(cm, ci, cj + 1, ck, cl));
	result.k_float = ck + get_offset(eval_misfit(cm, ci, cj, ck - 1, cl), EC, eval_misfit(cm, ci, cj, ck + 1, cl));
	result.l_float = cl + get_offset(eval_misfit(cm, ci, cj, ck, cl - 1), EC, eval_misfit(cm, ci, cj, ck, cl + 1));
	
	// Если был выбран логарифмический масштаб невязки, логично физику сопротивлений тоже интерполировать логарифмически
	if (mt == MisfitType::Logarithmic) {
		result.Ro_BH = interp_log(db.Ro_BH_nodes, result.m_float);
		result.Ro_PZ = interp_log(db.Ro_PZ_nodes, result.k_float);
		result.Ro_stratum = interp_log(db.Ro_stratum_nodes, result.l_float);
		result.R_BH = interp_log(db.R_BH_nodes, result.i_float);
		result.R_PZ = interp_log(db.R_PZ_nodes, result.j_float);
	}
	else {
		// При простой линейной невязке все интерполируем линейно
		result.Ro_BH = interp_linear(db.Ro_BH_nodes, result.m_float);
		result.R_BH = interp_linear(db.R_BH_nodes, result.i_float);
		result.R_PZ = interp_linear(db.R_PZ_nodes, result.j_float);
		result.Ro_PZ = interp_linear(db.Ro_PZ_nodes, result.k_float);
		result.Ro_stratum = interp_linear(db.Ro_stratum_nodes, result.l_float);
	}


	// ОПЦИОНАЛЬНО: жесткое отсечение (clamping) интерполированных значений по заданным границам
	result.Ro_BH = clamp(result.Ro_BH, bounds.Ro_BH.min_val, bounds.Ro_BH.max_val);
	result.R_BH = clamp(result.R_BH, bounds.R_BH.min_val, bounds.R_BH.max_val);
	result.R_PZ = clamp(result.R_PZ, bounds.R_PZ.min_val, bounds.R_PZ.max_val);
	result.Ro_PZ = clamp(result.Ro_PZ, bounds.Ro_PZ.min_val, bounds.Ro_PZ.max_val);
	result.Ro_stratum = clamp(result.Ro_stratum, bounds.Ro_stratum.min_val, bounds.Ro_stratum.max_val);
	
	return result;
}


