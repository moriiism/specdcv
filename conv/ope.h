#ifndef MORIIISM_HSCDCV_DECONV_OPE_H_
#define MORIIISM_HSCDCV_DECONV_OPE_H_

#include <unistd.h>
#include "mib_blas.h"
#include "mi_iolib.h"
#include "mir_math.h"

namespace Ope
{
    void GetDetArr(const double* const sky_arr,
                   const double* const resp_mat_arr,
                   int ndet, int nsky,
                   double* const det_arr);

    double GetSum(long narr,
                  const double* const val_arr);

} // namespace Ope

#endif // MORIIISM_HSCDCV_DECONV_OPE_H_
