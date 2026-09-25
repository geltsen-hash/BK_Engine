#include "bk_solver.h"
#include "json_parser.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace BK {

static const double PI = 3.14159265358979323846;

BKSolver::BKSolver() {
    // Дефолтные параметры сетки
    grid_params_.auto_grid = 1;
    grid_params_.tool_radius = 0.045;
    grid_params_.dr_start = 0.005;
    grid_params_.q_r = 1.15;
    grid_params_.r_max = 30.0;
    grid_params_.dz_tool = 0.05;
    grid_params_.dz_fine = 0.005;
}

BK_Status BKSolver::initialize(
    const BK_ElectrodeDef* electrodes,
    int num_electrodes,
    const BK_WorkModeDef* modes,
    int num_modes,
    const BK_GridParams* grid_params
) {
    if (!electrodes || num_electrodes <= 0 || !modes || num_modes <= 0) {
        setLastError("Invalid electrode or mode parameters provided.");
        return BK_ERR_INVALID_PARAMS;
    }

    if (grid_params) {
        grid_params_ = *grid_params;
        if (grid_params_.tool_radius > 0.0) tool_radius_ = grid_params_.tool_radius;
        if (grid_params_.dr_start <= 0.0) grid_params_.dr_start = 0.005;
        if (grid_params_.q_r <= 1.0) grid_params_.q_r = 1.15;
        if (grid_params_.r_max <= tool_radius_) grid_params_.r_max = 30.0;
        if (grid_params_.dz_tool <= 0.0) grid_params_.dz_tool = 0.05;
        if (grid_params_.dz_fine <= 0.0) grid_params_.dz_fine = 0.005;
    }

    electrodes_.clear();
    for (int i = 0; i < num_electrodes; ++i) {
        ElectrodeInternal el;
        el.name = electrodes[i].name ? electrodes[i].name : ("El_" + std::to_string(i));
        el.z_start = electrodes[i].z_start;
        el.length = std::max(1e-4, electrodes[i].length);
        el.z_end = el.z_start + el.length;
        electrodes_.push_back(el);
    }

    modes_.clear();
    for (int m = 0; m < num_modes; ++m) {
        ModeInternal mode;
        mode.name = modes[m].mode_name ? modes[m].mode_name : ("Mode_" + std::to_string(m));
        if (modes[m].states) {
            for (int e = 0; e < num_electrodes; ++e) {
                mode.electrode_states.push_back(modes[m].states[e]);
            }
        } else {
            setLastError("Mode states array is null for mode index " + std::to_string(m));
            return BK_ERR_INVALID_PARAMS;
        }
        modes_.push_back(mode);
    }

    // Инициализируем геометрию по умолчанию
    geometry_initialized_ = false;
    return setMediumGeometry(current_r_bh_, current_r_pz_);
}

BK_Status BKSolver::loadFromConfigFile(
    const char* config_filepath,
    const BK_GridParams* grid_params
) {
    if (!config_filepath) {
        setLastError("Config filepath is null.");
        return BK_ERR_INVALID_PARAMS;
    }

    std::ifstream file(config_filepath);
    if (!file.is_open()) {
        setLastError("Could not open config file: " + std::string(config_filepath));
        return BK_ERR_CONFIG_FILE_NOT_FOUND;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    SimpleJson::Parser parser(content);
    SimpleJson::JsonValue root = parser.parse();

    if (!root.is_object()) {
        setLastError("Failed to parse JSON config file: " + std::string(config_filepath));
        return BK_ERR_CONFIG_PARSE_FAILED;
    }

    probe_name_ = root["probe_name"].as_string("BK_Probe");
    if (root.has_key("tool_radius_m")) {
        tool_radius_ = root["tool_radius_m"].as_double(0.045);
    }

    // Чтение электродов
    const auto& el_arr = root["electrodes"];
    if (!el_arr.is_array() || el_arr.arr_val.empty()) {
        setLastError("No 'electrodes' array found in JSON config.");
        return BK_ERR_CONFIG_PARSE_FAILED;
    }

    std::vector<BK_ElectrodeDef> el_defs;
    std::vector<std::string> el_names;
    el_names.reserve(el_arr.arr_val.size());

    for (size_t i = 0; i < el_arr.arr_val.size(); ++i) {
        const auto& item = el_arr[i];
        std::string name = item["name"].as_string("El_" + std::to_string(i));
        el_names.push_back(name);
        double z_start = item["z_start"].as_double(0.0);
        double length = 0.0;
        if (item.has_key("length")) {
            length = item["length"].as_double(0.05);
        } else if (item.has_key("z_end")) {
            length = item["z_end"].as_double(z_start + 0.05) - z_start;
        }
        length = std::max(1e-4, length);

        BK_ElectrodeDef def;
        def.z_start = z_start;
        def.length = length;
        def.name = nullptr; // установим ниже
        el_defs.push_back(def);
    }
    for (size_t i = 0; i < el_defs.size(); ++i) {
        el_defs[i].name = el_names[i].c_str();
    }

    // Чтение режимов
    const auto& modes_arr = root["modes"];
    if (!modes_arr.is_array() || modes_arr.arr_val.empty()) {
        setLastError("No 'modes' array found in JSON config.");
        return BK_ERR_CONFIG_PARSE_FAILED;
    }

    std::vector<BK_WorkModeDef> mode_defs;
    std::vector<std::string> mode_names;
    std::vector<std::vector<BK_ElectrodeModeState>> all_mode_states;

    for (size_t m = 0; m < modes_arr.arr_val.size(); ++m) {
        const auto& m_item = modes_arr[m];
        std::string m_name = m_item["mode_name"].as_string("Mode_" + std::to_string(m));
        mode_names.push_back(m_name);

        std::vector<BK_ElectrodeModeState> states(el_defs.size());
        for (size_t e = 0; e < el_defs.size(); ++e) {
            states[e].state = BK_ELECTRODE_FLOATING;
            states[e].fixed_voltage = 0.0;
        }

        const auto& states_arr = m_item["states"];
        if (states_arr.is_array()) {
            for (size_t s = 0; s < states_arr.arr_val.size(); ++s) {
                const auto& s_item = states_arr[s];
                std::string target_el = s_item["electrode"].as_string("");
                auto it = std::find(el_names.begin(), el_names.end(), target_el);
                if (it != el_names.end()) {
                    int el_idx = static_cast<int>(std::distance(el_names.begin(), it));
                    std::string st_str = s_item["state"].as_string("FLOATING");
                    if (st_str == "CONNECTED" || st_str == "FIXED_VOLTAGE") {
                        states[el_idx].state = BK_ELECTRODE_CONNECTED;
                        states[el_idx].fixed_voltage = s_item["voltage"].as_double(0.0);
                    } else {
                        states[el_idx].state = BK_ELECTRODE_FLOATING;
                        states[el_idx].fixed_voltage = 0.0;
                    }
                }
            }
        }
        all_mode_states.push_back(states);
    }

    mode_defs.resize(modes_arr.arr_val.size());
    for (size_t m = 0; m < mode_defs.size(); ++m) {
        mode_defs[m].mode_name = mode_names[m].c_str();
        mode_defs[m].states = all_mode_states[m].data();
    }

    return initialize(el_defs.data(), static_cast<int>(el_defs.size()),
                      mode_defs.data(), static_cast<int>(mode_defs.size()), grid_params);
}

int BKSolver::getZIndex(double target_z) const {
    auto it = std::lower_bound(z_coords_.begin(), z_coords_.end(), target_z);
    if (it == z_coords_.end()) return H_ - 1;
    if (it == z_coords_.begin()) return 0;
    if (*it - target_z < target_z - *(it - 1)) return static_cast<int>(std::distance(z_coords_.begin(), it));
    return static_cast<int>(std::distance(z_coords_.begin(), it - 1));
}

int BKSolver::getRIndex(double target_r) const {
    auto it = std::lower_bound(r_coords_.begin(), r_coords_.end(), target_r);
    if (it == r_coords_.end()) return W_ - 1;
    if (it == r_coords_.begin()) return 0;
    if (*it - target_r < target_r - *(it - 1)) return static_cast<int>(std::distance(r_coords_.begin(), it));
    return static_cast<int>(std::distance(r_coords_.begin(), it - 1));
}

void BKSolver::buildGrids() {
    // 1. Построение радиальной сетки r_coords_
    // Точки привязки: r_tool, r_bh, r_pz, r_max
    r_coords_.clear();
    double r_curr = tool_radius_;
    r_coords_.push_back(r_curr);

    // Зона 1: от r_tool до r_bh
    double dr = grid_params_.dr_start;
    while (r_curr + dr < current_r_bh_ - 1e-4) {
        r_curr += dr;
        r_coords_.push_back(r_curr);
        dr *= 1.05;
    }
    r_curr = current_r_bh_;
    r_coords_.push_back(r_curr);

    // Зона 2: от r_bh до r_pz
    dr = std::max(grid_params_.dr_start, (current_r_pz_ - current_r_bh_) / 10.0);
    while (r_curr + dr < current_r_pz_ - 1e-4) {
        r_curr += dr;
        r_coords_.push_back(r_curr);
        dr *= 1.08;
    }
    r_curr = current_r_pz_;
    r_coords_.push_back(r_curr);

    // Зона 3: от r_pz до r_max
    dr = std::max(0.01, (r_coords_.back() - r_coords_[r_coords_.size() - 2]) * grid_params_.q_r);
    while (r_curr + dr < grid_params_.r_max - 1e-3) {
        r_curr += dr;
        r_coords_.push_back(r_curr);
        dr *= grid_params_.q_r;
    }
    r_coords_.push_back(grid_params_.r_max);

    // Удаление возможных дубликатов
    std::sort(r_coords_.begin(), r_coords_.end());
    r_coords_.erase(std::unique(r_coords_.begin(), r_coords_.end(), [](double a, double b) {
        return std::abs(a - b) < 1e-5;
    }), r_coords_.end());

    W_ = static_cast<int>(r_coords_.size());

    // 2. Построение вертикальной сетки z_coords_
    // Определение диапазона зонда
    double z_min_tool = 1e9, z_max_tool = -1e9;
    for (const auto& el : electrodes_) {
        z_min_tool = std::min(z_min_tool, el.z_start);
        z_max_tool = std::max(z_max_tool, el.z_end);
    }
    if (z_min_tool > z_max_tool) {
        z_min_tool = 0.0;
        z_max_tool = 2.0;
    }

    double z_tool_start = z_min_tool - 0.2;
    double z_tool_end = z_max_tool + 0.2;

    // Зоны мелкой сетки вокруг электродов
    struct Interval { double start; double end; };
    std::vector<Interval> fine_zones;
    for (const auto& el : electrodes_) {
        fine_zones.push_back({ el.z_start - 0.02, el.z_end + 0.02 });
    }

    std::vector<double> tool_z;
    double z_curr = z_tool_start;
    tool_z.push_back(z_curr);

    while (z_curr < z_tool_end) {
        bool in_fine = false;
        for (const auto& fz : fine_zones) {
            if (z_curr >= fz.start && z_curr <= fz.end) {
                in_fine = true;
                break;
            }
        }
        double step = in_fine ? grid_params_.dz_fine : grid_params_.dz_tool;
        z_curr += step;
        tool_z.push_back(z_curr);
    }

    // Принудительно сажаем точные границы электродов в сетку
    for (const auto& el : electrodes_) {
        tool_z.push_back(el.z_start);
        tool_z.push_back(el.z_end);
    }
    std::sort(tool_z.begin(), tool_z.end());
    tool_z.erase(std::unique(tool_z.begin(), tool_z.end(), [](double a, double b) {
        return std::abs(a - b) < 1e-4;
    }), tool_z.end());

    // Хвосты сверху и снизу
    double tail_length = 30.0;
    double tail_step = grid_params_.dz_tool;
    std::vector<double> tail_deltas;
    double acc = 0.0;
    while (acc < tail_length) {
        tail_deltas.push_back(tail_step);
        acc += tail_step;
        tail_step *= 1.15;
    }

    z_coords_.clear();
    // Верхний хвост (от z_min - 30 до z_tool_start)
    double top_z = tool_z.front();
    for (int i = static_cast<int>(tail_deltas.size()) - 1; i >= 0; --i) {
        top_z -= tail_deltas[i];
        z_coords_.push_back(top_z);
    }
    // Тело зонда
    for (double z : tool_z) {
        z_coords_.push_back(z);
    }
    // Нижний хвост
    double bot_z = tool_z.back();
    for (size_t i = 0; i < tail_deltas.size(); ++i) {
        bot_z += tail_deltas[i];
        z_coords_.push_back(bot_z);
    }

    std::sort(z_coords_.begin(), z_coords_.end());
    z_coords_.erase(std::unique(z_coords_.begin(), z_coords_.end(), [](double a, double b) {
        return std::abs(a - b) < 1e-4;
    }), z_coords_.end());

    H_ = static_cast<int>(z_coords_.size());
    N_ = W_ * H_;

    // Привязка индексов электродов
    for (auto& el : electrodes_) {
        el.z_idx_start = getZIndex(el.z_start);
        el.z_idx_end = getZIndex(el.z_end);
        if (el.z_idx_end < el.z_idx_start) std::swap(el.z_idx_start, el.z_idx_end);
    }
}

void BKSolver::buildConductanceGeometry() {
    geom_G_hor_.assign(N_, 0.0);
    geom_G_ver_.assign(N_, 0.0);
    cell_zone_id_.assign(N_, 0);

    for (int z = 0; z < H_; ++z) {
        double h_eff;
        if (z == 0) h_eff = (z_coords_[1] - z_coords_[0]) / 2.0;
        else if (z == H_ - 1) h_eff = (z_coords_[H_ - 1] - z_coords_[H_ - 2]) / 2.0;
        else h_eff = (z_coords_[z + 1] - z_coords_[z - 1]) / 2.0;

        for (int r = 0; r < W_; ++r) {
            int idx = z * W_ + r;
            double curr_r = r_coords_[r];

            // Зона среды
            if (curr_r <= current_r_bh_ + 1e-5) cell_zone_id_[idx] = 0; // Borehole
            else if (curr_r <= current_r_pz_ + 1e-5) cell_zone_id_[idx] = 1; // Invaded zone
            else cell_zone_id_[idx] = 2; // Virgin formation

            // Горизонтальный геометрический фактор
            if (r < W_ - 1) {
                double next_r = r_coords_[r + 1];
                geom_G_hor_[idx] = (2.0 * PI * h_eff) / std::log(next_r / curr_r);
            }

            // Вертикальный геометрический фактор
            if (z < H_ - 1) {
                if (r == 0) {
                    geom_G_ver_[idx] = 0.0; // Изолированный корпус прибора
                } else {
                    double dz = z_coords_[z + 1] - z_coords_[z];
                    double r_inner = (r_coords_[r] + r_coords_[r - 1]) / 2.0;
                    double r_outer = (r == W_ - 1) ? curr_r + (curr_r - r_coords_[r - 1]) / 2.0 : (r_coords_[r + 1] + curr_r) / 2.0;
                    geom_G_ver_[idx] = (PI * (r_outer * r_outer - r_inner * r_inner)) / dz;
                }
            }
        }
    }
}

BK_Status BKSolver::setMediumGeometry(double r_bh, double r_pz) {
    if (r_bh <= tool_radius_ || r_pz <= r_bh) {
        setLastError("Invalid geometry: must have tool_radius < r_bh < r_pz.");
        return BK_ERR_INVALID_PARAMS;
    }

    current_r_bh_ = r_bh;
    current_r_pz_ = r_pz;

    buildGrids();
    buildConductanceGeometry();

    G_hor_.assign(N_, 0.0);
    G_ver_.assign(N_, 0.0);
    mode_potentials_cache_.assign(modes_.size(), std::vector<double>(N_, 0.0));

    geometry_initialized_ = true;
    return BK_STATUS_OK;
}

void BKSolver::updateResistivityConductances(double rho_bh, double rho_pz, double rho_stratum) {
    double rho_table[3] = {
        std::max(1e-6, rho_bh),
        std::max(1e-6, rho_pz),
        std::max(1e-6, rho_stratum)
    };

    for (int i = 0; i < N_; ++i) {
        int z_id = cell_zone_id_[i];
        double rho = rho_table[z_id];
        G_hor_[i] = geom_G_hor_[i] / rho;
        G_ver_[i] = geom_G_ver_[i] / rho;
    }
}

BK_Status BKSolver::solveSingleMode(int mode_idx, std::vector<double>& potentials) {
    potentials.assign(N_, 0.0);
    const auto& mode = modes_[mode_idx];

    // Определение степеней свободы (DOF mapping)
    // -1: фиксированный потенциал (Dirichlet)
    // >= 0: свободная степень свободы или макроузел плавающего электрода
    std::vector<int> dof(N_, -2);
    std::vector<double> fixed_v(N_, 0.0);
    std::vector<uint8_t> is_fixed(N_, 0);

    // 1. Граничное условие на внешнем радиусе r = W - 1 (Земля V=0)
    for (int z = 0; z < H_; ++z) {
        int idx = z * W_ + (W_ - 1);
        is_fixed[idx] = 1;
        fixed_v[idx] = 0.0;
        dof[idx] = -1;
    }

    // 2. Электроды в данном режиме
    std::vector<int> floating_electrode_dof(electrodes_.size(), -1);
    int num_dofs = 0;

    for (size_t e = 0; e < electrodes_.size(); ++e) {
        const auto& el = electrodes_[e];
        const auto& st = mode.electrode_states[e];

        if (st.state == BK_ELECTRODE_CONNECTED) {
            for (int z = el.z_idx_start; z <= el.z_idx_end; ++z) {
                int idx = z * W_ + 0;
                is_fixed[idx] = 1;
                fixed_v[idx] = st.fixed_voltage;
                dof[idx] = -1;
            }
        } else { // FLOATING (эквипотенциальный макроузел)
            int macro_dof = num_dofs++;
            floating_electrode_dof[e] = macro_dof;
            for (int z = el.z_idx_start; z <= el.z_idx_end; ++z) {
                int idx = z * W_ + 0;
                dof[idx] = macro_dof;
            }
        }
    }

    // 3. Назначение DOF для всех остальных свободных узлов
    for (int i = 0; i < N_; ++i) {
        if (dof[i] == -2) {
            dof[i] = num_dofs++;
        }
    }

    if (num_dofs == 0) {
        for (int i = 0; i < N_; ++i) potentials[i] = fixed_v[i];
        return BK_STATUS_OK;
    }

    // 4. Сборка симметричной матрицы проводимостей A и вектора правой части B
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(num_dofs * 5);
    Eigen::VectorXd B_vec = Eigen::VectorXd::Zero(num_dofs);

    auto add_resistor = [&](int u, int v, double g) {
        if (g <= 0.0) return;
        int du = dof[u];
        int dv = dof[v];

        if (du >= 0 && dv >= 0) {
            triplets.push_back(Eigen::Triplet<double>(du, du, g));
            triplets.push_back(Eigen::Triplet<double>(dv, dv, g));
            if (du != dv) {
                triplets.push_back(Eigen::Triplet<double>(du, dv, -g));
                triplets.push_back(Eigen::Triplet<double>(dv, du, -g));
            }
        } else if (du >= 0 && dv < 0) {
            triplets.push_back(Eigen::Triplet<double>(du, du, g));
            B_vec(du) += g * fixed_v[v];
        } else if (dv >= 0 && du < 0) {
            triplets.push_back(Eigen::Triplet<double>(dv, dv, g));
            B_vec(dv) += g * fixed_v[u];
        }
    };

    for (int z = 0; z < H_; ++z) {
        for (int r = 0; r < W_; ++r) {
            int idx = z * W_ + r;
            // Горизонтальный резистор к (r + 1, z)
            if (r < W_ - 1) {
                add_resistor(idx, idx + 1, G_hor_[idx]);
            }
            // Вертикальный резистор к (r, z + 1)
            if (z < H_ - 1) {
                add_resistor(idx, idx + W_, G_ver_[idx]);
            }
        }
    }

    Eigen::SparseMatrix<double> A(num_dofs, num_dofs);
    A.setFromTriplets(triplets.begin(), triplets.end());

    // 5. Симметричный положительно-определенный решатель SimplicialLDLT
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(A);

    if (solver.info() != Eigen::Success) {
        setLastError("SimplicialLDLT factorization failed for mode " + mode.name);
        return BK_ERR_SINGULAR_MATRIX;
    }

    Eigen::VectorXd X = solver.solve(B_vec);
    if (solver.info() != Eigen::Success) {
        setLastError("SimplicialLDLT solve failed for mode " + mode.name);
        return BK_ERR_SOLVER_FAILED;
    }

    // 6. Распаковка решения в полный вектор потенциалов сетки
    for (int i = 0; i < N_; ++i) {
        if (dof[i] >= 0) {
            potentials[i] = X(dof[i]);
        } else {
            potentials[i] = fixed_v[i];
        }
    }

    return BK_STATUS_OK;
}

BK_Status BKSolver::solveResistivity(
    double rho_bh,
    double rho_pz,
    double rho_stratum,
    double* out_values,
    int out_values_count
) {
    if (!geometry_initialized_) {
        setLastError("Solver geometry is not initialized. Call setMediumGeometry first.");
        return BK_ERR_INVALID_PARAMS;
    }

    int total_expected = static_cast<int>(modes_.size() * electrodes_.size());
    if (!out_values || out_values_count < total_expected) {
        setLastError("Output buffer is null or too small. Expected at least " + std::to_string(total_expected));
        return BK_ERR_BUFFER_TOO_SMALL;
    }

    updateResistivityConductances(rho_bh, rho_pz, rho_stratum);

    int out_idx = 0;
    for (size_t m = 0; m < modes_.size(); ++m) {
        BK_Status st = solveSingleMode(static_cast<int>(m), mode_potentials_cache_[m]);
        if (st != BK_STATUS_OK) return st;

        const auto& V = mode_potentials_cache_[m];
        const auto& mode = modes_[m];

        for (size_t e = 0; e < electrodes_.size(); ++e) {
            const auto& el = electrodes_[e];
            const auto& state = mode.electrode_states[e];

            if (state.state == BK_ELECTRODE_CONNECTED) {
                // Ток через всю длину электрода в породу
                double current = 0.0;
                for (int z = el.z_idx_start; z <= el.z_idx_end; ++z) {
                    int idx = z * W_ + 0;
                    current += (V[idx] - V[idx + 1]) * G_hor_[idx];
                }
                out_values[out_idx++] = current;
            } else {
                // Потенциал на эквипотенциальном плавающем электроде
                int idx = el.z_idx_start * W_ + 0;
                out_values[out_idx++] = V[idx];
            }
        }
    }

    return BK_STATUS_OK;
}

BK_Status BKSolver::solveResistivityBatch(
    const double* rho_bh_arr,
    const double* rho_pz_arr,
    const double* rho_stratum_arr,
    int num_points,
    double* out_matrix_flat
) {
    if (!rho_bh_arr || !rho_pz_arr || !rho_stratum_arr || num_points <= 0 || !out_matrix_flat) {
        setLastError("Invalid batch parameters.");
        return BK_ERR_INVALID_PARAMS;
    }

    int single_point_vals = static_cast<int>(modes_.size() * electrodes_.size());

    for (int p = 0; p < num_points; ++p) {
        double* current_out = out_matrix_flat + p * single_point_vals;
        BK_Status st = solveResistivity(rho_bh_arr[p], rho_pz_arr[p], rho_stratum_arr[p],
                                        current_out, single_point_vals);
        if (st != BK_STATUS_OK) return st;
    }

    return BK_STATUS_OK;
}

BK_Status BKSolver::solve(
    const BK_MediumParams* medium,
    double* out_values,
    int out_values_count
) {
    if (!medium) {
        setLastError("Medium parameters pointer is null.");
        return BK_ERR_INVALID_PARAMS;
    }

    if (std::abs(medium->r_bh - current_r_bh_) > 1e-5 || std::abs(medium->r_pz - current_r_pz_) > 1e-5) {
        BK_Status st = setMediumGeometry(medium->r_bh, medium->r_pz);
        if (st != BK_STATUS_OK) return st;
    }

    return solveResistivity(medium->rho_bh, medium->rho_pz, medium->rho_stratum,
                            out_values, out_values_count);
}

void BKSolver::getProbeInfo(int* out_num_electrodes, int* out_num_modes) const {
    if (out_num_electrodes) *out_num_electrodes = static_cast<int>(electrodes_.size());
    if (out_num_modes) *out_num_modes = static_cast<int>(modes_.size());
}

BK_Status BKSolver::getElectrodeInfo(
    int electrode_index,
    char* out_name,
    int max_name_len,
    double* out_z_start,
    double* out_length
) const {
    if (electrode_index < 0 || electrode_index >= static_cast<int>(electrodes_.size())) {
        setLastError("Electrode index out of range: " + std::to_string(electrode_index));
        return BK_ERR_INVALID_PARAMS;
    }

    const auto& el = electrodes_[electrode_index];
    if (out_name && max_name_len > 0) {
        strncpy_s(out_name, max_name_len, el.name.c_str(), _TRUNCATE);
    }
    if (out_z_start) *out_z_start = el.z_start;
    if (out_length) *out_length = el.length;

    return BK_STATUS_OK;
}

void BKSolver::getGridDimensions(int* out_nr, int* out_nz) const {
    if (out_nr) *out_nr = W_;
    if (out_nz) *out_nz = H_;
}

void BKSolver::getGridNodes(
    double* out_r_nodes,
    int nr,
    double* out_z_nodes,
    int nz
) const {
    if (out_r_nodes && nr >= W_) {
        std::copy(r_coords_.begin(), r_coords_.end(), out_r_nodes);
    }
    if (out_z_nodes && nz >= H_) {
        std::copy(z_coords_.begin(), z_coords_.end(), out_z_nodes);
    }
}

BK_Status BKSolver::getFieldMaps(
    int mode_index,
    double* out_potentials_2d,
    double* out_currents_r_2d,
    double* out_currents_z_2d
) const {
    if (mode_index < 0 || mode_index >= static_cast<int>(modes_.size())) {
        setLastError("Mode index out of range: " + std::to_string(mode_index));
        return BK_ERR_INVALID_PARAMS;
    }

    if (mode_potentials_cache_.empty() || mode_potentials_cache_[mode_index].empty()) {
        setLastError("Field maps not computed yet. Run solveResistivity first.");
        return BK_ERR_INVALID_PARAMS;
    }

    const auto& V = mode_potentials_cache_[mode_index];

    if (out_potentials_2d) {
        std::copy(V.begin(), V.end(), out_potentials_2d);
    }

    if (out_currents_r_2d) {
        for (int z = 0; z < H_; ++z) {
            for (int r = 0; r < W_; ++r) {
                int idx = z * W_ + r;
                if (r < W_ - 1) {
                    out_currents_r_2d[idx] = (V[idx] - V[idx + 1]) * G_hor_[idx];
                } else {
                    out_currents_r_2d[idx] = 0.0;
                }
            }
        }
    }

    if (out_currents_z_2d) {
        for (int z = 0; z < H_; ++z) {
            for (int r = 0; r < W_; ++r) {
                int idx = z * W_ + r;
                if (z < H_ - 1) {
                    out_currents_z_2d[idx] = (V[idx] - V[idx + W_]) * G_ver_[idx];
                } else {
                    out_currents_z_2d[idx] = 0.0;
                }
            }
        }
    }

    return BK_STATUS_OK;
}

} // namespace BK
