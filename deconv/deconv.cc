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

    // read point src dat
    string* line_arr = NULL;
    long nline = 0;
    MiIolib::GenReadFileSkipComment(argval->GetPointSrcDat(),
                                    &line_arr,
                                    &nline);
    int nsrc = nline;

    // load skyz vs lambda data
    nline = 0;
    MiIolib::GenReadFileSkipComment(argval->GetSkyzLambdaFile(),
                                    &line_arr,
                                    &nline);
    double* lambda_arr = new double[nline];
    for(int iline = 0; iline < nline; iline ++){
        int nsplit = 0;
        string* split_arr = NULL;
        MiStr::GenSplit(line_arr[iline], &nsplit, &split_arr);
        lambda_arr[iline] = atof(split_arr[2].c_str());
        delete [] split_arr;
    }
    MiIolib::DelReadFile(line_arr);
    int nskyz = nline;    

    int nskys = nsrc;
    int ndetx = MifFits::GetAxisSize(argval->GetDataFile(), 0);
    int ndety = MifFits::GetAxisSize(argval->GetDataFile(), 1);
    int nsky = nskyz * nskys;
    int ndet = ndetx * ndety;

    printf("nskyz = %d\n", nskyz);
    printf("nskys = %d\n", nskys);
    printf("ndetx = %d\n", ndetx);
    printf("ndety = %d\n", ndety);
    
    // load data
    MifImgInfo* img_info_data = new MifImgInfo;
    img_info_data->InitSetImg(1, 1, ndetx, ndety);
    int bitpix_data = 0;
    double* data_arr = NULL;
    MifFits::InFitsImageD(argval->GetDataFile(), img_info_data,
                          &bitpix_data, &data_arr);
    int nph_data = MirMath::GetSum(ndet, data_arr);
    MiIolib::Printf2(fp_log, "N photon = %d\n", nph_data);

    // fill zero for negative data
    for(int idet = 0; idet < ndet; idet ++){
        if(data_arr[idet] < 0.0){
            data_arr[idet] = 0.0;
        }
    }

    // adu --> electron
    for(int idet = 0; idet < ndet; idet ++){
        data_arr[idet] *= argval->GetRatioEleToAdu();
    }

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
    printf("naxis0(resp) = %d\n", naxis0);
    printf("naxis1(resp) = %d\n", naxis1);
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

    // load efficiency file
    double* eff_mat_arr = NULL;
    int bitpix_eff = 0;
    MifImgInfo* img_info_eff = new MifImgInfo;
    img_info_eff->InitSetImg(1, 1, nsky, 1);
    MifFits::InFitsImageD(argval->GetEffFile(), img_info_eff,
                          &bitpix_eff, &eff_mat_arr);

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

    // div by ratio_ele_to_adu
    for(int isky = 0; isky < nsky; isky ++){
        sky_new_arr[isky] /= argval->GetRatioEleToAdu();
    }
    double sum_sky_new = MirMath::GetSum(nsky, sky_new_arr);
    MiIolib::Printf2(fp_log, "sum_sky_new = %e\n", sum_sky_new);

    // div by eff_arr
    for(int isky = 0; isky < nsky; isky ++){
        sky_new_arr[isky] /= eff_mat_arr[isky];
    }

    long naxes[2];
    naxes[0] = nskyz;
    naxes[1] = nskys;
    int bitpix_out = -32;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "rec", 2,
                           bitpix_out,
                           naxes, sky_new_arr);

    
    // output spectrum to qdp
    char qdpout[kLineSize];
    sprintf(qdpout, "%s/spec_%s.qdp", argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    FILE* fp_qdp = fopen(qdpout, "w");
    fprintf(fp_qdp, "skip sing \n");
    fprintf(fp_qdp, "\n");
    for(int iskys = 0; iskys < nskys; iskys ++){
        for(int iskyz = 0; iskyz < nskyz; iskyz ++){    
            int isky = iskyz + nskyz * iskys;
            fprintf(fp_qdp, "%e  %e  !  %d\n", lambda_arr[iskyz], sky_new_arr[isky], iskyz);
        }
        fprintf(fp_qdp, "\n");
        fprintf(fp_qdp, "no\n");
        fprintf(fp_qdp, "\n");
    }
    fprintf(fp_qdp, "\n");

    fprintf(fp_qdp, "time off\n");
    fprintf(fp_qdp, "la file\n");
    fprintf(fp_qdp, "lw 5\n");
    fprintf(fp_qdp, "p v\n");
    fprintf(fp_qdp, "la rot\n");
    fprintf(fp_qdp, "win 1\n");
    fprintf(fp_qdp, "la y Jy\n");
    fprintf(fp_qdp, "win 2\n");
    fprintf(fp_qdp, "la y Jy\n");
    fprintf(fp_qdp, "la x lambda (um)\n");
    fclose(fp_qdp);
    delete [] lambda_arr;

    // convolve for check
    // multiply by eff_arr
    for(int isky = 0; isky < nsky; isky ++){
        sky_new_arr[isky] *= eff_mat_arr[isky];
    }
    // convolved image
    double* det_arr = new double[ndet];
    SrtlibRl::GetDetArr(sky_new_arr,
                        resp_mat_arr,
                        ndet, nsky,
                        det_arr);
    
    naxes[0] = ndetx;
    naxes[1] = ndety;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "conv", 2,
                           bitpix_out,
                           naxes, det_arr);
    
    double time_ed = MiTime::GetTimeSec();
    MiIolib::Printf2(fp_log,
                     "duration = %e sec.\n", time_ed - time_st);

    fclose(fp_log);
    
    return status_prog;
}
