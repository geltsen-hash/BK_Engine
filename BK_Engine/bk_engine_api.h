#ifndef BK_ENGINE_API_H
#define BK_ENGINE_API_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #ifdef BK_EXPORTS
    #define BK_API __declspec(dllexport) __stdcall
  #else
    #define BK_API __declspec(dllimport) __stdcall
  #endif
#else
  #define BK_API
#endif

/* --- Коды возврата --- */
typedef enum {
    BK_STATUS_OK                  =  0,
    BK_ERR_INVALID_HANDLE         = -1,
    BK_ERR_INVALID_PARAMS         = -2,
    BK_ERR_CONFIG_FILE_NOT_FOUND  = -3,
    BK_ERR_CONFIG_PARSE_FAILED    = -4,
    BK_ERR_SINGULAR_MATRIX        = -5,
    BK_ERR_BUFFER_TOO_SMALL       = -6,
    BK_ERR_SOLVER_FAILED          = -7
} BK_Status;

/* --- Состояния электродов в режиме --- */
typedef enum {
    BK_ELECTRODE_CONNECTED = 0, /* Подключен к источнику U (при U=0 земля). На выходе: Ток I [А] */
    BK_ELECTRODE_FLOATING  = 1  /* Плавающий проводник (sum(I)=0).      На выходе: Потенциал V [В] */
} BK_ElectrodeState;

/* --- Геометрия электрода --- */
typedef struct {
    double z_start;       /* Начало электрода по оси Z [м] */
    double length;        /* Длина электрода [м]           */
    const char* name;     /* Имя электрода (например, "A0", "M1", "B0") */
} BK_ElectrodeDef;

/* --- Состояние электрода в режиме --- */
typedef struct {
    BK_ElectrodeState state;
    double fixed_voltage; /* Напряжение [В] (при state == BK_ELECTRODE_CONNECTED) */
} BK_ElectrodeModeState;

/* --- Описание режима работы --- */
typedef struct {
    const char* mode_name;               /* Имя режима ("Deep_AL_B1", "Medium_AM_B0") */
    const BK_ElectrodeModeState* states; /* Массив состояний размером [num_electrodes] */
} BK_WorkModeDef;

/* --- Параметры сеточной дискретизации --- */
typedef struct {
    int auto_grid;        /* 1 = автоматический расчет сетки, 0 = ручной */
    double tool_radius;   /* Радиус корпуса прибора [м] (по умолчанию ~0.045) */
    double dr_start;      /* Начальный радиальный шаг [м] */
    double q_r;           /* Множитель нарастания шага по r */
    double r_max;         /* Внешняя граница расчетной области [м] */
    double dz_tool;       /* Шаг по оси z в области прибора [м] */
    double dz_fine;       /* Сгущенный шаг по z на границах электродов [м] */
} BK_GridParams;

/* --- Полные параметры 3-слойной среды Дахнова --- */
typedef struct {
    double r_bh;          /* Радиус скважины [м] */
    double rho_bh;        /* УЭС бурового раствора [Ом*м] */
    double r_pz;          /* Радиус зоны проникновения [м] */
    double rho_pz;        /* УЭС зоны проникновения [Ом*м] */
    double rho_stratum;   /* УЭС неизмененной части пласта [Ом*м] */
} BK_MediumParams;

/* Непрозрачный указатель на экземпляр солвера */
typedef void* BKSolverHandle;

/* ========================================================================= */
/*                         ФУНКЦИИ API                                       */
/* ========================================================================= */

/* --- 1. Инициализация и жизненный цикл --- */
BK_Status BK_API BK_CreateSolver(
    const BK_ElectrodeDef* electrodes,
    int num_electrodes,
    const BK_WorkModeDef* modes,
    int num_modes,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
);

BK_Status BK_API BK_CreateSolverFromConfigFile(
    const char* config_filepath,
    const BK_GridParams* grid_params,
    BKSolverHandle* out_handle
);

BK_Status BK_API BK_DestroySolver(BKSolverHandle handle);

/* --- 2. Двухуровневый расчет палеток (High-Performance API) --- */

/* Внешний цикл: фиксация радиальных границ и адаптация сетки */
BK_Status BK_API BK_SetMediumGeometry(
    BKSolverHandle handle,
    double r_bh,
    double r_pz
);

/* Внутренний цикл: быстрый расчет откликов при изменении только 3 УЭС
   out_values: одномерный буфер размера [num_modes * num_electrodes]
   Для каждого режима и электрода:
     - если CONNECTED -> вычисленный ток I [А]
     - если FLOATING  -> вычисленный потенциал V [В]
*/
BK_Status BK_API BK_SolveResistivity(
    BKSolverHandle handle,
    double rho_bh,
    double rho_pz,
    double rho_stratum,
    double* out_values,
    int out_values_count
);

/* Векторный пакетный расчет пачки точек (УЭС) для зафиксированной геометрии
   out_matrix_flat: одномерный буфер размера [num_points * num_modes * num_electrodes]
*/
BK_Status BK_API BK_SolveResistivityBatch(
    BKSolverHandle handle,
    const double* rho_bh_arr,
    const double* rho_pz_arr,
    const double* rho_stratum_arr,
    int num_points,
    double* out_matrix_flat
);

/* --- 3. Атомарный расчет единичной точки среды (Stateless-style) --- */
BK_Status BK_API BK_Solve(
    BKSolverHandle handle,
    const BK_MediumParams* medium,
    double* out_values,
    int out_values_count
);

/* --- 4. Информация о зонде и сетке --- */
BK_Status BK_API BK_GetProbeInfo(
    BKSolverHandle handle,
    int* out_num_electrodes,
    int* out_num_modes
);

BK_Status BK_API BK_GetElectrodeInfo(
    BKSolverHandle handle,
    int electrode_index,
    char* out_name,
    int max_name_len,
    double* out_z_start,
    double* out_length
);

BK_Status BK_API BK_GetGridDimensions(BKSolverHandle handle, int* out_nr, int* out_nz);
BK_Status BK_API BK_GetGridNodes(
    BKSolverHandle handle,
    double* out_r_nodes,
    int nr,
    double* out_z_nodes,
    int nz
);

/* --- 5. Выгрузка 2D-полей потенциалов и токов (Caller-Allocated) --- */
BK_Status BK_API BK_GetFieldMaps(
    BKSolverHandle handle,
    int mode_index,
    double* out_potentials_2d,
    double* out_currents_r_2d,
    double* out_currents_z_2d
);

/* --- 6. Диагностика ошибок --- */
BK_Status BK_API BK_GetLastErrorMessage(BKSolverHandle handle, char* buffer, int max_len);

#ifdef __cplusplus
}
#endif

#endif /* BK_ENGINE_API_H */
