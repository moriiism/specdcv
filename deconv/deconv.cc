// deconvolution by Richardson-Lucy method

#include "mi_time.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "arg_deconv.h"
#include "rl.h"
#include "mir_math.h"
// #include "srtmathlib.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    double time_st = MiTime::GetTimeSec();
    
    ArgValDeconv* argval = new ArgValDeconv;
    argval->Init(argc, argv);
    argval->Print(stdout);

    char logfile[kLineSize];
    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
        char cmd[kLineSize];
        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
        int ret = system(cmd);
        (void) ret;
    }
    sprintf(logfile, "%s/%s_%s.log",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str(),
            argval->GetProgname().c_str());
    FILE* fp_log = fopen(logfile, "w");
    argval->Print(fp_log);
    MiIolib::Printf2(fp_log, "-----------------------------\n");    

    int nskys = argval->GetNskys();
    int nskyz = argval->GetNskyz();
    int ndetx = argval->GetNdetx();
    int ndety = argval->GetNdety();
    int nsky = nskys * nskyz;
    int ndet = ndetx * ndety;    

    // load data
    MifImgInfo* img_info_data = new MifImgInfo;
    img_info_data->InitSetImg(1, 1, ndetx, ndety);
    int bitpix_data = 0;
    double* data_arr = NULL;
    MifFits::InFitsImageD(argval->GetDataFile(), img_info_data,
                          &bitpix_data, &data_arr);
    int nph_data = MirMath::GetSum(ndet, data_arr);
    MiIolib::Printf2(fp_log, "N photon = %d\n", nph_data);

    // load bg model data
    double* bg_arr = NULL;
    //    if(argval->GetBgFile() == "none"){
    bg_arr = new double[ndet];
    for(int idet = 0; idet < ndet; idet++){
        bg_arr[idet] = 0.0;
    }
    //    } else {
    //        MifImgInfo* img_info_bg = new MifImgInfo;
    //        img_info_bg->InitSetImg(1, 1, ndetx, ndety);
    //        int bitpix_bg = 0;
    //        MifFits::InFitsImageD(argval->GetBgFile(), img_info_bg,
    //                              &bitpix_bg, &bg_arr);
    //        int nph_bg = SrtMathlib::GetSum(ndet, bg_arr);
    //        MiIolib::Printf2(fp_log, "N bg = %d\n", nph_bg);
    //        delete img_info_bg;
    //    }

    // load response file
    int naxis0 = MifFits::GetAxisSize(argval->GetRespFile(), 0);
    int naxis1 = MifFits::GetAxisSize(argval->GetRespFile(), 1);
    if ((naxis0 != ndet) || (naxis1 != nsky)){
        MiIolib::Printf2(fp_log,
                         "Error: response file size error.\n");
        abort();
    }
    double* resp_mat_arr = NULL;
    int bitpix_resp = 0;
    MifImgInfo* img_info_resp = new MifImgInfo;
    img_info_resp->InitSetImg(1, 1, ndet, nsky);
    MifFits::InFitsImageD(argval->GetRespFile(),
                          img_info_resp,
                          &bitpix_resp,
                          &resp_mat_arr);

    //// load efficiency file
    //double* eff_mat_arr = NULL;
    //int bitpix_eff = 0;
    //MifImgInfo* img_info_eff = new MifImgInfo;
    //img_info_eff->InitSetImg(1, 1, nskyx, nskyy);
    //MifFits::InFitsImageD(argval->GetEffFile(), img_info_eff,
    //                      &bitpix_eff, &eff_mat_arr);

//    // check
//    for(int iskyy = 0; iskyy < nskyy; iskyy ++){
//        for(int iskyx = 0; iskyx < nskyx; iskyx ++){
//            int isky = nskyx * iskyy + iskyx;
//            int imat = isky * ndet;            
//            double resp_sum = 0.0;
//            for(int idet = 0; idet < ndet; idet ++){
//                resp_sum += resp_mat_arr[imat + idet];
//            }
//            if ( fabs(resp_sum - 1.0) > 1.0e-10){
//                // printf("warning: resp_sum = %e\n",
//                // resp_sum);
//            }
//        }
//    }

    // sky image to be reconstructed
    double* sky_init_arr = new double[nsky];
    for(int isky = 0; isky < nsky; isky ++){
        sky_init_arr[isky] = float(nph_data) / nsky;
    }
    printf("nph_data = %d, nsky = %d\n", nph_data, nsky);
    
    double* sky_new_arr = new double[nsky];
    if (argval->GetAccMethod() == "none"){
        SrtlibRl::Richlucy(
            fp_log,
            sky_init_arr,
            data_arr, bg_arr,
            resp_mat_arr,
            ndet, nsky,
            argval->GetOutdir(), argval->GetOutfileHead(),
            argval->GetNem(), argval->GetTolEm(),
            sky_new_arr);
    } else {
        printf("bad acc_method\n");
        abort();
    }
    
    double sum_sky_new = MirMath::GetSum(nsky, sky_new_arr);
    MiIolib::Printf2(fp_log, "sum_sky_new = %e\n", sum_sky_new);

    //// div by eff_arr
    //for(int isky = 0; isky < nsky; isky ++){
    //    sky_new_arr[isky] /= eff_mat_arr[isky];
    //}
    
    long naxes[2];
    naxes[0] = nskys;
    naxes[1] = nskyz;
    int bitpix_out = -32;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "rec", 2,
                           bitpix_out,
                           naxes, sky_new_arr);

    double time_ed = MiTime::GetTimeSec();
    MiIolib::Printf2(fp_log,
                     "duration = %e sec.\n", time_ed - time_st);

    fclose(fp_log);
    
    return status_prog;
}
