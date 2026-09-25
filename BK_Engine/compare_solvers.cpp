#include "bk_engine_api.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>

// =========================================================================
// 1. Имитация классического ResistorGrid из BK_PALETTE_CREATOR
// =========================================================================

struct LayerDakhnov {
    double r_boundary;
    double rho;
};

struct OldElectrode {
    double z_start_m;
    double z_end_m;
    double voltage;
    std::string name;
};

struct OldMeasElectrode {
    double z_start_m;
    double z_end_m;
    std::string name;
};

class OldResistorGrid {
private:
    int W, H, N;
    std::vector<double> r_coords;
    std::vector<double> z_coords;
    std::vector<double> v;
    std::vector<double> R_hor;
    std::vector<double> R_ver;
    std::vector<int> is_fixed;
    std::vector<double> fixed_v;
    std::vector<OldMeasElectrode> saved_m_els;

    int getZIndex(double target_z) const {
        auto it = std::lower_bound(z_coords.begin(), z_coords.end(), target_z);
        if (it == z_coords.end()) return H - 1;
        if (it == z_coords.begin()) return 0;
        if (*it - target_z < target_z - *(it - 1)) return (int)std::distance(z_coords.begin(), it);
        return (int)std::distance(z_coords.begin(), it - 1);
    }

    double getRhoForRadius(double r, const std::vector<LayerDakhnov>& zones) const {
        for (const auto& zone : zones) if (r <= zone.r_boundary) return zone.rho;
        return zones.empty() ? 100.0 : zones.back().rho;
    }

public:
    OldResistorGrid(const std::vector<double>& r_nodes, const std::vector<double>& z_nodes) {
        r_coords = r_nodes; z_coords = z_nodes;
        W = (int)r_coords.size(); H = (int)z_coords.size(); N = W * H;
        v.assign(N, 0.0); R_hor.assign(N, 1.0); R_ver.assign(N, 1.0);
        is_fixed.assign(N, 0); fixed_v.assign(N, 0.0);
    }

    void fillModel(const std::vector<LayerDakhnov>& zones) {
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
                    R_hor[idx] = (getRhoForRadius((curr_r + next_r) / 2.0, zones) / (2.0 * PI * h_eff)) * std::log(next_r / curr_r);
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
    }

    void setSonde(const std::vector<OldElectrode>& a_els, const std::vector<OldElectrode>& b_els, const std::vector<OldMeasElectrode>& m_els) {
        saved_m_els = m_els;
        auto apply_fixed = [&](const OldElectrode& el) {
            for (int z = getZIndex(el.z_start_m); z <= getZIndex(el.z_end_m); z++) {
                fixed_v[z * W] = el.voltage; is_fixed[z * W] = 1; v[z * W] = el.voltage;
            }
        };
        for (const auto& a : a_els) apply_fixed(a);
        for (const auto& b : b_els) apply_fixed(b);

        for (int z = 0; z < H; z++) {
            fixed_v[z * W + (W - 1)] = 0.0;
            is_fixed[z * W + (W - 1)] = 1;
            v[z * W + (W - 1)] = 0.0;
        }
        for (const auto& m : saved_m_els) {
            for (int z = getZIndex(m.z_start_m); z < getZIndex(m.z_end_m); z++) R_ver[z * W] = 1e-4;
        }
    }

    void solveDirect() {
        std::vector<Eigen::Triplet<double>> triplets;
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
                    B_vec(i) = 0.0;
                }
            }
        }
        Eigen::SparseMatrix<double> A(N, N);
        A.setFromTriplets(triplets.begin(), triplets.end());
        Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
        solver.compute(A);
        Eigen::VectorXd X = solver.solve(B_vec);
        for (int i = 0; i < N; i++) v[i] = X(i);
    }

    double getElectrodeCurrent(const OldElectrode& el) const {
        double I = 0.0;
        for (int z = getZIndex(el.z_start_m); z <= getZIndex(el.z_end_m); z++) I += (v[z * W] - v[z * W + 1]) / R_hor[z * W];
        return I;
    }

    double getMeasElectrodeVoltage(const OldMeasElectrode& el) const {
        int start_idx = getZIndex(el.z_start_m);
        int end_idx = getZIndex(el.z_end_m);
        double sum_v = 0.0;
        int count = 0;
        for (int z = start_idx; z <= end_idx; z++) {
            sum_v += v[z * W];
            count++;
        }
        return count == 0 ? 0.0 : sum_v / count;
    }
};

// =========================================================================
// 2. Тестовая программа сравнения
// =========================================================================

int main() {
    std::cout << "=================================================================" << std::endl;
    std::cout << " СРАВНЕНИЕ РАСЧЕТА: BK_PALETTE_CREATOR (Old) vs BK_Engine.dll (New)" << std::endl;
    std::cout << "=================================================================" << std::endl;

    // Параметры среды
    double r_bh = 0.108;
    double rho_bh = 1.0;
    double r_pz = 0.400;
    double rho_pz = 10.0;
    double rho_t = 50.0;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Параметры среды:\n"
              << "  Радиус скважины  R_bh = " << r_bh << " м,  Ro_bh = " << rho_bh << " Ом*м\n"
              << "  Радиус зоны зонд R_pz = " << r_pz << " м,  Ro_pz = " << rho_pz << " Ом*м\n"
              << "  УЭС пласта       Ro_t  = " << rho_t << " Ом*м\n\n";

    // --- 1. РАСЧЕТ НА BK_Engine.dll ---
    BKSolverHandle handle = nullptr;
    BK_CreateSolverFromConfigFile("sample_sonde_bk3.json", nullptr, &handle);
    BK_SetMediumGeometry(handle, r_bh, r_pz);

    int n_els = 0, n_modes = 0;
    BK_GetProbeInfo(handle, &n_els, &n_modes);
    std::vector<double> dll_results(n_els * n_modes, 0.0);

    int nr = 0, nz = 0;
    BK_GetGridDimensions(handle, &nr, &nz);
    std::vector<double> r_nodes(nr), z_nodes(nz);
    BK_GetGridNodes(handle, r_nodes.data(), nr, z_nodes.data(), nz);

    auto t0_dll = std::chrono::high_resolution_clock::now();
    BK_SolveResistivity(handle, rho_bh, rho_pz, rho_t, dll_results.data(), (int)dll_results.size());
    auto t1_dll = std::chrono::high_resolution_clock::now();
    double time_dll_ms = std::chrono::duration<double, std::milli>(t1_dll - t0_dll).count();

    // --- 2. РАСЧЕТ НА ИСХОДНОМ АЛГОРИТМЕ BK_PALETTE_CREATOR (на той же сетке) ---
    std::vector<LayerDakhnov> model = {
        { r_bh, rho_bh },
        { r_pz, rho_pz },
        { 30.0, rho_t }
    };

    OldResistorGrid old_grid(r_nodes, z_nodes);
    old_grid.fillModel(model);

    std::vector<OldElectrode> A_gr = {
        { 0.00, 0.20, 1.0, "A0" },
        { 1.50, 2.00, 1.0, "A1" }
    };
    std::vector<OldElectrode> B_gr = {
        { 3.00, 4.00, 0.0, "B0" }
    };
    std::vector<OldMeasElectrode> M_gr = {
        { 0.35, 0.40, "M1" },
        { 0.60, 0.65, "N1" },
        { 0.90, 0.95, "M2" },
        { 1.20, 1.25, "N2" }
    };

    auto t0_old = std::chrono::high_resolution_clock::now();
    old_grid.setSonde(A_gr, B_gr, M_gr);
    old_grid.solveDirect();
    auto t1_old = std::chrono::high_resolution_clock::now();
    double time_old_ms = std::chrono::duration<double, std::milli>(t1_old - t0_old).count();

    double old_I_A0 = old_grid.getElectrodeCurrent(A_gr[0]);
    double old_I_A1 = old_grid.getElectrodeCurrent(A_gr[1]);
    double old_I_B0 = old_grid.getElectrodeCurrent(B_gr[0]);
    double old_V_M1 = old_grid.getMeasElectrodeVoltage(M_gr[0]);
    double old_V_N1 = old_grid.getMeasElectrodeVoltage(M_gr[1]);
    double old_V_M2 = old_grid.getMeasElectrodeVoltage(M_gr[2]);
    double old_V_N2 = old_grid.getMeasElectrodeVoltage(M_gr[3]);

    // --- 3. СРАВНИТЕЛЬНАЯ ТАБЛИЦА ---
    std::cout << "================================================================================" << std::endl;
    std::cout << std::left << std::setw(12) << "Электрод"
              << std::setw(15) << "Тип"
              << std::setw(20) << "BK_PALETTE (Old)"
              << std::setw(20) << "BK_Engine (DLL)"
              << std::setw(15) << "Разница (%)" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    auto print_row = [](const std::string& name, const std::string& type, double old_val, double dll_val) {
        double diff = (old_val != 0.0) ? std::abs(dll_val - old_val) / std::abs(old_val) * 100.0 : 0.0;
        std::cout << std::left << std::setw(12) << name
                  << std::setw(15) << type
                  << std::setw(20) << std::setprecision(6) << old_val
                  << std::setw(20) << std::setprecision(6) << dll_val
                  << std::setw(15) << std::setprecision(4) << diff << "%" << std::endl;
    };

    print_row("A0", "Ток I [А]", old_I_A0, dll_results[0]);
    print_row("M1", "Потенциал V", old_V_M1, dll_results[1]);
    print_row("N1", "Потенциал V", old_V_N1, dll_results[2]);
    print_row("M2", "Потенциал V", old_V_M2, dll_results[3]);
    print_row("N2", "Потенциал V", old_V_N2, dll_results[4]);
    print_row("A1", "Ток I [А]", old_I_A1, dll_results[5]);
    print_row("B0", "Ток I [А]", old_I_B0, dll_results[6]);

    std::cout << "================================================================================" << std::endl;
    std::cout << "\nПроизводительность:\n";
    std::cout << "  BK_PALETTE_CREATOR (SparseLU):        " << std::setprecision(2) << time_old_ms << " мс\n";
    std::cout << "  BK_Engine.dll (SimplicialLDLT / SPD): " << std::setprecision(2) << time_dll_ms << " мс\n";
    std::cout << "  Ускорение DLL:                         " << std::setprecision(1) << (time_old_ms / time_dll_ms) << "x быстрее!\n";
    std::cout << "================================================================================" << std::endl;

    BK_DestroySolver(handle);
    return 0;
}
