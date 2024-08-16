//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// main.cpp
//
// Code generation for function 'main'
//

/*************************************************************************/
/* This automatically generated example C++ main file shows how to call  */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/

// Include files
#include "main.h"
#include "TIME_entrypoint.h"
#include "TIME_entrypoint_terminate.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Declarations
static coder::array<double, 1U> argInit_Unboundedx1_real_T();

static double argInit_real_T();

// Function Definitions
static coder::array<double, 1U> argInit_Unboundedx1_real_T()
{
  coder::array<double, 1U> result;
  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(2);
  // Loop over the array to initialize each element.
  for (int idx0{0}; idx0 < result.size(0); idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[idx0] = argInit_real_T();
  }
  return result;
}

static double argInit_real_T()
{
  return 10.0;
}

int main(int, char **)
{
  // The initialize function is being called automatically from your entry-point
  // function. So, a call to initialize is not included here. Invoke the
  // entry-point functions.
  // You can call entry-point functions multiple times.
  main_TIME_entrypoint();
  // Terminate the application.
  // You do not need to do this more than one time.
  TIME_entrypoint_terminate();
  return 0;
}

void main_TIME_entrypoint()
{
  coder::array<double, 2U> ZTW_HNGD_SPEC;
  coder::array<double, 2U> ZTW_SPEC;
  coder::array<double, 1U> wav;
  double fs_tmp;
  // Initialize function 'TIME_entrypoint' input arguments.
  // Initialize function input argument 'wav'.
  wav = argInit_Unboundedx1_real_T();
  fs_tmp = argInit_real_T();
  // Call the entry-point 'TIME_entrypoint'.
  TIME_entrypoint(wav, fs_tmp, fs_tmp, fs_tmp, ZTW_SPEC, ZTW_HNGD_SPEC);
}

// End of code generation (main.cpp)
