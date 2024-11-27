#include "mir_math.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mi_time.h"
#include "arg_conv.h"
#include "ope.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    double time_st = MiTime::GetTimeSec();
    
    ArgValConv* argval = new ArgValConv;
    argval->Init(argc, argv);
    argval->Print(stdout);

    char logfile[kLineSize];
    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
        char cmd[kLineSize];
        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
        int ret = system(cmd);
        (void) ret;
    }
    sprintf(logfile, "%s/%s.log",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    FILE* fp_log = fopen(logfile, "w");
    argval->Print(fp_log);
    MiIolib::Printf2(fp_log, "-----------------------------\n");    


    // image data size
    //int nskys = MifFits::GetAxisSize(argval->GetSkyFile(), 0); 
    //int nskyz = MifFits::GetAxisSize(argval->GetSkyFile(), 1);

    int nskys = argval->GetNskys();
    int nskyz = argval->GetNskyz();
    int ndetx = argval->GetNdetx();
    int ndety = argval->GetNdety();        
    int nsky = nskys * nskyz;
    int ndet = ndetx * ndety;
    
//    // load sky 
//    double* sky_arr = NULL;
//    double sum_sky = 0.0;
//    MifImgInfo* img_info_sky = new MifImgInfo;
//    img_info_sky->InitSetImg(1, 1, nskys, nskyz);
//    int bitpix_sky = 0;
//    MifFits::InFitsImageD(argval->GetSkyFile(), img_info_sky,
//                          &bitpix_sky, &sky_arr);
//    sum_sky = MirMath::GetSum(nsky, sky_arr);
//    MiIolib::Printf2(fp_log, "sum_sky = %e\n", sum_sky);


    double* sky_arr = new double[nsky];
    for(int iskys = 0; iskys < nskys; iskys ++){
        for(int iskyz = 0; iskyz < nskyz; iskyz ++){
            int isky = iskyz + nskyz * iskys;
            sky_arr[isky] = 1.0;
        }
    }

    

    
    // load response file
    int naxis0 = MifFits::GetAxisSize(argval->GetRespFile(), 0);
    int naxis1 = MifFits::GetAxisSize(argval->GetRespFile(), 1);
    if ((naxis0 != ndet) || (naxis1 != nsky)){
        MiIolib::Printf2(fp_log, "Error: response file size error.\n");
        abort();
    }
    double* resp_mat_arr = NULL;
    int bitpix_resp = 0;
    MifImgInfo* img_info_resp = new MifImgInfo;
    img_info_resp->InitSetImg(1, 1, ndet, nsky);
    MifFits::InFitsImageD(argval->GetRespFile(), img_info_resp,
                          &bitpix_resp, &resp_mat_arr);

    // det images
    double* det_arr = new double[ndet];
    Ope::GetDetArr(sky_arr,
                   resp_mat_arr,
                   ndet, nsky,
                   det_arr);
    long naxes[2];
    naxes[0] = ndetx;
    naxes[1] = ndety;
    int bitpix_out = bitpix_resp;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "conv", 2,
                           bitpix_out,
                           naxes, det_arr);

    double time_ed = MiTime::GetTimeSec();
    MiIolib::Printf2(fp_log, "duration = %e sec.\n", time_ed - time_st);

    fclose(fp_log);
    
    return status_prog;
}
