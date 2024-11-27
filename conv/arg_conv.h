#ifndef MORIIISM_SPECDCV_CONV_ARG_CONV_H_
#define MORIIISM_SPECDCV_CONV_ARG_CONV_H_

#include "mi_base.h"

class ArgValConv : public MiArgBase{
public:
    ArgValConv() :
        MiArgBase(),
        progname_(""),
        sky_file_(""),
        resp_file_(""),
        nskys_(0),
        nskyz_(0),
        ndetx_(0),
        ndety_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValConv(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetSkyFile() const {return sky_file_;};
    string GetRespFile() const {return resp_file_;};
    int GetNskys() const {return nskys_;};
    int GetNskyz() const {return nskyz_;};
    int GetNdetx() const {return ndetx_;};
    int GetNdety() const {return ndety_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string sky_file_;
    string resp_file_;
    int nskys_;
    int nskyz_;
    int ndetx_;
    int ndety_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SPECDCV_CONV_ARG_CONV_H_

