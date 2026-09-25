#include "bk_engine_api.h"
#include "bk_solver.h"
#include <cstring>
#include <string>

static std::string g_last_global_error = "";

static BK::BKSolver* getSolver(BKSolverHandle handle) {
    return reinterpret_cast<BK::BKSolver*>(handle);
}

extern "C" {

BK_Status BK_API BK_CreateSolver(
    const BK_ElectrodeDef* electrodes,
    int num_electrodes,
    const BK_WorkModeDef* modes,
    int num_modes,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
) {
    if (!out_handle) return BK_ERR_INVALID_PARAMS;
    *out_handle = nullptr;

    try {
        auto solver = new BK::BKSolver();
        BK_Status status = solver->initialize(electrodes, num_electrodes, modes, num_modes, grid_params);
        if (status != BK_STATUS_OK) {
            g_last_global_error = solver->getLastError();
            delete solver;
            return status;
        }
        *out_handle = reinterpret_cast<BKSolverHandle>(solver);
        return BK_STATUS_OK;
    } catch (const std::exception& ex) {
        g_last_global_error = ex.what();
        return BK_ERR_SOLVER_FAILED;
    } catch (...) {
        g_last_global_error = "Unknown exception in BK_CreateSolver";
        return BK_ERR_SOLVER_FAILED;
    }
}

BK_Status BK_API BK_CreateSolverFromConfigFile(
    const char* config_filepath,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
) {
    if (!out_handle || !config_filepath) return BK_ERR_INVALID_PARAMS;
    *out_handle = nullptr;

    try {
        auto solver = new BK::BKSolver();
        BK_Status status = solver->loadFromConfigFile(config_filepath, grid_params);
        if (status != BK_STATUS_OK) {
            g_last_global_error = solver->getLastError();
            delete solver;
            return status;
        }
        *out_handle = reinterpret_cast<BKSolverHandle>(solver);
        return BK_STATUS_OK;
    } catch (const std::exception& ex) {
        g_last_global_error = ex.what();
        return BK_ERR_SOLVER_FAILED;
    } catch (...) {
        g_last_global_error = "Unknown exception in BK_CreateSolverFromConfigFile";
        return BK_ERR_SOLVER_FAILED;
    }
}

BK_Status BK_API BK_DestroySolver(BKSolverHandle handle) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    auto solver = getSolver(handle);
    delete solver;
    return BK_STATUS_OK;
}

BK_Status BK_API BK_SetMediumGeometry(
    BKSolverHandle handle,
    double r_bh,
    double r_pz
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->setMediumGeometry(r_bh, r_pz);
}

BK_Status BK_API BK_SolveResistivity(
    BKSolverHandle handle,
    double rho_bh,
    double rho_pz,
    double rho_stratum,
    double* out_values,
    int out_values_count
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->solveResistivity(rho_bh, rho_pz, rho_stratum, out_values, out_values_count);
}

BK_Status BK_API BK_SolveResistivityBatch(
    BKSolverHandle handle,
    const double* rho_bh_arr,
    const double* rho_pz_arr,
    const double* rho_stratum_arr,
    int num_points,
    double* out_matrix_flat
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->solveResistivityBatch(rho_bh_arr, rho_pz_arr, rho_stratum_arr, num_points, out_matrix_flat);
}

BK_Status BK_API BK_Solve(
    BKSolverHandle handle,
    const BK_MediumParams* medium,
    double* out_values,
    int out_values_count
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->solve(medium, out_values, out_values_count);
}

BK_Status BK_API BK_GetProbeInfo(
    BKSolverHandle handle,
    int* out_num_electrodes,
    int* out_num_modes
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    getSolver(handle)->getProbeInfo(out_num_electrodes, out_num_modes);
    return BK_STATUS_OK;
}

BK_Status BK_API BK_GetElectrodeInfo(
    BKSolverHandle handle,
    int electrode_index,
    char* out_name,
    int max_name_len,
    double* out_z_start,
    double* out_length
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->getElectrodeInfo(electrode_index, out_name, max_name_len, out_z_start, out_length);
}

BK_Status BK_API BK_GetGridDimensions(BKSolverHandle handle, int* out_nr, int* out_nz) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    getSolver(handle)->getGridDimensions(out_nr, out_nz);
    return BK_STATUS_OK;
}

BK_Status BK_API BK_GetGridNodes(
    BKSolverHandle handle,
    double* out_r_nodes,
    int nr,
    double* out_z_nodes,
    int nz
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    getSolver(handle)->getGridNodes(out_r_nodes, nr, out_z_nodes, nz);
    return BK_STATUS_OK;
}

BK_Status BK_API BK_GetFieldMaps(
    BKSolverHandle handle,
    int mode_index,
    double* out_potentials_2d,
    double* out_currents_r_2d,
    double* out_currents_z_2d
) {
    if (!handle) return BK_ERR_INVALID_HANDLE;
    return getSolver(handle)->getFieldMaps(mode_index, out_potentials_2d, out_currents_r_2d, out_currents_z_2d);
}

BK_Status BK_API BK_GetLastErrorMessage(BKSolverHandle handle, char* buffer, int max_len) {
    if (!buffer || max_len <= 0) return BK_ERR_INVALID_PARAMS;

    std::string msg = "";
    if (handle) {
        msg = getSolver(handle)->getLastError();
    }
    if (msg.empty()) {
        msg = g_last_global_error;
    }

#if defined(_WIN32)
    strncpy_s(buffer, max_len, msg.c_str(), _TRUNCATE);
#else
    strncpy(buffer, msg.c_str(), max_len - 1);
    buffer[max_len - 1] = '\0';
#endif

    return BK_STATUS_OK;
}

} // extern "C"
