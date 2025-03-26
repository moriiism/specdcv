#ifndef MORIIISM_DECONVAKARI_ARG_TRIM_H_
#define MORIIISM_DECONVAKARI_ARG_TRIM_H_

#include "mi_base.h"

class ArgValTrim : public MiArgBase{
public:
    ArgValTrim() :
        MiArgBase(),
        progname_(""),
        in_img_(""),
        iskyz_(0),
        x_lo_(0),
        x_up_(0),
        y_lo_(0),
        y_up_(0),
        outdir_(""),
        outfile_head_(""){}
    ~ArgValTrim(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInImg() const {return in_img_;};
    int GetIskyz() const {return iskyz_;};
    int GetXLo() const {return x_lo_;};
    int GetXUp() const {return x_up_;};
    int GetYLo() const {return y_lo_;};
    int GetYUp() const {return y_up_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_img_;
    int iskyz_;
    int x_lo_;
    int x_up_;
    int y_lo_;
    int y_up_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_DECONVAKARI_ARG_TRIM_H_
