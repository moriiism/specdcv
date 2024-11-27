#ifndef SPECDCV_MKRESP_ARG_MKRESP_POINT_NG_H_
#define SPECDCV_MKRESP_ARG_MKRESP_POINT_NG_H_

#include "mi_base.h"

class ArgValMkrespPointNg : public MiArgBase{
public:
    ArgValMkrespPointNg() :
        MiArgBase(),
        progname_(""),
        img_spec_(""),
        img_ref_(""),
        img_psf_(""),
        spec_resp_dat_(""),
        point_src_dat_(""),
        plus_minus_for_delta_pix_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValMkrespPointNg(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetImgSpec() const {return img_spec_;};
    string GetImgRef() const {return img_ref_;};
    string GetImgPsf() const {return img_psf_;};
    string GetSpecRespDat() const {return spec_resp_dat_;};
    string GetPointSrcDat() const {return point_src_dat_;};
    string GetPlusMinusForDeltaPix() const {return plus_minus_for_delta_pix_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string img_spec_;
    string img_ref_;
    string img_psf_;
    string spec_resp_dat_;
    string point_src_dat_;
    string plus_minus_for_delta_pix_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // SPECDCV_MKRESP_ARG_MKRESP_POINT_NG_H_
