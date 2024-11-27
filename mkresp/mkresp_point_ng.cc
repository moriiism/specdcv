#include "mir_math.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "arg_mkresp_point_ng.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    ArgValMkrespPointNg* argval = new ArgValMkrespPointNg;
    argval->Init(argc, argv);
    argval->Print(stdout);

    char logfile[kLineSize];
    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
        char cmd[kLineSize];
        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
        system(cmd);
    }
    sprintf(logfile, "%s/%s_%s.log",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str(),
            argval->GetProgname().c_str());
    FILE* fp_log = fopen(logfile, "w");
    MiIolib::Printf2(fp_log, "-----------------------------\n");
    argval->Print(fp_log);

    int naxis_psf = MifFits::GetNaxis(argval->GetImgPsf());
    int naxis_spec = MifFits::GetNaxis(argval->GetImgSpec());
    MiIolib::Printf2(fp_log, "naxis_psf = %d\n", naxis_psf);
    MiIolib::Printf2(fp_log, "naxis_spec = %d\n", naxis_spec);
    int npixx_psf = MifFits::GetAxisSize(argval->GetImgPsf(), 0);
    int npixy_psf = MifFits::GetAxisSize(argval->GetImgPsf(), 1);
    int npixx_spec = MifFits::GetAxisSize(argval->GetImgSpec(), 0);
    int npixy_spec = MifFits::GetAxisSize(argval->GetImgSpec(), 1);
    MiIolib::Printf2(fp_log, "psf image size: %d, %d\n", npixx_psf, npixy_psf);
    MiIolib::Printf2(fp_log, "spec image size: %d, %d\n", npixx_spec, npixy_spec);


    // get (posx_psf, posy_psf) with maximal pixel value
    MifImgInfo* img_info_psf = new MifImgInfo;
    img_info_psf->InitSetImg(1, 1, npixx_psf, npixy_psf);
    double* img_psf_arr = NULL;
    int bitpix_psf = 0;
    MifFits::InFitsImageD(argval->GetImgPsf(), img_info_psf,
			  &bitpix_psf, &img_psf_arr);
    int npix_psf = npixx_psf * npixy_psf;

    // force to be non-negative
    int npix_psf_force_to_be_non_negative = 0;
    for (int ipix = 0; ipix < npix_psf; ipix ++){
        if(img_psf_arr[ipix] < 0.0){
            img_psf_arr[ipix] = 0.0;
            npix_psf_force_to_be_non_negative ++;
        }
    }
    MiIolib::Printf2(fp_log, "npix_psf_force_to_be_non_negative = %d\n",
                     npix_psf_force_to_be_non_negative);

    // div by sum of pixel values
    double sum_img_psf = MirMath::GetSum(npix_psf, img_psf_arr);
    for (int ipix = 0; ipix < npix_psf; ipix ++){
        img_psf_arr[ipix] /= sum_img_psf;
    }
    
    int index_max = MirMath::GetLocMax(npix_psf, img_psf_arr);
    MiIolib::Printf2(fp_log, "index_max = %d\n", index_max);
    int posx_psf = index_max % npixx_psf + 1;
    int posy_psf = index_max / npixx_psf + 1;
    MiIolib::Printf2(fp_log, "(posx_psf, posy_psf) with maximal pixel value: %d, %d\n",
                     posx_psf, posy_psf);
    MiIolib::Printf2(fp_log, "where, posx_psf is in [1, npixx_psf], "
                     "posy_psf is in [1, npixy_psf].\n");

   
    // get 2-d image of detector
    MifImgInfo* img_info_spec = new MifImgInfo;
    img_info_spec->InitSetImg(1, 1, npixx_spec, npixy_spec);
    double* img_spec_arr = NULL;
    int bitpix_spec = 0;
    MifFits::InFitsImageD(argval->GetImgSpec(), img_info_spec,
			  &bitpix_spec, &img_spec_arr);


    // read point src dat
    string* lines_arr = NULL;
    long nline = 0;
    MiIolib::GenReadFileSkipComment(argval->GetPointSrcDat(),
                                    &lines_arr,
                                    &nline);
    int nsrc = nline;
    double* posx_src_arr = new double[nsrc];
    double* posy_src_arr = new double[nsrc];
    for(long iline = 0; iline < nline; iline ++){
        int nsplit = 0;
        string* split_arr = NULL;
        MiStr::GenSplit(lines_arr[iline], &nsplit, &split_arr);
        posx_src_arr[iline] = atof(split_arr[0].c_str());
        posy_src_arr[iline] = atof(split_arr[1].c_str());
        MiStr::DelSplit(split_arr);
    }
    MiIolib::DelReadFile(lines_arr);


    printf("%e %e\n", posx_src_arr[0], posy_src_arr[0]);
    printf("%e %e\n", posx_src_arr[1], posy_src_arr[1]);

    
    // read spec_resp_dat
    MiIolib::GenReadFileSkipComment(argval->GetSpecRespDat(),
                                    &lines_arr,
                                    &nline);
    int nskyz = nline - 1;
    double* lambda_arr = new double[nskyz];
    double* sensi_arr = new double[nskyz];
    double* sensi_err_arr = new double[nskyz];
    int* delta_pix_arr = new int[nskyz];
    for(long iline = 1; iline < nline; iline ++){
        int nsplit = 0;
        string* split_arr = NULL;
        MiStr::GenSplit(lines_arr[iline], &nsplit, &split_arr);
        lambda_arr[iline - 1] = atof(split_arr[0].c_str());
        sensi_arr[iline - 1] = atof(split_arr[1].c_str());
        sensi_err_arr[iline - 1] = atof(split_arr[2].c_str());
        MiStr::DelSplit(split_arr);
    }
    MiIolib::DelReadFile(lines_arr);

    // find 3.00656 um
    int iskyz_zero = 0;
    for(int iskyz = 0; iskyz < nskyz; iskyz++){
        if( fabs(lambda_arr[iskyz] - 3.00656) < 1.0e-10){
            iskyz_zero = iskyz;
        }
    }
    printf("iskyz_zero = %d\n", iskyz_zero);

    // set delta_pix_arr
    double plus_minus_for_delta_pix = 0.0;
    if("plus" == argval->GetPlusMinusForDeltaPix()){
        plus_minus_for_delta_pix = 1.0;
    } else if("minus" == argval->GetPlusMinusForDeltaPix()){
        plus_minus_for_delta_pix = -1.0;
    } else {
        abort();
    }
    for(int iskyz = 0; iskyz < nskyz; iskyz++){
        delta_pix_arr[iskyz] = plus_minus_for_delta_pix * (iskyz - iskyz_zero);
    }
    
    for(int iskyz = 0; iskyz < nskyz; iskyz++){
        printf("%d %e %e %e\n", delta_pix_arr[iskyz], lambda_arr[iskyz],
               sensi_arr[iskyz], sensi_err_arr[iskyz]);
    }
    
    int nskys = nsrc;
    int nsky = nskys * nskyz;
    int ndetx = npixx_spec;
    int ndety = npixy_spec;
    int ndet = ndetx * ndety;

    MiIolib::Printf2(fp_log, "nskys = %d\n", nskys);
    MiIolib::Printf2(fp_log, "nskyz = %d\n", nskyz);
    MiIolib::Printf2(fp_log, "nsky  = %d\n", nsky);
    MiIolib::Printf2(fp_log, "ndetx = %d\n", ndetx);
    MiIolib::Printf2(fp_log, "ndety = %d\n", ndety);
    MiIolib::Printf2(fp_log, "ndet  = %d\n", ndet);

    // for efficiency map
    double* img_eff_arr = new double [nsky];
    for(int isky = 0; isky < nsky; isky ++){
        img_eff_arr[isky] = 0.0;
    }

    // for resonse matrix
    int nresp = ndet * nsky;
    double* resp_norm_arr = new double [nresp];
    for(int iresp = 0; iresp < nresp; iresp ++){
        resp_norm_arr[iresp] = 0.0;
    }
    MiIolib::Printf2(fp_log, "nresp  = %d\n", nresp);

    // (pos0x_psf, pos0y_psf): by index starting from zero
    int pos0x_psf = posx_psf - 1;
    int pos0y_psf = posy_psf - 1;
    for(int iskys = 0; iskys < nskys; iskys ++){
        int posx_src = posx_src_arr[iskys];
        int posy_src = posy_src_arr[iskys];
        for(int iskyz = 0; iskyz < nskyz; iskyz ++){
            // If image shift toward y-direction by grism is zero,
            // indices must satisfy the followings:
            //   ipsfx - pos0x_psf = idetx - posx_src
            //   ipsfy - pos0y_psf = idety - posy_src
            // Therefore, if the effect of image shift toward y-direction by grism
            // is considered, indices becomes the followings:
            //   ipsfx = pos0x_psf + idetx - posx_src
            //   ipsfy = pos0y_psf + idety - posy_src - delta_pix_arr[iskyz]
            double* img_det_arr = new double[ndet];
            for(int idet = 0; idet < ndet; idet ++){
                img_det_arr[idet] = 1.0e-20;
            }
            for(int idety = 0; idety < ndety; idety ++){
                int ipsfy = pos0y_psf + idety - posy_src - delta_pix_arr[iskyz];
                if(ipsfy < 0 || npixy_psf - 1 < ipsfy){
                    continue;
                }
                for(int idetx = 0; idetx < ndetx; idetx ++){
                    int ipsfx = pos0x_psf + idetx - posx_src;
                    if(ipsfx < 0 || npixx_psf - 1 < ipsfx){
                        continue;
                    }
                    int idet = idetx + idety * ndetx;
                    int ipsf = ipsfx + ipsfy * npixx_psf;
                    img_det_arr[idet] += img_psf_arr[ipsf] * sensi_arr[iskyz];
                    // printf("idet, ipsf = %d, %d\n", idet, ipsf);
                }
            }

            // normalize det image
            double sum_img_det = MirMath::GetSum(ndet, img_det_arr);
            MiIolib::Printf2(fp_log, "iskyz = %d, iskys = %d: sum_img_det = %e\n",
                             iskyz, iskys, sum_img_det);
            for (int idet = 0; idet < ndet; idet ++){
                img_det_arr[idet] /= sum_img_det;
            }

            // fill to response
            // ndet x nsky matrix
            // column wise matrix
            int isky = iskyz + nskyz * iskys;
            // fill to efficiency
            img_eff_arr[isky] = sum_img_det;
            
            int imat = isky * ndet;
            for(int idet = 0; idet < ndet; idet ++){
                resp_norm_arr[imat + idet] = img_det_arr[idet];
            }
            
            char tag[kLineSize];
            sprintf(tag, "gimage_%3.3d_%3.3d", iskys, iskyz);
            printf("%s\n", tag);
            int naxis_gimage = 2;
            long* naxes_gimage = new long[naxis_gimage];
            naxes_gimage[0] = ndetx;
            naxes_gimage[1] = ndety;
            MifFits::OutFitsImageD(argval->GetOutdir(),
                                   argval->GetOutfileHead(),
                                   tag, naxis_gimage, bitpix_spec,
                                   naxes_gimage, img_det_arr);
            delete [] naxes_gimage;
            delete [] img_det_arr;
        }
    }

    // efficiency 
    int naxis_eff = 2;
    long* naxes_eff = new long[naxis_eff];
    naxes_eff[0] = nskys;
    naxes_eff[1] = nskyz;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "eff", naxis_eff, bitpix_spec,
                           naxes_eff, img_eff_arr);
    delete [] naxes_eff;
    delete [] img_eff_arr;

    // output response
    int naxis_resp = 2;
    long* naxes_resp = new long[naxis_resp];
    naxes_resp[0] = ndet;
    naxes_resp[1] = nsky;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "resp", naxis_resp, bitpix_spec,
                           naxes_resp, resp_norm_arr);
    delete [] naxes_resp;
    delete [] resp_norm_arr;
    fclose(fp_log);

    return status_prog;
}
