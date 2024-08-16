//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// _coder_TIME_entrypoint_info.cpp
//
// Code generation for function 'TIME_entrypoint'
//

// Include files
#include "_coder_TIME_entrypoint_info.h"
#include "emlrt.h"
#include "tmwtypes.h"

// Function Declarations
static const mxArray *emlrtMexFcnResolvedFunctionsInfo();

// Function Definitions
static const mxArray *emlrtMexFcnResolvedFunctionsInfo()
{
  const mxArray *nameCaptureInfo;
  const char_T *data[4]{
      "789c6360f4f465646060e06380800c3608cd0be50b4069260654802ecf88838601560616"
      "147d30f97e289d9c9f57925a5102e1e425e6a6c275a6e4e766e625e6"
      "95845416a43214a516e7e794a5a68065d2327352433273538391397e205eae1b92149c03"
      "9202b19d335293b3834b73198a328a112ecc41e6c0c363050effb210",
      "191e8e38c243004d3eda35563f233f3755bf38b138332523b1483f3835d74cdf29240084"
      "e31d0b0af453f34a8a2ae30bf233f34a8af5433c7d5de3c1226001bd"
      "5c34775750e86e7102ee86c947b906f9c7831d13eee9e7121f1ce0ea1c1214ea0b774702"
      "85ee60c3e90e884c4a7e69524e2ac2df0b28b4cf06a77da8f244c417",
      "8ea0814415a1f0e127d2bdb8f21b2f0307988e997e38959ef61d9a78509f9ef6c1c040d9"
      "47693e13c5619f009a7c9681a9576165a09f73ba8b89978f417e6871"
      "545a0a923b0208d843c81d0c38f8b4361f008a076fac",
      ""};
  nameCaptureInfo = nullptr;
  emlrtNameCaptureMxArrayR2016a(&data[0], 1664U, &nameCaptureInfo);
  return nameCaptureInfo;
}

mxArray *emlrtMexFcnProperties()
{
  mxArray *xEntryPoints;
  mxArray *xInputs;
  mxArray *xResult;
  const char_T *propFieldName[9]{"Version",
                                 "ResolvedFunctions",
                                 "Checksum",
                                 "EntryPoints",
                                 "CoverageInfo",
                                 "IsPolymorphic",
                                 "PropertyList",
                                 "UUID",
                                 "ClassEntryPointIsHandle"};
  const char_T *epFieldName[8]{
      "Name",     "NumberOfInputs", "NumberOfOutputs", "ConstantInputs",
      "FullPath", "TimeStamp",      "Constructor",     "Visible"};
  xEntryPoints =
      emlrtCreateStructMatrix(1, 1, 8, (const char_T **)&epFieldName[0]);
  xInputs = emlrtCreateLogicalMatrix(1, 4);
  emlrtSetField(xEntryPoints, 0, "Name",
                emlrtMxCreateString("TIME_entrypoint"));
  emlrtSetField(xEntryPoints, 0, "NumberOfInputs",
                emlrtMxCreateDoubleScalar(4.0));
  emlrtSetField(xEntryPoints, 0, "NumberOfOutputs",
                emlrtMxCreateDoubleScalar(2.0));
  emlrtSetField(xEntryPoints, 0, "ConstantInputs", xInputs);
  emlrtSetField(
      xEntryPoints, 0, "FullPath",
      emlrtMxCreateString(
          "/home/sasidhar/Sem6/BTP/BTP_App/entry_points/TIME_entrypoint.m"));
  emlrtSetField(xEntryPoints, 0, "TimeStamp",
                emlrtMxCreateDoubleScalar(739299.86932870373));
  emlrtSetField(xEntryPoints, 0, "Constructor",
                emlrtMxCreateLogicalScalar(false));
  emlrtSetField(xEntryPoints, 0, "Visible", emlrtMxCreateLogicalScalar(true));
  xResult =
      emlrtCreateStructMatrix(1, 1, 9, (const char_T **)&propFieldName[0]);
  emlrtSetField(xResult, 0, "Version",
                emlrtMxCreateString("23.2.0.2485118 (R2023b) Update 6"));
  emlrtSetField(xResult, 0, "ResolvedFunctions",
                (mxArray *)emlrtMexFcnResolvedFunctionsInfo());
  emlrtSetField(xResult, 0, "Checksum",
                emlrtMxCreateString("bbx3MiJbBqeiJIXmgdseo"));
  emlrtSetField(xResult, 0, "EntryPoints", xEntryPoints);
  return xResult;
}

// End of code generation (_coder_TIME_entrypoint_info.cpp)
