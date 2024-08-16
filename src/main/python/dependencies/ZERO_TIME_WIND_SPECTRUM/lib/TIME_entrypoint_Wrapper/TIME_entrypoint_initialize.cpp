//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// TIME_entrypoint_initialize.cpp
//
// Code generation for function 'TIME_entrypoint_initialize'
//

// Include files
#include "TIME_entrypoint_initialize.h"
#include "TIME_entrypoint_data.h"
#include "rt_nonfinite.h"
#include "omp.h"

// Function Definitions
void TIME_entrypoint_initialize()
{
  omp_init_nest_lock(&TIME_entrypoint_nestLockGlobal);
  isInitialized_TIME_entrypoint = true;
}

// End of code generation (TIME_entrypoint_initialize.cpp)
