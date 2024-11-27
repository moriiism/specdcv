#include "mir_math.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "arg_mkresp_np.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    ArgValMkrespNp* argval = new ArgValMkrespNp;
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

    
    // read spec_resp_dat
    string* lines_arr = NULL;
    long nline = 0;
    MiIolib::GenReadFileSkipComment(argval->GetSpecRespDat(),
                                    &lines_arr,
                                    &nline);
    int nskyz = nline;
    int* delta_pix_arr = new int[nskyz];
    double* lambda_arr = new double[nskyz];
    double* sensi_arr = new double[nskyz];
    double* sensi_err_arr = new double[nskyz];
    for(long iline = 0; iline < nline; iline ++){
        int nsplit = 0;
        string* split_arr = NULL;
        MiStr::GenSplit(lines_arr[iline], &nsplit, &split_arr);
        delta_pix_arr[iline] = atof(split_arr[0].c_str());
        lambda_arr[iline] = atof(split_arr[1].c_str());
        sensi_arr[iline] = atof(split_arr[2].c_str());
        sensi_err_arr[iline] = atof(split_arr[3].c_str());
        MiStr::DelSplit(split_arr);
    }
    MiIolib::DelReadFile(lines_arr);

    for(int iskyz = 0; iskyz < nskyz; iskyz++){
        printf("%d %e %e %e\n", delta_pix_arr[iskyz], lambda_arr[iskyz],
               sensi_arr[iskyz], sensi_err_arr[iskyz]);
    }


    int nskyx = npixx_spec;
    int nskyy = npixy_spec;
    int nsky = nskyx * nskyy * nskyz;
    int ndetx = npixx_spec;
    int ndety = npixy_spec;
    int ndet = ndetx * ndety;

    MiIolib::Printf2(fp_log, "nskyx = %d\n", nskyx);
    MiIolib::Printf2(fp_log, "nskyy = %d\n", nskyy);
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

    printf("jjjj\n");
    
    // for resonse matrix
    int nresp = ndet * nsky;
    double* resp_norm_arr = new double [nresp];
    for(int iresp = 0; iresp < nresp; iresp ++){
        resp_norm_arr[iresp] = 0.0;
    }
    MiIolib::Printf2(fp_log, "nresp  = %d\n", nresp);

    printf("kkkkk\n");

    
    // (pos0x_psf, pos0y_psf): by index starting from zero
    int pos0x_psf = posx_psf - 1;
    int pos0y_psf = posy_psf - 1;
    for(int iskyz = 0; iskyz < nskyz; iskyz ++){    
        for(int iskyy = 0; iskyy < nskyy; iskyy ++){
            for(int iskyx = 0; iskyx < nskyx; iskyx ++){
                // If image shift toward y-direction by prism is zero,
                // indices must satisfy the followings:
                //   ipsfx - pos0x_psf = idetx - iskyx
                //   ipsfy - pos0y_psf = idety - iskyy
                // Therefore, if the effect of image shift toward y-direction by prism
                // is considered, indices becomes the followings:
                //   ipsfx = pos0x_psf + idetx - iskyx
                //   ipsfy = pos0y_psf + idety - iskyy - iskyz
                double* img_det_arr = new double[ndet];
                for(int idet = 0; idet < ndet; idet ++){
                    img_det_arr[idet] = 0.0;
                }
                for(int idety = 0; idety < ndety; idety ++){
                    int ipsfy = pos0y_psf + idety - iskyy - delta_pix_arr[iskyz];
                    if(ipsfy < 0 || npixy_psf - 1 < ipsfy){
                        continue;
                    }
                    for(int idetx = 0; idetx < ndetx; idetx ++){
                        int ipsfx = pos0x_psf + idetx - iskyx;
                        if(ipsfx < 0 || npixx_psf - 1 < ipsfx){
                            continue;
                        }
                        int idet = idetx + idety * ndetx;
                        int ipsf = ipsfx + ipsfy * npixx_psf;
                        img_det_arr[idet] = img_psf_arr[ipsf] * sensi_arr[iskyz];
                        // printf("idet, ipsf = %d, %d\n", idet, ipsf);
                    }
                }

                // normalize det image
                double sum_img_det = MirMath::GetSum(ndet, img_det_arr);
                MiIolib::Printf2(fp_log, "(iskyx, iskyy) = (%d, %d): sum_img_det = %e\n",
                                 iskyx, iskyy, sum_img_det);
                for (int idet = 0; idet < ndet; idet ++){
                    img_det_arr[idet] /= sum_img_det;
                }

                // fill to response
                // ndet x nsky matrix
                // column wise matrix
                int isky = nskyx * nskyy * iskyz + nskyx * iskyy + iskyx;
                // fill to efficiency
                img_eff_arr[isky] = sum_img_det;
            
                int imat = isky * ndet;
                for(int idet = 0; idet < ndet; idet ++){
                    resp_norm_arr[imat + idet] = img_det_arr[idet];
                }
            
                //char tag[kLineSize];
                //sprintf(tag, "gimage_%3.3d_%3.3d_%3.3d", iskyx, iskyy, iskyz);
                //printf("%s\n", tag);
                //int naxis_gimage = 2;
                //long* naxes_gimage = new long[naxis_gimage];
                //naxes_gimage[0] = ndetx;
                //naxes_gimage[1] = ndety;
                //MifFits::OutFitsImageD(argval->GetOutdir(),
                //                       argval->GetOutfileHead(),
                //                       tag, naxis_gimage, bitpix_spec,
                //                       naxes_gimage, img_det_arr);
                delete [] img_det_arr;
            }
        }
    }

    // efficiency 
    int naxis_eff = 2;
    long* naxes_eff = new long[naxis_eff];
    naxes_eff[0] = nskyx;
    naxes_eff[1] = nskyy;
    MifFits::OutFitsImageD(argval->GetOutdir(),
                           argval->GetOutfileHead(),
                           "eff", naxis_eff, bitpix_spec,
                           naxes_eff, img_eff_arr);
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
    delete [] resp_norm_arr;

    fclose(fp_log);

    return status_prog;
}
