#include "mir_math.h"
#include "mi_time.h"
#include "arg_trim.h"
#include "mir_qdp_tool.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "TRandom3.h"
// simulate hsc image subtraction and
// distribution of pixel values

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValTrim* argval = new ArgValTrim;
    argval->Init(argc, argv);
    argval->Print(stdout);

    int nskyx = MifFits::GetAxisSize(argval->GetInImg(), 0);
    int nskyy = MifFits::GetAxisSize(argval->GetInImg(), 1);
    int nskyz = MifFits::GetAxisSize(argval->GetInImg(), 2);
    
    printf("nskyx, nskyy, nskyz = %d, %d, %d\n", nskyx, nskyy, nskyz);

    int iskyz = argval->GetIskyz();
    // load in_img
    double* img_arr = NULL;
    MifImgInfo* img_info = new MifImgInfo;
    img_info->InitSetCube(1, 1, iskyz, nskyx, nskyy, iskyz);
    int bitpix_img = 0;
    MifFits::InFitsCubeD(argval->GetInImg(),
                         img_info,
                         &bitpix_img, &img_arr);
    //MifFits::InFitsImageD(argval->GetInImg(), img_info,
    //                      &bitpix_img, &img_arr);
    delete img_info;

    int nsky = nskyx * nskyy;
    HistDataNerr2d* hd2d_img = new HistDataNerr2d;
    hd2d_img->Init(nskyx, 0, nskyx,
                   nskyy, 0, nskyy);
    hd2d_img->SetOvalArr(nsky, img_arr);

    HistData2d* hd2d_sub_img = hd2d_img->GenSubHist(
        argval->GetXLo(), argval->GetXUp(),
        argval->GetYLo(), argval->GetYUp());

    // get peak
    printf("peak position in original image: (x, y) = (%e, %e)\n",
           hd2d_sub_img->GetXvalAtOvalMax(),
           hd2d_sub_img->GetYvalAtOvalMax());
    printf("peak position in trimed image: (x, y) = (%e, %e)\n",
           hd2d_sub_img->GetXvalAtOvalMax() - argval->GetXLo(),
           hd2d_sub_img->GetYvalAtOvalMax() - argval->GetYLo());
    
    char outfile[kLineSize];
    sprintf(outfile, "%.100s/%.100s_trim.fits",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    printf("outfile=%s\n", outfile);
    long naxes[2];
    naxes[0] = argval->GetXUp() - argval->GetXLo() + 1;
    naxes[1] = argval->GetYUp() - argval->GetYLo() + 1;
    MifFits::OutFitsImageD(outfile,
                           2, bitpix_img,
                           naxes, hd2d_sub_img->GetOvalArr()->GetVal());

    
    
    return status_prog;
}


