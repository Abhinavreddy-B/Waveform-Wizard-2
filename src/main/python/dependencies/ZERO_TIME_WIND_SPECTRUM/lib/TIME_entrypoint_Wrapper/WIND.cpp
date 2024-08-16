#include "TIME_entrypoint.h"
#include "TIME_entrypoint_terminate.h"
#include "TIME_entrypoint_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "omp.h"

std::pair<std::vector<double>, std::vector<double>> main_TIME_entrypoint(std::vector<double> input_data, double fs, double N, double M)
{
    coder::array<double, 2U> ZTW_SPEC;
    coder::array<double, 2U> ZTW_HNGD_SPEC;
    coder::array<double, 1U> wav(input_data);
    TIME_entrypoint(wav, fs, N, M, ZTW_SPEC, ZTW_HNGD_SPEC);

    TIME_entrypoint_terminate();

    std::vector<double> ztwSpec(ZTW_SPEC.begin(), ZTW_SPEC.end());
    std::vector<double> ztwHngdSpec(ZTW_HNGD_SPEC.begin(), ZTW_HNGD_SPEC.end());

    return std::make_pair(ztwSpec, ztwHngdSpec);
}

extern "C" {
    void WIND(const double *data, int data_size, double fs, double N, double M, double **result_ZTW_SPEC, double **result_ZTW_HNGD_SPEC, int *result_ztwHngdSpec_size, int *result_ztwSpec_size) {
      std::vector<double> input_data(data, data + data_size);
      auto output_data = main_TIME_entrypoint(input_data, fs, N, M);
      auto [ztwSpec, ztwHngdSpec] = output_data;

      *result_ztwSpec_size = ztwSpec.size();
      *result_ZTW_SPEC = (double *)malloc(sizeof(double) * ztwSpec.size());
      std::copy(ztwSpec.begin(), ztwSpec.end(), *result_ZTW_SPEC);

      *result_ztwHngdSpec_size = ztwHngdSpec.size();
      *result_ZTW_HNGD_SPEC = (double *)malloc(sizeof(double) * ztwHngdSpec.size());
      std::copy(ztwHngdSpec.begin(), ztwHngdSpec.end(), *result_ZTW_HNGD_SPEC);

    }

    void free_array(int *array) {
        free(array);
    }
}
