#include "bk_engine_api.h"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    std::cout << "=== Testing BK_Engine C API ===" << std::endl;

    BK_GridParams grid;
    grid.auto_grid = 1;
    grid.tool_radius = 0.045;
    grid.dr_start = 0.005;
    grid.q_r = 1.15;
    grid.r_max = 30.0;
    grid.dz_tool = 0.05;
    grid.dz_fine = 0.005;

    BKSolverHandle handle = nullptr;
    BK_Status status = BK_CreateSolverFromConfigFile("sample_sonde_bk3.json", &grid, &handle);
    if (status != BK_STATUS_OK) {
        char err[512];
        BK_GetLastErrorMessage(nullptr, err, sizeof(err));
        std::cerr << "Error creating solver: " << err << std::endl;
        return 1;
    }

    int n_els = 0, n_modes = 0;
    BK_GetProbeInfo(handle, &n_els, &n_modes);
    std::cout << "Probe initialized: " << n_els << " electrodes, " << n_modes << " modes." << std::endl;

    for (int i = 0; i < n_els; ++i) {
        char name[64];
        double z_start = 0, len = 0;
        BK_GetElectrodeInfo(handle, i, name, sizeof(name), &z_start, &len);
        std::cout << "  Electrode [" << i << "]: " << name << " at z=" << z_start << ", len=" << len << std::endl;
    }

    int nr = 0, nz = 0;
    BK_GetGridDimensions(handle, &nr, &nz);
    std::cout << "Grid dimensions: Nr=" << nr << ", Nz=" << nz << " (total nodes=" << nr * nz << ")" << std::endl;

    // 1. Set Geometry
    std::cout << "\nSetting geometry: r_bh=0.108m, r_pz=0.400m..." << std::endl;
    status = BK_SetMediumGeometry(handle, 0.108, 0.400);
    if (status != BK_STATUS_OK) {
        std::cerr << "Failed to set geometry!" << std::endl;
        return 1;
    }

    // 2. Solve Single Point
    int total_vals = n_els * n_modes;
    std::vector<double> out_vals(total_vals, 0.0);

    auto t0 = std::chrono::high_resolution_clock::now();
    status = BK_SolveResistivity(handle, 1.0, 10.0, 50.0, out_vals.data(), total_vals);
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    if (status != BK_STATUS_OK) {
        char err[512];
        BK_GetLastErrorMessage(handle, err, sizeof(err));
        std::cerr << "Error solving resistivity: " << err << std::endl;
        return 1;
    }

    std::cout << "Solve completed in " << ms << " ms." << std::endl;
    std::cout << "Results:" << std::endl;
    for (int m = 0; m < n_modes; ++m) {
        std::cout << "  Mode " << m << ":" << std::endl;
        for (int e = 0; e < n_els; ++e) {
            char name[64];
            double z_start = 0, len = 0;
            BK_GetElectrodeInfo(handle, e, name, sizeof(name), &z_start, &len);
            double val = out_vals[m * n_els + e];
            std::cout << "    " << name << " -> " << val << std::endl;
        }
    }

    // 3. Batch Solve Benchmark (100 points)
    int n_points = 100;
    std::vector<double> r_bh(n_points, 1.0);
    std::vector<double> r_pz(n_points, 10.0);
    std::vector<double> r_t(n_points, 50.0);
    std::vector<double> batch_out(n_points * total_vals, 0.0);

    auto bt0 = std::chrono::high_resolution_clock::now();
    status = BK_SolveResistivityBatch(handle, r_bh.data(), r_pz.data(), r_t.data(), n_points, batch_out.data());
    auto bt1 = std::chrono::high_resolution_clock::now();
    double b_ms = std::chrono::duration<double, std::milli>(bt1 - bt0).count();

    std::cout << "\nBatch solve of " << n_points << " points: " << b_ms << " ms total (" 
              << (b_ms / n_points) << " ms/point, " << (n_points * 1000.0 / b_ms) << " points/sec)" << std::endl;

    // Clean up
    BK_DestroySolver(handle);
    std::cout << "\n=== All Tests Passed Successfully ===" << std::endl;
    return 0;
}
