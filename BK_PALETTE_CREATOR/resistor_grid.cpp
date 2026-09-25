#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include "variable.h"
#include "resistor_grid.h"

using namespace std;

vector<double> buildRadialGridAdaptive(double r_tool, vector<double> boundaries, double dr_start, double q_r) {
	vector<double> r_nodes;
	double current_r = r_tool;
	double current_dr = dr_start;
	r_nodes.push_back(current_r);
	// Обязательно сортируем границы по возрастанию на всякий случай
	sort(boundaries.begin(), boundaries.end());
	for (size_t i = 0; i < boundaries.size(); ++i) {
		double target_r = boundaries[i];

		// Если граница меньше или равна текущему радиусу (например, пользователь задал ЗП равной скважине) - пропускаем
		if (target_r <= current_r) continue;
		while (current_r < target_r) {
			// Если следующий логарифмический шаг перелетает границу
			if (current_r + current_dr >= target_r) {

				// Защита от создания микроскопически тонких ячеек (вызывающих сбои решателя)
				// Если оставшийся кусочек больше 10% от текущего шага, создаем новый узел
				if (target_r - current_r > current_dr * 0.1) {
					current_r = target_r;
					r_nodes.push_back(current_r);
				}
				else {
					// Если мы подошли вплотную к границе, просто "дотягиваем" последний созданный узел до нее
					r_nodes.back() = target_r;
					current_r = target_r;
				}
				break; // Граница достигнута, переходим к следующему слою
			}
			else {
				// Обычный логарифмический шаг
				current_r += current_dr;
				r_nodes.push_back(current_r);
				current_dr *= q_r; // Увеличиваем шаг
			}
		}
	}
	return r_nodes;
}

vector<double> buildSmartDepthGrid_new(
	const vector<Electrode>& a_els,
	const vector<Electrode>& b_els,
	const vector<MeasElectrode>& m_els,
	double dz_base, double dz_fine, double threshold_len,
	double q_tail, double tail_length)
{
	double z_min = 1e9;
	double z_max = -1e9;
	vector<double> boundaries;

	// Структура для хранения зон, где нужна мелкая сетка
	struct FineZone { double z1, z2; };
	vector<FineZone> fine_zones;
	auto process = [&](const auto& els) {
		for (const auto& el : els) {
			boundaries.push_back(el.z_start_m);
			boundaries.push_back(el.z_end_m);

			if (el.z_start_m < z_min) z_min = el.z_start_m;
			if (el.z_end_m > z_max) z_max = el.z_end_m;
			double len = el.z_end_m - el.z_start_m;
			// Если электрод короче порога - создаем вокруг него зону мелкой сетки (+5 мм буфер по краям)
			if (len <= threshold_len) {
				fine_zones.push_back({ el.z_start_m - 0.005, el.z_end_m + 0.005 });
			}
		}
	};
	process(a_els); process(b_els); process(m_els);
	// Очищаем и сортируем все физические границы
	sort(boundaries.begin(), boundaries.end());
	boundaries.erase(unique(boundaries.begin(), boundaries.end(), [](double a, double b) {
		return abs(a - b) < 1e-6;
	}), boundaries.end());
	// Буфер безопасности всего зонда
	z_min -= 0.5;
	z_max += 0.5;
	vector<double> z_nodes;
	// 1. ВЕРХНИЙ ХВОСТ
	vector<double> top_tail;
	double curr_z = z_min;
	double curr_dz = dz_base;
	while (curr_z - curr_dz >= z_min - tail_length) {
		curr_z -= curr_dz;
		top_tail.push_back(curr_z);
		curr_dz *= q_tail;
	}
	for (int i = (int)top_tail.size() - 1; i >= 0; --i) z_nodes.push_back(top_tail[i]);
	// 2. ЗОНА ПРИБОРА (Умный гибридный шаг с прилипанием)
	curr_z = z_min;
	z_nodes.push_back(curr_z);
	while (curr_z < z_max - 1e-6) {
		double step = dz_base; // По умолчанию идем крупно
		// Проверяем, не зашли ли мы в зону мелкого электрода
		for (const auto& fz : fine_zones) {
			if (curr_z >= fz.z1 - 1e-6 && curr_z <= fz.z2 + 1e-6) {
				step = dz_fine; // Переключаемся на мелкий шаг
				break;
			}
		}
		double next_z = curr_z + step;
		// МАГНИТ К ГРАНИЦАМ: 
		// Если наш шаг перепрыгивает границу любого электрода, мы укорачиваем шаг, чтобы упасть точно на нее.
		for (double b : boundaries) {
			if (b > curr_z + 1e-6 && b <= next_z + 1e-5) {
				next_z = b;
				break; // Границы отсортированы, берем самую ближнюю
			}
		}
		// Защита от зависания при нулевых шагах
		if (next_z - curr_z < 1e-5) next_z = curr_z + 1e-4;
		curr_z = next_z;
		z_nodes.push_back(curr_z);
	}
	// 3. НИЖНИЙ ХВОСТ
	curr_z = z_nodes.back();
	curr_dz = dz_base * q_tail;
	while (curr_z + curr_dz <= z_max + tail_length) {
		curr_z += curr_dz;
		z_nodes.push_back(curr_z);
		curr_dz *= q_tail;
	}
	// 4. Очистка микро-погрешностей double
	vector<double> final_nodes;
	if (!z_nodes.empty()) final_nodes.push_back(z_nodes[0]);
	for (size_t i = 1; i < z_nodes.size(); ++i) {
		if (z_nodes[i] - final_nodes.back() > 1e-5) {
			final_nodes.push_back(z_nodes[i]);
		}
	}
	return final_nodes;
}


// Новый умный генератор сетки по глубине
 vector<double> buildSmartDepthGrid(
	const vector<Electrode>& a_els,
	const vector<Electrode>& b_els,
	const vector<MeasElectrode>& m_els,
	double dz_fine, double q_z, double tail_length)
{
	// 1. Собираем все физические границы электродов
	vector<double> borders;
	for (const auto& el : a_els) { borders.push_back(el.z_start_m); borders.push_back(el.z_end_m); }
	for (const auto& el : b_els) { borders.push_back(el.z_start_m); borders.push_back(el.z_end_m); }
	for (const auto& el : m_els) { borders.push_back(el.z_start_m); borders.push_back(el.z_end_m); }
	// Сортируем и удаляем дубликаты (если электроды стоят впритык)
	sort(borders.begin(), borders.end());
	borders.erase(unique(borders.begin(), borders.end()), borders.end());
	double z_min_tool = borders.front();
	double z_max_tool = borders.back();
	vector<double> z_nodes;
	double buffer_dist = 0.1; // Буфер 15 см вокруг любого края электрода, где сетка не растягивается!
	// 2. Строим ВЕРХНИЙ ХВОСТ (уходит вверх от прибора)
	vector<double> top_tail;
	double curr_z = z_min_tool;
	double curr_dz = dz_fine;
	while (curr_z - curr_dz > z_min_tool - tail_length) {
		curr_z -= curr_dz;
		top_tail.push_back(curr_z);
		if (z_min_tool - curr_z > buffer_dist) curr_dz *= q_z; // Растягиваем только за пределами буфера
	}
	top_tail.push_back(z_min_tool - tail_length);

	// Разворачиваем и добавляем в итоговый массив
	for (int i = (int)top_tail.size() - 1; i >= 0; --i) z_nodes.push_back(top_tail[i]);
	z_nodes.push_back(z_min_tool);
	// 3. Заполняем пустоты МЕЖДУ границами электродов (или сами электроды)
	for (size_t i = 0; i < borders.size() - 1; ++i) {
		double z1 = borders[i];
		double z2 = borders[i + 1];
		double L = z2 - z1;
		if (L <= dz_fine) {
			z_nodes.push_back(z2);
			continue;
		}
		// Считаем симметричное заполнение пустоты
		vector<double> left_half;
		curr_z = z1;
		curr_dz = dz_fine;
		double mid = z1 + L / 2.0;
		while (curr_z + curr_dz < mid) {
			curr_z += curr_dz;
			left_half.push_back(curr_z);
			// Внутри буферной зоны держим мелкий шаг, растягиваем только середину
			if (curr_z - z1 > buffer_dist) curr_dz *= q_z;
		}
		// Добавляем левую половину (разгон)
		for (double val : left_half) z_nodes.push_back(val);
		// Создаем зеркальную правую половину (торможение к следующему электроду)
		for (int j = (int)left_half.size() - 1; j >= 0; --j) {
			double dist_from_z1 = left_half[j] - z1;
			z_nodes.push_back(z2 - dist_from_z1);
		}
		z_nodes.push_back(z2); // Замыкающий узел точно на границе!
	}
	// 4. Строим НИЖНИЙ ХВОСТ (уходит вниз от прибора)
	curr_z = z_max_tool;
	curr_dz = dz_fine;
	while (curr_z + curr_dz < z_max_tool + tail_length) {
		curr_z += curr_dz;
		z_nodes.push_back(curr_z);
		if (curr_z - z_max_tool > buffer_dist) curr_dz *= q_z;
	}
	z_nodes.push_back(z_max_tool + tail_length);
	return z_nodes;
}


vector<double> buildRadialGrid(double r_tool, double r_max, double dr_start, double q_r) {
	vector<double> r_nodes;
	double current_r = r_tool;
	double current_dr = dr_start;
	r_nodes.push_back(current_r);
	while (current_r < r_max) {
		current_r += current_dr;
		r_nodes.push_back(current_r);
		current_dr *= q_r;
	}
	return r_nodes;
}

vector<double> buildDepthGrid(double z_tool_start, double z_tool_end, double dz_tool, double q_z, double tail_length) {
	vector<double> z_nodes;
	vector<double> tail_deltas;
	double current_dz = dz_tool * q_z;
	double sum_tail = 0.0;
	while (sum_tail < tail_length) {
		tail_deltas.push_back(current_dz);
		sum_tail += current_dz;
		current_dz *= q_z;
	}
	double current_z = z_tool_start - sum_tail;
	z_nodes.push_back(current_z);
	for (int i = tail_deltas.size() - 1; i >= 0; --i) {
		current_z += tail_deltas[i];
		z_nodes.push_back(current_z);
	}
	while (current_z < z_tool_end - dz_tool * 0.5) {
		current_z += dz_tool;
		z_nodes.push_back(current_z);
	}
	for (size_t i = 0; i < tail_deltas.size(); ++i) {
		current_z += tail_deltas[i];
		z_nodes.push_back(current_z);
	}
	return z_nodes;
}

void getJetColor(double v, double vmin, double vmax, unsigned char& r, unsigned char& g, unsigned char& b) {
	if (v < vmin) v = vmin;
	if (v > vmax) v = vmax;
	double dv = vmax - vmin;
	if (dv == 0.0) { r = 255; g = 255; b = 255; return; }
	double c = (v - vmin) / dv;
	r = static_cast<unsigned char>(max(0.0, min(1.0, 1.5 - abs(1.0 - 4.0 * (c - 0.5)))) * 255);
	g = static_cast<unsigned char>(max(0.0, min(1.0, 1.5 - abs(1.0 - 4.0 * c))) * 255);
	b = static_cast<unsigned char>(max(0.0, min(1.0, 1.5 - abs(1.0 - 4.0 * (c + 0.5)))) * 255);
}

	int  ResistorGrid::getZIndex(double target_z) const {
		auto it = lower_bound(z_coords.begin(), z_coords.end(), target_z);
		if (it == z_coords.end()) return H - 1;
		if (it == z_coords.begin()) return 0;
		if (*it - target_z < target_z - *(it - 1)) return distance(z_coords.begin(), it);
		return distance(z_coords.begin(), it - 1);
	};

	int ResistorGrid::getRIndex(double target_r) const {
		auto it = lower_bound(r_coords.begin(), r_coords.end(), target_r);
		if (it == r_coords.end()) return W - 1;
		if (it == r_coords.begin()) return 0;
		if (*it - target_r < target_r - *(it - 1)) return distance(r_coords.begin(), it);
		return distance(r_coords.begin(), it - 1);
	};

	double ResistorGrid::getRhoForRadius(double r, const vector<LayerDakhnov>& zones) const {
		for (const auto& zone : zones) if (r <= zone.r_boundary) return zone.rho;
		return zones.empty() ? 100.0 : zones.back().rho;
	};

	ResistorGrid::ResistorGrid(const vector<double>& r_nodes, const vector<double>& z_nodes) {
		r_coords = r_nodes; z_coords = z_nodes;
		W = r_coords.size(); H = z_coords.size(); N = W * H;
		v.assign(N, 0.0); R_hor.assign(N, 1.0); R_ver.assign(N, 1.0);
		is_fixed.assign(N, 0); fixed_v.assign(N, 0.0);
	};

	void ResistorGrid::fillModel(const vector<LayerDakhnov>& zones) {
		const double PI = 3.14159265358979323846;
		for (int z = 0; z < H; z++) {
			double h_eff;
			if (z == 0) h_eff = (z_coords[1] - z_coords[0]) / 2.0;
			else if (z == H - 1) h_eff = (z_coords[H - 1] - z_coords[H - 2]) / 2.0;
			else h_eff = (z_coords[z + 1] - z_coords[z - 1]) / 2.0;
			for (int r = 0; r < W; r++) {
				int idx = z * W + r;
				double curr_r = r_coords[r];
				if (r < W - 1) {
					double next_r = r_coords[r + 1];
					R_hor[idx] = (getRhoForRadius((curr_r + next_r) / 2.0, zones) / (2.0 * PI * h_eff)) * log(next_r / curr_r);
				}
				if (z < H - 1) {
					if (r == 0) R_ver[idx] = 1e6;
					else {
						double dz = z_coords[z + 1] - z_coords[z];
						double r_inner = (r_coords[r] + r_coords[r - 1]) / 2.0;
						double r_outer = (r == W - 1) ? curr_r + (curr_r - r_coords[r - 1]) / 2.0 : (r_coords[r + 1] + curr_r) / 2.0;
						R_ver[idx] = (getRhoForRadius(curr_r, zones) * dz) / (PI * (r_outer * r_outer - r_inner * r_inner));
					}
				}
			}
		}
	};

	void ResistorGrid::setSonde(const vector<Electrode>& a_els, const vector<Electrode>& b_els, const vector<MeasElectrode>& m_els) {
		saved_m_els = m_els;
		auto apply_fixed = [&](const Electrode& el) {
			for (int z = getZIndex(el.z_start_m); z <= getZIndex(el.z_end_m); z++) {
				fixed_v[z * W] = el.voltage; is_fixed[z * W] = 1; v[z * W] = el.voltage;
			}
		};
		for (const auto& a : a_els) apply_fixed(a);
		for (const auto& b : b_els) apply_fixed(b);

		//В матрице размером W(узлов по радиусу) на H(узлов по глубине),
		//индекс W - 1 соответствует самому последнему столбцу узлов, который физически
		//находится на расстоянии r_max от оси скважины.Цикл пробегает сверху вниз по всей высоте модели
		//(от z = 0 до H) и жестко привязывает потенциал каждого крайнего правого пикселя к нулю.
		for (int z = 0; z < H; z++) {
			fixed_v[z * W + (W - 1)] = 0.0; // Задаем потенциал 0 Вольт
			is_fixed[z * W + (W - 1)] = 1; // Жестко фиксируем этот узел (решатель не будет его менять)
			v[z * W + (W - 1)] = 0.0;  // Обновляем текущее значение
		}
		for (const auto& m : saved_m_els) {
			for (int z = getZIndex(m.z_start_m); z < getZIndex(m.z_end_m); z++) R_ver[z * W] = 1e-4;
		}
	};

	void ResistorGrid::solveDirect(bool debug) {
		if(debug == true)
		    cout << "[Solver DIRECT] Формирование разреженной матрицы (размер " << W << "x" << H << ", узлов " << N << ")...\n";
		vector<Eigen::Triplet<double>> triplets;
		triplets.reserve(N * 5);
		Eigen::VectorXd B_vec = Eigen::VectorXd::Zero(N);
		for (int z = 0; z < H; z++) {
			for (int r = 0; r < W; r++) {
				int i = z * W + r;
				if (is_fixed[i] == 1) {
					triplets.push_back(Eigen::Triplet<double>(i, i, 1.0));
					B_vec(i) = fixed_v[i];
				}
				else {
					double s_c = 0.0;
					if (r < W - 1) {
						double c = 1.0 / R_hor[i];
						s_c += c;
						triplets.push_back(Eigen::Triplet<double>(i, i + 1, -c));
					}
					if (r > 0) {
						double c = 1.0 / R_hor[i - 1];
						s_c += c;
						triplets.push_back(Eigen::Triplet<double>(i, i - 1, -c));
					}
					if (z < H - 1) {
						double c = 1.0 / R_ver[i];
						s_c += c;
						triplets.push_back(Eigen::Triplet<double>(i, i + W, -c));
					}
					if (z > 0) {
						double c = 1.0 / R_ver[i - W];
						s_c += c;
						triplets.push_back(Eigen::Triplet<double>(i, i - W, -c));
					}
					triplets.push_back(Eigen::Triplet<double>(i, i, s_c));
				}
			}
		}
		if (debug == true)
		    cout << "[Solver DIRECT] Решение системы линейных уравнений A*X=B с помощью Eigen::SparseLU...\n";
		Eigen::SparseMatrix<double> A(N, N);
		A.setFromTriplets(triplets.begin(), triplets.end());
		Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
		solver.compute(A);
		
		if (solver.info() != Eigen::Success) {
			if (debug == true) 
			    cout << "КРИТИЧЕСКАЯ ОШИБКА: Разложение матрицы не удалось!\n";
			return;
		}
		Eigen::VectorXd X = solver.solve(B_vec);

		if (solver.info() != Eigen::Success) {
			if (debug == true)
			    cout << "КРИТИЧЕСКАЯ ОШИБКА: Решение системы не удалось!\n";
			return;
		}
		for (int i = 0; i < N; i++) {
			v[i] = X(i);
		}
		if (debug == true)
		    cout << "[Done] Система успешно и абсолютно точно решена.\n";
	};
	
	double ResistorGrid::getElectrodeCurrent(const Electrode& el) const {
		double I = 0.0;
		for (int z = getZIndex(el.z_start_m); z <= getZIndex(el.z_end_m); z++) I += (v[z * W] - v[z * W + 1]) / R_hor[z * W];
		return I;
	};

	double ResistorGrid::getMeasElectrodeCurrent(const MeasElectrode& el) const {
		double I = 0.0;
		for (int z = getZIndex(el.z_start_m); z <= (getZIndex(el.z_start_m) + getZIndex(el.z_end_m)) / 2; z++) I += (v[z * W + 1] - v[z * W]) / R_hor[z * W];
		return I;
	};

	double ResistorGrid::getMeasElectrodeVoltage(const MeasElectrode& el) const {
		int start_idx = getZIndex(el.z_start_m);
		int end_idx = getZIndex(el.z_end_m);
		double sum_v = 0.0;
		int count = 0;
		// Проходим по всем узлам электрода (поверхность прибора, r = 0)
		for (int z = start_idx; z <= end_idx; z++) {
			sum_v += v[z * W];
			count++;
		}
		if (count == 0) return 0.0;
		// Возвращаем средний потенциал на электроде в Вольтах
		return sum_v / count;
	}


	double ResistorGrid::getRightBoundaryCurrent() const {
		double I = 0.0;
		for (int z = 0; z < H; z++) I += (v[z * W + (W - 2)] - v[z * W + (W - 1)]) / R_hor[z * W + (W - 2)];
		return I;
	};

	void ResistorGrid::exportMapsWithLegendToBMP() {
		//cout << "[Export] Генерация физических карт с легендой...\n";
		auto drawChar = [](vector<unsigned char>& buf, int img_W, int img_H, int x, int y, char c, int scale = 2) {
			const int font[14][5] = {
				{7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7}, {5,5,7,1,1},
				{7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1}, {7,5,7,5,7}, {7,5,7,1,7},
				{0,0,0,2,0}, {0,0,7,0,0}, {0,2,7,2,0}, {7,4,6,4,7}
			};
			int idx = -1;
			if (c >= '0' && c <= '9') idx = c - '0';
			else if (c == '.') idx = 10; else if (c == '-') idx = 11;
			else if (c == '+') idx = 12; else if (c == 'E' || c == 'e') idx = 13;
			if (idx == -1) return;
			for (int row = 0; row < 5; row++) {
				for (int col = 0; col < 3; col++) {
					if ((font[idx][row] >> (2 - col)) & 1) {
						for (int dy = 0; dy < scale; dy++) {
							for (int dx = 0; dx < scale; dx++) {
								int px = x + col * scale + dx;
								int py = y + (4 - row) * scale + dy;
								if (px >= 0 && px < img_W && py >= 0 && py < img_H) {
									int p_idx = (py * img_W + px) * 3;
									buf[p_idx] = 0; buf[p_idx + 1] = 0; buf[p_idx + 2] = 0;
								}
							}
						}
					}
				}
			}
		};
		auto drawString = [&](vector<unsigned char>& buf, int img_W, int img_H, int x, int y, const string& s, int scale = 2) {
			int curr_x = x;
			for (char c : s) {
				drawChar(buf, img_W, img_H, curr_x, y, c, scale);
				curr_x += 4 * scale;
			}
		};
		auto saveBMP = [&](const string& filename, const vector<double>& values, bool log_scale, const string& title) {
			int plot_W = 400;
			int bar_W = 150;
			int img_W = plot_W + bar_W;
			int img_H = 1000;
			vector<unsigned char> pixels(img_W * img_H * 3, 255);
			double r_min = r_coords.front(), r_max = r_coords.back();
			double z_min = z_coords.front(), z_max = z_coords.back();
			double vmin = 1e9, vmax = -1e9;
			vector<double> plot_vals(img_W * img_H, 0);
			for (int py = 0; py < img_H; py++) {
				double target_z = z_min + py * (z_max - z_min) / (img_H - 1);
				int z_idx = getZIndex(target_z);
				for (int px = 0; px < plot_W; px++) {
					double target_r = r_min + px * (r_max - r_min) / (plot_W - 1);
					int r_idx = getRIndex(target_r);
					double val = values[z_idx * W + r_idx];
					if (log_scale) val = (val > 1e-12) ? log10(val) : -12.0;
					plot_vals[py * img_W + px] = val;
					if (val < vmin) vmin = val;
					if (val > vmax) vmax = val;
				}
			}
			for (int py = 0; py < img_H; py++) {
				for (int px = 0; px < plot_W; px++) {
					unsigned char r, g, b;
					getJetColor(plot_vals[py * img_W + px], vmin, vmax, r, g, b);
					int idx = (py * img_W + px) * 3;
					pixels[idx] = b; pixels[idx + 1] = g; pixels[idx + 2] = r;
				}
			}
			int cb_x = plot_W + 20, cb_w = 30;
			int cb_y_start = 100, cb_y_end = img_H - 100;
			for (int py = cb_y_start; py <= cb_y_end; py++) {
				double frac = (double)(py - cb_y_start) / (cb_y_end - cb_y_start);
				double val = vmin + frac * (vmax - vmin);
				unsigned char r, g, b;
				getJetColor(val, vmin, vmax, r, g, b);

				for (int px = cb_x; px < cb_x + cb_w; px++) {
					int idx = (py * img_W + px) * 3;
					pixels[idx] = b; pixels[idx + 1] = g; pixels[idx + 2] = r;
				}
			}
			int num_ticks = 10;
			for (int i = 0; i <= num_ticks; i++) {
				double frac = (double)i / num_ticks;
				double val = vmin + frac * (vmax - vmin);
				if (log_scale) val = pow(10.0, val);
				int py = cb_y_start + frac * (cb_y_end - cb_y_start);

				char text_buf[32];
				if (log_scale || abs(val) < 0.01) snprintf(text_buf, sizeof(text_buf), "%.1E", val);
				else snprintf(text_buf, sizeof(text_buf), "%.2f", val);
				drawString(pixels, img_W, img_H, cb_x + cb_w + 10, py - 4, text_buf, 2);
			}
			int row_padded = (img_W * 3 + 3) & (~3);
			int file_size = 54 + row_padded * img_H;
			unsigned char header[54] = {
				'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0,
				40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0,
				0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
			};
			header[2] = (unsigned char)(file_size);
			header[3] = (unsigned char)(file_size >> 8);
			header[4] = (unsigned char)(file_size >> 16);
			header[18] = (unsigned char)(img_W); header[19] = (unsigned char)(img_W >> 8);
			header[22] = (unsigned char)(img_H); header[23] = (unsigned char)(img_H >> 8);
			ofstream f(filename, ios::out | ios::binary);
			f.write(reinterpret_cast<char*>(header), 54);
			for (int y = 0; y < img_H; y++) {
				for (int x = 0; x < img_W; x++) {
					int idx = (y * img_W + x) * 3;
					f.write(reinterpret_cast<char*>(&pixels[idx]), 3);
				}
				unsigned char pad[3] = { 0 };
				f.write(reinterpret_cast<char*>(pad), row_padded - img_W * 3);
			}
			f.close();
		};
		saveBMP("potentials_adaptive.bmp", v, false, "Voltage (V)");
		vector<double> currents(N, 0.0);
		for (int z = 0; z < H - 1; z++) {
			for (int r = 0; r < W - 1; r++) {
				double I_hor = (v[z * W + r] - v[z * W + r + 1]) / R_hor[z * W + r];
				double I_ver = (v[z * W + r] - v[(z + 1) * W + r]) / R_ver[z * W + r];
				currents[z * W + r] = sqrt(I_hor * I_hor + I_ver * I_ver);
			}
		}
		saveBMP("currents_adaptive.bmp", currents, true, "Current (A)");
	};


