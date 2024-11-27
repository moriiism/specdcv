#include "ope.h"

void Ope::GetDetArr(const double* const sky_arr,
                    const double* const resp_mat_arr,
                    int ndet, int nsky,
                    double* const det_arr) // ndet
{
    // make detector image
    // det_arr = resp_mat_mat %*% sky_arr
    char transa[2];
    strcpy(transa, "N");

    // y := alpha*A*x + beta*y
    dgemv_(transa, ndet, nsky, 1.0,
           const_cast<double*>(resp_mat_arr), ndet,
           const_cast<double*>(sky_arr), 1,
           0.0, det_arr, 1);
}


double Ope::GetSum(
    long narr,
    const double* const val_arr)
{
    MiBase::IsValidArray(narr, val_arr);
    double ans = 0.0;
    for(long idata = 0; idata < narr; idata++){
        ans += val_arr[idata];
    }
    return ans;
}

