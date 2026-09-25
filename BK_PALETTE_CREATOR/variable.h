#pragma once
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex> 
#include <string> 
#include <typeinfo.h>
#include <string>
#include <bitset>
#include <vector>
#include "winuser.h"
#include <mmsystem.h>
#include <algorithm>
#pragma comment(lib,"Winmm.lib")
#pragma warning(disable:4244)

using namespace std;


const float PI = 3.141592741f;
const float mA = 0.001f;
const float mV = 0.001f;

const uint8_t Rop_steps = 19;//
const uint8_t Dzp_steps = 14;//количество шагов по диаметру скважины
const uint8_t Rzp_steps = 13;//количество шагов по УЭС зоны проникновения 
const uint8_t Ds_steps = 6;// количество шагов по диаметру скважины

// шаг по диаметру скважины
const string Ds[6] = { "143", "156", "168", "195", "220", "270" };
// шаг по контрасту УЭС пласта к УЭС зоны проникновения
const string Rzp[13] = { "0_01", "0_02", "0_05", "0_1", "0_2", "0_5", "1", "2", "5", "10", "20", "50", "100" };
// шаг по диаметру зоны проникновения
const string Dzp[14] = { "1_1", "1_2", "1_3", "1_4", "1_5", "1_6", "1_8", "2_0", "2_5", "3_0","4_0", "5_0", "7_0", "10_0" };


const float Rop_Ros_ratio_steps[Rop_steps] = { 1.0f, 2.0f, 5.0f, 10.0f, 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f, 50000.0f,100000.0f, 200000.0f, 500000.0f, 1000000.0f };
const float Ds_vals[Ds_steps] = { 143.0f, 156.0f, 5.0f, 168.0f, 220.0f, 270.0f };


enum I_CORR {
	AL_B1, AL_BO, AM_B1, AM_BO, AS_B1, AS_BO
};

enum ELECTROD {
	SML, S, M, L
};

enum STATUS {
	COMPLETED,
	NOT_COMPLETED
};
enum VISUAL {
	CALK_MODE,
	SHOW_MODE,
	INVERSION_MODE

};
//входные параметры среды для прямой задачи по старшинству
//Ds - диаметр скважины
//Rop_Rozp - отношение УЭС пласта к УЭС зоны проникновения
//Dzp_Ds - отношение диаметра зоны проникновения к диаметру скважины
//Rop_Ros - отношение УЭС пласта к УЭС скважинного флюида
enum RATIO {
	Ds_, Rop_Rozp, Dzp_Ds, Rop_Ros
};

/*
enum Ds_step {
	Ds, Rop_Rozp, Dzp_Ds, Rop_Ros
};

enum Rop_Ros_step {

};
*/

struct LISTKOV_TOOL {

};


//
#pragma pack(push, 1)

struct SondeParams {
	double dr_start ;
	double q_r ;
	double r_max ;
	double dz_tool;
	double dz_fine;
	double threshold;
	double q_z_tail;

	double r_tool;
	double tail, voltage;
	double z_tool_start;
	// токовые электроды (Z и L)
	double short_z, short_l;
	double mid_z, mid_l;
	double long_z, long_l;
	// grnd электроды B
	double b0_z, b0_l;
	double b1_z, b1_l;
	double b2_z, b2_l;
	double b2_add_z, b2_add_l;
	// Измерительные электроды M/N
	double um_z, um_l;
	double un_z, un_l;
	double z_tool_end;
};

struct Params {
	double dr_start;
	double q_r;
	double r_max;
	double dz_tool;
	double dz_fine;
	double threshold;
	double q_z_tail;
	double r_tool;
	double tail;
	double voltage;
	double z_tool_start;
};

struct Electrode {
	double z_start_m, z_end_m;
	double voltage;
	string name;
};

struct MeasElectrode {
	double z_start_m, z_end_m;
	string name;
};

enum ELECTRODE_MODE {
	CURR, GRND, MEAS
 };

struct ELECTRODE {
	double z_start_mm, z_end_mm;
	int mode;
	string name;
	string work_mode;
};

struct LayerDakhnov {
	double r_boundary;
	double rho;
};


struct BK_SONDE {
	float Ib;//ток на измерительном электроде b0
	float Ia;//ток на токовом электроде A1 A2 A3
	float dU;// разница потенциалов на M N электродах
	float AT_I0;//
	float PH_I0;//
};

struct BK_ALL_DATA {// 
	int32_t frame;
	float acsel_X;
	float acsel_Y;
	float acsel_Z;
	BK_SONDE bk_sonde[4];
	float append_PSw;
	float append_PSn;
	float append_Ureal;
	float append_Ilimit;
};

//структура данных в паллетке
struct BK {
	double Ia[4];
	double Ib[4];
	double dU[4];
	float R_BH;
	float R_PZ_R_BH;
	float Ro_PZ_Ro_BH;
	float Ro_stratum_Ro_BH;
};


#pragma pack(push, 1)
struct BK_ {
	// Фиксированные поля (17 байт)
	float R_BH;
	float R_PZ_R_BH;
	float Ro_PZ_Ro_BH;
	float Ro_stratum_Ro_BH;
	uint8_t work_modes;
	// Указатели для работы в памяти
	double *Ia = nullptr;
	double *Ib = nullptr;
	double *dU = nullptr;

	// Конструктор: настраиваем структуру под выбор пользователя
	BK_(uint8_t n) : work_modes(n) {
		Ia = new double[n](); // () обнуляет массив
		Ib = new double[n]();
		dU = new double[n]();
	}

	// Обязательно освобождаем память
	~BK_() {
		delete[] Ia;
		delete[] Ib;
		delete[] dU;
	}

	// Запрещаем копирование, чтобы не удалить память дважды
	BK_(const BK_&) = delete;
	BK_& operator=(const BK_&) = delete;
};
#pragma pack(pop)

struct BK_PALLETE_NODES {
	float R_BH[100] = { 0.0, };
	float R_PZ_R_BH[100] = { 0.0, };
	float Ro_PZ_Ro_BH[100] = { 0.0, };
	float Ro_stratum_Ro_BH[100] = { 0.0, };
	uint32_t points[4] = { 0, };

};

struct BK_METROLOGY {
	uint32_t add [128];
};

struct BK_PALLETE_FILE_HEADER {
	uint32_t status;
	SondeParams params;
	BK_PALLETE_NODES BK_pallete_nodes;
};

struct Vector_BK {
	vector<double> R_BH_nodes;
	vector<double> R_PZ_nodes;
	vector<double> Ro_PZ_nodes;
	vector<double> Ro_stratum_nodes;
	
	// Сам контейнер
	vector<vector<vector<vector<BK>>>> data;
	// Переменные для размеров
	size_t n_R_BH, n_R_PZ, n_Ro_PZ, n_Ro_stratum;
	// Метод для инициализации
	void init(size_t n1, size_t n2, size_t n3, size_t n4) {
		n_R_BH = n1; n_R_PZ = n2; n_Ro_PZ = n3; n_Ro_stratum = n4;
		data.assign(n_R_BH, vector(n_R_PZ, vector(n_Ro_PZ, vector<BK>(n_Ro_stratum))));
	}
	// Удобный метод для получения элемента (с проверкой границ)
	BK& at(size_t i, size_t j, size_t k, size_t l) {
		return data.at(i).at(j).at(k).at(l);
	}
	void clear() {
		data.clear();
		data.shrink_to_fit();
		R_BH_nodes.clear();
		R_PZ_nodes.clear();
		Ro_PZ_nodes.clear();
		Ro_stratum_nodes.clear();
		n_R_BH = n_R_PZ = n_Ro_PZ = n_Ro_stratum = 0;
	}

};

struct BK_Weights {
	double W_a = 1.0;
	double W_b = 1.0;
	double W_u = 1.0;
};

// Структура для хранения локальных весов каждого узла (по 4 канала)
struct BK_LocalWeights {
	double W_a[4];
	double W_b[4];
	double W_u[4];
};

// Структура для задания аппаратных погрешностей
struct HardwareError {
	double rel_err; // Относительная погрешность (например, 0.05 для 5%)
	double abs_err; // Абсолютная погрешность (уровень фонового шума / порог чувствительности)
};
// Настройки погрешности для каждого типа данных
struct BK_Errors {
	HardwareError Ia = { 0.05, 1e-6 }; // Пример: 5% относительная и 1 мкА абсолютная
	HardwareError Ib = { 0.05, 1e-8 }; // Пример: 5% относительная и 10 нА абсолютная
	HardwareError dU = { 0.02, 1e-5 }; // Пример: 2% относительная и 10 мкВ абсолютная
};

// Единая структура для хранения и грубого узла, и уточненного подузлового решения
struct SearchResult {
	// Индексы и данные наилучшего узла (Brute Force)
	size_t m, i, j, k, l;
	double min_error;
	BK best_match;
	// Уточненные (дробные) индексы
	double m_float, i_float, j_float, k_float, l_float;

	// Интерполированные физические параметры
	double Ro_BH;
	double R_BH;
	double R_PZ;
	double Ro_PZ;
	double Ro_stratum;
};

// Перечисления для настроек поиска
enum class MisfitType { Simple, Logarithmic };
enum class WeightType { Global, Local };

struct Limit {
	double min_val = -numeric_limits<double>::infinity();
	double max_val = numeric_limits<double>::infinity();
};

struct SearchConstraints {
	Limit Ro_BH;
	Limit R_BH;
	Limit R_PZ;
	Limit Ro_PZ;
	Limit Ro_stratum;
};


struct Vector_BK_ {
	BK_Errors errors;
	BK_Weights weights;

	vector<double> Ro_BH_nodes;
	vector<double> R_BH_nodes;
	vector<double> R_PZ_nodes;
	vector<double> Ro_PZ_nodes;
	vector<double> Ro_stratum_nodes;

	vector<BK> data_4d;
	vector<BK> data_5d;

	vector<BK_LocalWeights> weights_5d; // Вектор рассчитанных локальных весов

	size_t n_Ro_BH, n_R_BH = 0, n_R_PZ = 0, n_Ro_PZ = 0, n_Ro_stratum = 0;

	void init_4d(size_t n1, size_t n2, size_t n3, size_t n4) {
		n_R_BH = n1;
		n_R_PZ = n2;
		n_Ro_PZ = n3;
		n_Ro_stratum = n4;
		data_4d.assign(n_R_BH * n_R_PZ * n_Ro_PZ * n_Ro_stratum, BK{});
	}

	void generate_5d(const vector<double>& Ro_BH) {
		Ro_BH_nodes.clear();
		Ro_BH_nodes = Ro_BH;
		n_Ro_BH = Ro_BH.size(); // Количество слоев равно длине вектора
		size_t slice_size = data_4d.size();
		data_5d.resize(n_Ro_BH * slice_size);
		for (size_t m = 0; m < n_Ro_BH; ++m) {
			double ro_multiplier = Ro_BH[m]; // Множитель для текущего слоя
			for (size_t e = 0; e < slice_size; ++e) {
				BK item = data_4d[e]; // Копируем 4D элемент
				// Модифицируем Ia и Ib
				for (int idx = 0; idx < 4; ++idx) {
					item.Ia[idx] /= ro_multiplier;
					item.Ib[idx] /= ro_multiplier;
				}
				data_5d[m * slice_size + e] = item; // Записываем в 5D
			}
		}
	}

	BK& at(size_t i, size_t j, size_t k, size_t l) {
		size_t index = i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) + j * (n_Ro_PZ * n_Ro_stratum) + k * (n_Ro_stratum)+l;
		return data_4d.at(index);
	}

	const BK& at(size_t i, size_t j, size_t k, size_t l) const {
		size_t index = i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) + j * (n_Ro_PZ * n_Ro_stratum) + k * (n_Ro_stratum)+l;
		return data_4d.at(index);
	}

	BK& at(size_t m, size_t i, size_t j, size_t k, size_t l) {
		size_t index = m * (n_R_BH * n_R_PZ * n_Ro_PZ * n_Ro_stratum) + i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) + j * (n_Ro_PZ * n_Ro_stratum) + k * (n_Ro_stratum)+l;
		return data_5d.at(index);
	}

	const BK& at(size_t m, size_t i, size_t j, size_t k, size_t l) const {
		size_t index = m * (n_R_BH * n_R_PZ * n_Ro_PZ * n_Ro_stratum) + i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) + j * (n_Ro_PZ * n_Ro_stratum) + k * (n_Ro_stratum)+l;
		return data_5d.at(index);
	}

	void clear() {
		data_4d.clear(); data_4d.shrink_to_fit();
		data_5d.clear(); data_5d.shrink_to_fit();
		R_BH_nodes.clear(); R_PZ_nodes.clear();
		Ro_PZ_nodes.clear(); Ro_stratum_nodes.clear();
		n_Ro_BH = n_R_BH = n_R_PZ = n_Ro_PZ = n_Ro_stratum = 0;
	}

	// Вспомогательный метод для получения элемента с защитой от выхода за границы
	const BK& get_safe(int m, int i, int j, int k, int l) const {
		m = clamp(m, 0, (int)n_Ro_BH - 1);
		i = clamp(i, 0, (int)n_R_BH - 1);
		j = clamp(j, 0, (int)n_R_PZ - 1);
		k = clamp(k, 0, (int)n_Ro_PZ - 1);
		l = clamp(l, 0, (int)n_Ro_stratum - 1);
		size_t index = m * (n_R_BH * n_R_PZ * n_Ro_PZ * n_Ro_stratum) +
			i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) +
			j * (n_Ro_PZ * n_Ro_stratum) +
			k * (n_Ro_stratum)+l;
		return data_5d[index];
	}
	// Метод для предварительного расчета локальных весов
	void calculate_local_weights() {
		size_t total_size = data_5d.size();
		weights_5d.resize(total_size);
		const double eps = 1e-12;
		for (int m = 0; m < n_Ro_BH; ++m) {
			for (int i = 0; i < n_R_BH; ++i) {
				for (int j = 0; j < n_R_PZ; ++j) {
					for (int k = 0; k < n_Ro_PZ; ++k) {
						for (int l = 0; l < n_Ro_stratum; ++l) {

							const BK& center = get_safe(m, i, j, k, l);
							BK_LocalWeights lw;
							for (int idx = 0; idx < 4; ++idx) {
								double diff_Ia = 0, diff_Ib = 0, diff_dU = 0;
								int num_n = 0;
								int offsets[5][2] = { {m - 1,m + 1}, {i - 1,i + 1}, {j - 1,j + 1}, {k - 1,k + 1}, {l - 1,l + 1} };

								for (int d = 0; d < 5; ++d) {
									for (int step = 0; step < 2; ++step) {
										int nm = (d == 0) ? offsets[0][step] : m;
										int ni = (d == 1) ? offsets[1][step] : i;
										int nj = (d == 2) ? offsets[2][step] : j;
										int nk = (d == 3) ? offsets[3][step] : k;
										int nl = (d == 4) ? offsets[4][step] : l;
										if (nm < 0 || nm >= n_Ro_BH || ni < 0 || ni >= n_R_BH || nj < 0 || nj >= n_R_PZ || nk < 0 || nk >= n_Ro_PZ || nl < 0 || nl >= n_Ro_stratum) continue;
										const BK& neighbor = get_safe(nm, ni, nj, nk, nl);
										diff_Ia += fabs(neighbor.Ia[idx] - center.Ia[idx]);
										diff_Ib += fabs(neighbor.Ib[idx] - center.Ib[idx]);
										diff_dU += fabs(neighbor.dU[idx] - center.dU[idx]);
										num_n++;
									}
								}
								double c_Ia = (diff_Ia / num_n) / (fabs(center.Ia[idx]) + eps);
								double c_Ib = (diff_Ib / num_n) / (fabs(center.Ib[idx]) + eps);
								double c_dU = (diff_dU / num_n) / (fabs(center.dU[idx]) + eps);
								double total_c = c_Ia + c_Ib + c_dU + eps;
								lw.W_a[idx] = c_Ia / total_c;
								lw.W_b[idx] = c_Ib / total_c;
								lw.W_u[idx] = c_dU / total_c;
							}
							size_t index = m * (n_R_BH * n_R_PZ * n_Ro_PZ * n_Ro_stratum) +
								i * (n_R_PZ * n_Ro_PZ * n_Ro_stratum) +
								j * (n_Ro_PZ * n_Ro_stratum) + k * (n_Ro_stratum)+l;
							weights_5d[index] = lw;
						}
					}
				}
			}
		}
	}


};

#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<double> buildSmartDepthGrid_new(
	const vector<Electrode>& a_els,
	const vector<Electrode>& b_els,
	const vector<MeasElectrode>& m_els,
	double dz_base, double dz_fine, double threshold_len,
	double q_tail, double tail_length);

vector<double> buildSmartDepthGrid(
	const vector<Electrode>& a_els,
	const vector<Electrode>& b_els,
	const vector<MeasElectrode>& m_els,
	double dz_fine, double q_z, double tail_length);

vector<double> buildRadialGridAdaptive(double r_tool, vector<double> boundaries, double dr_start, double q_r);

vector<double> buildRadialGrid(double r_tool, double r_max, double dr_start, double q_r);

vector<double> buildDepthGrid(double z_tool_start, double z_tool_end, double dz_tool, double q_z, double tail_length);

void getJetColor(double v, double vmin, double vmax, unsigned char& r, unsigned char& g, unsigned char& b);
