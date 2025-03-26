#include "arg_deconv.h"

// public

void ArgValDeconv::Init(int argc, char* argv[])
{
    progname_ = "deconv";

    option long_options[] = {
        {"debug",       required_argument, NULL, 'd'},
        {"help",        required_argument, NULL, 'h'},
        {"verbose",     required_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    // long option default

    SetOption(argc, argv, long_options);
    
    printf("ArgVal::Init: # of arg = %d\n", argc - optind);
    int narg = 11;
    if (argc - optind != narg){
        printf("# of arguments must be %d.\n", narg);
        Usage(stdout);
    }
    int iarg = optind;
    data_file_      = argv[iarg]; iarg++;
    point_src_dat_  = argv[iarg]; iarg++;
    resp_file_      = argv[iarg]; iarg++;
    eff_file_       = argv[iarg]; iarg++;
    skyz_lambda_file_ = argv[iarg]; iarg++;
    ratio_ele_to_adu_ = atof(argv[iarg]); iarg++;
    nem_            = atoi(argv[iarg]); iarg++;
    tol_em_         = atof(argv[iarg]); iarg++;
    acc_method_     = argv[iarg]; iarg++;
    outdir_         = argv[iarg]; iarg++;
    outfile_head_   = argv[iarg]; iarg++;
}

void ArgValDeconv::Print(FILE* fp) const
{
    fprintf(fp, "%s: g_flag_debug   : %d\n", __func__, g_flag_debug);
    fprintf(fp, "%s: g_flag_help    : %d\n", __func__, g_flag_help);
    fprintf(fp, "%s: g_flag_verbose : %d\n", __func__, g_flag_verbose);

    fprintf(fp, "%s: progname_       : %s\n", __func__, progname_.c_str());
    fprintf(fp, "%s: data_file_      : %s\n", __func__, data_file_.c_str());
    fprintf(fp, "%s: point_src_dat_  : %s\n", __func__, point_src_dat_.c_str());
    fprintf(fp, "%s: resp_file_      : %s\n", __func__, resp_file_.c_str());
    fprintf(fp, "%s: eff_file_       : %s\n", __func__, eff_file_.c_str());
    fprintf(fp, "%s: skyz_lambda_file_ : %s\n", __func__, skyz_lambda_file_.c_str());
    fprintf(fp, "%s: ratio_ele_to_adu_ : %e\n", __func__, ratio_ele_to_adu_);
    fprintf(fp, "%s: nem_            : %d\n", __func__, nem_);
    fprintf(fp, "%s: tol_em_         : %e\n", __func__, tol_em_);
    fprintf(fp, "%s: acc_method_     : %s\n", __func__, acc_method_.c_str());
    fprintf(fp, "%s: outdir_         : %s\n", __func__, outdir_.c_str());
    fprintf(fp, "%s: outfile_head_   : %s\n", __func__, outfile_head_.c_str());
}

// private

void ArgValDeconv::Null()
{
    progname_ = "";
    data_file_ = "";
    point_src_dat_ = "";
    resp_file_ = "";
    eff_file_ = "";
    skyz_lambda_file_ = "";
    ratio_ele_to_adu_ = 0.0;
    nem_ = 0;
    tol_em_ = 0.0;
    acc_method_ = "";
    outdir_    = "";
    outfile_head_ = "";
}

void ArgValDeconv::SetOption(int argc, char* argv[], option* long_options)
{
    if(0 < g_flag_verbose){
        MPrintInfo("start...");
    }
    // option default
    g_flag_debug   = 0;
    g_flag_help    = 0;
    g_flag_verbose = 0;
    while (1) {
        int option_index = 0;
        int retopt = getopt_long(argc, argv, "dhv",
                                 long_options, &option_index);
        if(-1 == retopt)
            break;
        switch (retopt) {
        case 0:
            // long option
            break;
        case 'd':
            g_flag_debug = atoi(optarg);
            printf("%s: g_flag_debug = %d\n", __func__, g_flag_debug);
            break;
        case 'h':
            g_flag_help = atoi(optarg);
            printf("%s: g_flag_help = %d\n", __func__, g_flag_help);
            if(0 != g_flag_help){
                Usage(stdout);
            }
            break;
        case 'v':
            g_flag_verbose = atoi(optarg);
            printf("%s: g_flag_verbose = %d\n", __func__, g_flag_verbose);
            break;
        case '?':
            printf("%s: retopt (= %c) is invalid flag.\n",
                   __func__, retopt);
            Usage(stdout);
            break;
        default:
            printf("%s: error: getopt returned character code 0%o ??\n", __func__, retopt);
            abort();
        }
    }
    if(0 < g_flag_verbose){
        MPrintInfo("done.");
    }
}


void ArgValDeconv::Usage(FILE* fp) const
{
    fprintf(fp,
            "usage: %s [--help (0)] [--verbose (0)] [--debug (0)] "
            "data_file  point_src_dat  resp_file  eff_file  skyz_lambda_file  ratio_ele_to_adu  "
            "nem  tol_em  acc_method  "
            "outdir  outfile_head \n",
            progname_.c_str());
    abort();
}
