/*
 * bk_engine_mex.cpp - MATLAB MEX wrapper gateway for BK_Engine
 *
 * Compilation inside MATLAB:
 *   mex -I. bk_engine_mex.cpp BK_Engine.lib (or link source files directly)
 */

#if defined(MATLAB_MEX_FILE)

#include "mex.h"
#include "bk_engine_api.h"
#include <string>
#include <vector>
#include <cstring>

static BKSolverHandle getHandleFromMx(const mxArray* mx) {
    if (!mx || !mxIsUint64(mx)) {
        mexErrMsgIdAndTxt("BK:MEX:InvalidHandle", "First argument must be a valid 64-bit uint64 solver handle.");
    }
    uint64_t val = *reinterpret_cast<uint64_t*>(mxGetData(mx));
    return reinterpret_cast<BKSolverHandle>(val);
}

static mxArray* createMxHandle(BKSolverHandle handle) {
    mxArray* mx = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *reinterpret_cast<uint64_t*>(mxGetData(mx)) = reinterpret_cast<uint64_t>(handle);
    return mx;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    if (nrhs < 1 || !mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("BK:MEX:Usage", 
            "Usage:\n"
            "  h = bk_engine_mex('create', config_file_or_struct)\n"
            "  bk_engine_mex('set_geom', h, r_bh, r_pz)\n"
            "  res = bk_engine_mex('solve', h, rho_bh, rho_pz, rho_t)\n"
            "  res_batch = bk_engine_mex('solve_batch', h, rho_bh_v, rho_pz_v, rho_t_v)\n"
            "  [V, Jr, Jz, r, z] = bk_engine_mex('get_field_maps', h, mode_idx)\n"
            "  bk_engine_mex('destroy', h)\n");
    }

    char cmd[64];
    mxGetString(prhs[0], cmd, sizeof(cmd));

    if (strcmp(cmd, "create") == 0) {
        if (nrhs < 2 || !mxIsChar(prhs[1])) {
            mexErrMsgIdAndTxt("BK:MEX:Create", "create requires JSON config path string.");
        }
        char cfg_path[512];
        mxGetString(prhs[1], cfg_path, sizeof(cfg_path));

        BKSolverHandle handle = nullptr;
        BK_Status status = BK_CreateSolverFromConfigFile(cfg_path, nullptr, &handle);
        if (status != BK_STATUS_OK) {
            char err[512];
            BK_GetLastErrorMessage(nullptr, err, sizeof(err));
            mexErrMsgIdAndTxt("BK:MEX:CreateFailed", "Failed to create solver: %s", err);
        }
        plhs[0] = createMxHandle(handle);
    }
    else if (strcmp(cmd, "destroy") == 0) {
        if (nrhs < 2) mexErrMsgIdAndTxt("BK:MEX:Destroy", "destroy requires handle.");
        BKSolverHandle handle = getHandleFromMx(prhs[1]);
        BK_DestroySolver(handle);
    }
    else if (strcmp(cmd, "set_geom") == 0) {
        if (nrhs < 4) mexErrMsgIdAndTxt("BK:MEX:SetGeom", "set_geom requires handle, r_bh, r_pz.");
        BKSolverHandle handle = getHandleFromMx(prhs[1]);
        double r_bh = mxGetScalar(prhs[2]);
        double r_pz = mxGetScalar(prhs[3]);
        BK_Status status = BK_SetMediumGeometry(handle, r_bh, r_pz);
        if (status != BK_STATUS_OK) {
            char err[512];
            BK_GetLastErrorMessage(handle, err, sizeof(err));
            mexErrMsgIdAndTxt("BK:MEX:SetGeomFailed", "%s", err);
        }
    }
    else if (strcmp(cmd, "solve") == 0) {
        if (nrhs < 5) mexErrMsgIdAndTxt("BK:MEX:Solve", "solve requires handle, rho_bh, rho_pz, rho_t.");
        BKSolverHandle handle = getHandleFromMx(prhs[1]);
        double rho_bh = mxGetScalar(prhs[2]);
        double rho_pz = mxGetScalar(prhs[3]);
        double rho_t  = mxGetScalar(prhs[4]);

        int nEls = 0, nModes = 0;
        BK_GetProbeInfo(handle, &nEls, &nModes);
        int total = nEls * nModes;

        // Создаем матрицу MATLAB [nModes x nEls]
        plhs[0] = mxCreateDoubleMatrix(nEls, nModes, mxREAL); // column-major => [nEls, nModes]
        double* out_ptr = mxGetPr(plhs[0]);

        BK_Status status = BK_SolveResistivity(handle, rho_bh, rho_pz, rho_t, out_ptr, total);
        if (status != BK_STATUS_OK) {
            char err[512];
            BK_GetLastErrorMessage(handle, err, sizeof(err));
            mexErrMsgIdAndTxt("BK:MEX:SolveFailed", "%s", err);
        }
    }
    else if (strcmp(cmd, "solve_batch") == 0) {
        if (nrhs < 5) mexErrMsgIdAndTxt("BK:MEX:Batch", "solve_batch requires handle, rho_bh_vec, rho_pz_vec, rho_t_vec.");
        BKSolverHandle handle = getHandleFromMx(prhs[1]);
        
        size_t n_pts = mxGetNumberOfElements(prhs[2]);
        if (mxGetNumberOfElements(prhs[3]) != n_pts || mxGetNumberOfElements(prhs[4]) != n_pts) {
            mexErrMsgIdAndTxt("BK:MEX:BatchSizeMismatch", "Vectors rho_bh, rho_pz, rho_t must have identical lengths.");
        }

        const double* r_bh = mxGetPr(prhs[2]);
        const double* r_pz = mxGetPr(prhs[3]);
        const double* r_t  = mxGetPr(prhs[4]);

        int nEls = 0, nModes = 0;
        BK_GetProbeInfo(handle, &nEls, &nModes);
        int single_len = nEls * nModes;

        mwSize dims[3] = { static_cast<mwSize>(nEls), static_cast<mwSize>(nModes), static_cast<mwSize>(n_pts) };
        plhs[0] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
        double* out_ptr = mxGetPr(plhs[0]);

        BK_Status status = BK_SolveResistivityBatch(handle, r_bh, r_pz, r_t, static_cast<int>(n_pts), out_ptr);
        if (status != BK_STATUS_OK) {
            char err[512];
            BK_GetLastErrorMessage(handle, err, sizeof(err));
            mexErrMsgIdAndTxt("BK:MEX:BatchFailed", "%s", err);
        }
    }
    else if (strcmp(cmd, "get_field_maps") == 0) {
        if (nrhs < 3) mexErrMsgIdAndTxt("BK:MEX:Maps", "get_field_maps requires handle, mode_idx.");
        BKSolverHandle handle = getHandleFromMx(prhs[1]);
        int mode_idx = static_cast<int>(mxGetScalar(prhs[2]));

        int nr = 0, nz = 0;
        BK_GetGridDimensions(handle, &nr, &nz);

        plhs[0] = mxCreateDoubleMatrix(nr, nz, mxREAL); // V
        plhs[1] = (nlhs > 1) ? mxCreateDoubleMatrix(nr, nz, mxREAL) : nullptr; // Jr
        plhs[2] = (nlhs > 2) ? mxCreateDoubleMatrix(nr, nz, mxREAL) : nullptr; // Jz
        
        double* pV  = mxGetPr(plhs[0]);
        double* pJr = (plhs[1]) ? mxGetPr(plhs[1]) : nullptr;
        double* pJz = (plhs[2]) ? mxGetPr(plhs[2]) : nullptr;

        BK_Status status = BK_GetFieldMaps(handle, mode_idx, pV, pJr, pJz);
        if (status != BK_STATUS_OK) {
            char err[512];
            BK_GetLastErrorMessage(handle, err, sizeof(err));
            mexErrMsgIdAndTxt("BK:MEX:MapsFailed", "%s", err);
        }

        if (nlhs > 3) {
            plhs[3] = mxCreateDoubleMatrix(nr, 1, mxREAL);
            plhs[4] = mxCreateDoubleMatrix(nz, 1, mxREAL);
            BK_GetGridNodes(handle, mxGetPr(plhs[3]), nr, mxGetPr(plhs[4]), nz);
        }
    }
    else {
        mexErrMsgIdAndTxt("BK:MEX:UnknownCommand", "Unknown command: %s", cmd);
    }
}

#endif // MATLAB_MEX_FILE
