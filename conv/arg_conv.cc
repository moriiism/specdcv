#include "arg_conv.h"

// public

void ArgValConv::Init(int argc, char* argv[])
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
    int narg = 8;
    if (argc - optind != narg){
        printf("# of arguments must be %d.\n", narg);
        Usage(stdout);
    }
    int iarg = optind;
    sky_file_       = argv[iarg]; iarg++;
    resp_file_      = argv[iarg]; iarg++;
    nskyz_          = atoi(argv[iarg]); iarg++;    
    nskys_          = atoi(argv[iarg]); iarg++;
    ndetx_          = atoi(argv[iarg]); iarg++;
    ndety_          = atoi(argv[iarg]); iarg++;    
    outdir_         = argv[iarg]; iarg++;
    outfile_head_   = argv[iarg]; iarg++;
}

void ArgValConv::Print(FILE* fp) const
{
    fprintf(fp, "%s: g_flag_debug   : %d\n", __func__, g_flag_debug);
    fprintf(fp, "%s: g_flag_help    : %d\n", __func__, g_flag_help);
    fprintf(fp, "%s: g_flag_verbose : %d\n", __func__, g_flag_verbose);

    fprintf(fp, "%s: progname_       : %s\n", __func__, progname_.c_str());
    fprintf(fp, "%s: sky_file_       : %s\n", __func__, sky_file_.c_str());
    fprintf(fp, "%s: resp_file_      : %s\n", __func__, resp_file_.c_str());
    fprintf(fp, "%s: nskyz_          : %d\n", __func__, nskyz_);    
    fprintf(fp, "%s: nskys_          : %d\n", __func__, nskys_);
    fprintf(fp, "%s: ndetx_          : %d\n", __func__, ndetx_);
    fprintf(fp, "%s: ndety_          : %d\n", __func__, ndety_);    
    fprintf(fp, "%s: outdir_         : %s\n", __func__, outdir_.c_str());
    fprintf(fp, "%s: outfile_head_   : %s\n", __func__, outfile_head_.c_str());
}

// private

void ArgValConv::Null()
{
    progname_ = "";
    sky_file_ = "";
    resp_file_ = "";
    nskyz_ = 0;    
    nskys_ = 0;
    ndetx_ = 0;
    ndety_ = 0;
    outdir_    = "";
    outfile_head_ = "";
}

void ArgValConv::SetOption(int argc, char* argv[], option* long_options)
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


void ArgValConv::Usage(FILE* fp) const
{
    fprintf(fp,
            "usage: %s [--help (0)] [--verbose (0)] [--debug (0)] "
            "sky_file  resp_file  skyz  nskys  ndetx  ndety  "
            "outdir  outfile_head \n",
            progname_.c_str());
    abort();
}
