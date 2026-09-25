#ifndef BK_SOLVER_H
#define BK_SOLVER_H

#include "bk_engine_api.h"
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>

namespace BK {

struct ElectrodeInternal {
    std::string name;
    double z_start = 0.0;
    double length = 0.0;
    double z_end = 0.0;
    int z_idx_start = 0;
    int z_idx_end = 0;
};

struct ModeInternal {
    std::string name;
    std::vector<BK_ElectrodeModeState> electrode_states;
};

class BKSolver {
public:
    BKSolver();
    ~BKSolver() = default;

    BK_Status initialize(
        const BK_ElectrodeDef* electrodes,
        int num_electrodes,
        const BK_WorkModeDef* modes,
        int num_modes,
        const BK_GridParams* grid_params
    );

    BK_Status loadFromConfigFile(
        const char* config_filepath,
        const BK_GridParams* grid_params
    );

    BK_Status setMediumGeometry(double r_bh, double r_pz);

    BK_Status solveResistivity(
        double rho_bh,
        double rho_pz,
        double rho_stratum,
        double* out_values,
        int out_values_count
    );

    BK_Status solveResistivityBatch(
        const double* rho_bh_arr,
        const double* rho_pz_arr,
        const double* rho_stratum_arr,
        int num_points,
        double* out_matrix_flat
    );

    BK_Status solve(
        const BK_MediumParams* medium,
        double* out_values,
        int out_values_count
    );

    void getProbeInfo(int* out_num_electrodes, int* out_num_modes) const;

    BK_Status getElectrodeInfo(
        int electrode_index,
        char* out_name,
        int max_name_len,
        double* out_z_start,
        double* out_length
    ) const;

    void getGridDimensions(int* out_nr, int* out_nz) const;

    void getGridNodes(
        double* out_r_nodes,
        int nr,
        double* out_z_nodes,
        int nz
    ) const;

    BK_Status getFieldMaps(
        int mode_index,
        double* out_potentials_2d,
        double* out_currents_r_2d,
        double* out_currents_z_2d
    ) const;

    const std::string& getLastError() const { return last_error_; }

private:
    void setLastError(const std::string& err) const {
        std::lock_guard<std::mutex> lock(error_mutex_);
        last_error_ = err;
    }

    void buildGrids();
    void buildConductanceGeometry();
    void updateResistivityConductances(double rho_bh, double rho_pz, double rho_stratum);
    BK_Status solveSingleMode(int mode_idx, std::vector<double>& mode_potentials);

    int getZIndex(double target_z) const;
    int getRIndex(double target_r) const;

    // Зонд и режимы
    std::string probe_name_;
    double tool_radius_ = 0.045;
    std::vector<ElectrodeInternal> electrodes_;
    std::vector<ModeInternal> modes_;
    BK_GridParams grid_params_;

    // Геометрия среды
    double current_r_bh_ = 0.108;
    double current_r_pz_ = 0.400;
    bool geometry_initialized_ = false;

    // Сетка (W = nr, H = nz)
    std::vector<double> r_coords_;
    std::vector<double> z_coords_;
    int W_ = 0;
    int H_ = 0;
    int N_ = 0;

    // Геометрические факторы проводимостей
    // G_hor = geom_G_hor / rho, G_ver = geom_G_ver / rho
    std::vector<double> geom_G_hor_;
    std::vector<double> geom_G_ver_;
    std::vector<int> cell_zone_id_; // 0: borehole, 1: invaded zone, 2: virgin formation

    // Текущие проводимости (1/R)
    std::vector<double> G_hor_;
    std::vector<double> G_ver_;

    // Кэш решений потенциалов для каждого режима [num_modes][N]
    std::vector<std::vector<double>> mode_potentials_cache_;

    mutable std::string last_error_;
    mutable std::mutex error_mutex_;
};

} // namespace BK

#endif // BK_SOLVER_H
