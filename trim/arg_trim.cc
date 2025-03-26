#include "arg_trim.h"

// public

void ArgValTrim::Init(int argc, char* argv[])
{
    progname_ = "trim";

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
    in_img_        = argv[iarg]; iarg++;
    iskyz_         = atoi(argv[iarg]); iarg++;
    x_lo_          = atoi(argv[iarg]); iarg++;
    x_up_          = atoi(argv[iarg]); iarg++;
    y_lo_          = atoi(argv[iarg]); iarg++;
    y_up_          = atoi(argv[iarg]); iarg++;
    outdir_        = argv[iarg]; iarg++;
    outfile_head_  = argv[iarg]; iarg++;
}

void ArgValTrim::Print(FILE* fp) const
{
    fprintf(fp, "%s: g_flag_debug   : %d\n", __func__, g_flag_debug);
    fprintf(fp, "%s: g_flag_help    : %d\n", __func__, g_flag_help);
    fprintf(fp, "%s: g_flag_verbose : %d\n", __func__, g_flag_verbose);

    fprintf(fp, "%s: progname_      : %s\n",
            __func__, progname_.c_str());
    fprintf(fp, "%s: in_img_        : %s\n",
            __func__, in_img_.c_str());
    fprintf(fp, "%s: iskyz_         : %d\n",
            __func__, iskyz_);
    fprintf(fp, "%s: x_lo_        : %d\n",
            __func__, x_lo_);
    fprintf(fp, "%s: x_up_        : %d\n",
            __func__, x_up_);
    fprintf(fp, "%s: y_lo_        : %d\n",
            __func__, y_lo_);
    fprintf(fp, "%s: y_up_        : %d\n",
            __func__, y_up_);    
    fprintf(fp, "%s: outdir_        : %s\n",
            __func__, outdir_.c_str());
    fprintf(fp, "%s: outfile_head_  : %s\n",
            __func__, outfile_head_.c_str());        
}

// private

void ArgValTrim::Null()
{
    progname_   = "";
    in_img_     = "";
    iskyz_      = 0;
    x_lo_       = 0;
    x_up_       = 0;
    y_lo_       = 0;
    y_up_       = 0;
    outdir_     = "";
    outfile_head_ = "";
}

void ArgValTrim::SetOption(int argc, char* argv[], option* long_options)
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


void ArgValTrim::Usage(FILE* fp) const
{
    fprintf(fp,
            "usage: %s [--help (0)] [--verbose (0)] [--debug (0)] "
            "in_img  iskyz  x_lo  x_up  y_lo  y_up  outdir  outfile_head \n",
            progname_.c_str());
    abort();
}
