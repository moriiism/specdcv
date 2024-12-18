#ifndef MORIIISM_SPECDCV_DECONV_ARG_DECONV_H_
#define MORIIISM_SPECDCV_DECONV_ARG_DECONV_H_

#include "mi_base.h"

class ArgValDeconv : public MiArgBase{
public:
    ArgValDeconv() :
        MiArgBase(),
        progname_(""),
        data_file_(""),
        resp_file_(""),
        nskys_(0),
        nskyz_(0),
        ndetx_(0),
        ndety_(0),
        nem_(0),
        tol_em_(0.0),
        acc_method_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValDeconv(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetDataFile() const {return data_file_;};
    string GetRespFile() const {return resp_file_;};
    int GetNskys() const {return nskys_;};
    int GetNskyz() const {return nskyz_;};
    int GetNdetx() const {return ndetx_;};
    int GetNdety() const {return ndety_;};
    int GetNem() const {return nem_;};
    double GetTolEm() const {return tol_em_;};
    string GetAccMethod() const {return acc_method_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string data_file_;
    string resp_file_;
    int nskys_;
    int nskyz_;
    int ndetx_;
    int ndety_;
    int    nem_;
    double tol_em_;
    string acc_method_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SPECDCV_DECONV_ARG_DECONV_H_


