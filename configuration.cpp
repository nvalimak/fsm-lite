#include "configuration.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib> // std::exit()
#include <cassert>
#include <getopt.h>
using namespace std;

configuration::configuration(int argc, char **argv)
    : prog(argv[0]), listfile(""), tmpfile(""), inputformat(input_reader::input_fasta),
      minlength(9), maxlength(100), minfreq(1), minsupport(2), maxsupport(~0u),
      verbose(false), debug(false), good(false)
{
    assert (argc > 0);
    good = parse(argc, argv);
}

int atoi_min(char const *value, int min, char const *parameter, char const *name)
{
    istringstream iss(value);
    int i;
    char c;
    if (!(iss >> i) || iss.get(c))
    {
        cerr << "error: argument of " << parameter << " must be of type <int>, and greater than or equal to " << min << endl
             << "Check " << name << " --help' for more information." << endl;
        exit(1);
    }

    if (i < min)
    {
        cerr << "error: argument of " << parameter << " must be greater than or equal to " << min << endl
             << "Check `" << name << " --help' for more information." << endl;
        exit(1);
    }
    return i;
}

void configuration::print_short_usage() const
{
    cerr << "See " << prog << " --help for more information." << endl;
    exit(1);
}

void configuration::print_usage() const
{
    cerr << "usage: " << prog << " -l <file> -t <file> [options]" << endl
         << "  -l,--list <file>     Text file that lists all input files as whitespace-separated pairs "<< endl
         << "                            <data-name> <data-filename>" << endl
         << "                       where <data-name> is unique identifier (without whitespace)" << endl
         << "                       and <data-filename> is full path to each input file." << endl
         << "                       Default data file format is FASTA (uncompressed)." << endl
         << "  -t,--tmp <file>      Store temporary index data" << endl
         << "options:" << endl
         << "  -m,--min <int>       Minimum length to report (default 9)" << endl
         << "  -M,--max <int>       Maximum length to report (default 100)" << endl
         << "  -f,--freq <int>      Minimum frequency per input file to report (default 1)" << endl
         << "  -s,--minsupp <int>   Minimum number of input files with support to report (default 2)" << endl
         << "  -S,--maxsupp <int>   Maximum number of input files with support to report (default inf)" << endl
         << "  -v,--verbose         Verbose output" << endl;
    exit(1);
}

bool configuration::parse(int argc, char **argv)
{    
    static struct option long_options[] =
        {
            {"list",      required_argument, 0, 'l'},
            {"tmp",       required_argument, 0, 't'},
            {"min",       required_argument, 0, 'm'},
            {"max",       required_argument, 0, 'M'},
            {"freq",      required_argument, 0, 'f'},
            {"minsupp",   required_argument, 0, 's'},
            {"maxsupp",   required_argument, 0, 'S'},
            {"verbose",   no_argument,       0, 'v'},
            {"debug",     no_argument,       0, 'D'},
            {"help",      no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "l:t:m:M:f:s:S:vDh",
                            long_options, &option_index)) != -1) 
    {
        switch(c) 
        {
        case 'l':
            listfile = string(optarg); break;
        case 't':
            tmpfile = string(optarg); break;
        case 'm':
            minlength = atoi_min(optarg, 1, "-m,--min", argv[0]); break;
        case 'M':
            maxlength = atoi_min(optarg, 1, "-M,--max", argv[0]); break;
        case 'f':
            minfreq = atoi_min(optarg, 1, "-f,--freq", argv[0]); break;
        case 's':
            minsupport = atoi_min(optarg, 1, "-s,--minsupp", argv[0]); break;
        case 'S':
            maxsupport = atoi_min(optarg, 1, "-S,--maxsupp", argv[0]); break;
        case 'v':
            verbose = true; break;
        case 'D':
            debug = true; break;
        case 'h':
            print_usage(); return 1;
        default:
            cerr << "error: invalid parameter!" << endl;
            return false;
        }
    }
    if (listfile == "")
    {
        cerr << "error: -l,--list is a required parameter" << endl;
        return false;
    }
    if (tmpfile == "")
    {
        cerr << "error: -t,--tmp is a required parameter" << endl;
        return false;
    }

    if (minlength > maxlength)
    {
        cerr << "error: -m,--min must be smaller than or equal to -M,--max" << endl;
        return false;
    }
    if (minsupport > maxsupport)
    {
        cerr << "error: -s,--minsupp must be smaller than or equal to -S,--maxsupp" << endl;
        return false;
    }

    return true;
}
