#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "input_reader.h"
#include <string>


class configuration
{
public:
    configuration(int, char **);
    bool parse(int, char **);
    void print_short_usage() const;
    void print_usage() const;

    std::string prog;
    std::string listfile;
    std::string tmpfile;
    int inputformat;
    unsigned minlength;
    unsigned maxlength;
    unsigned minfreq;
    unsigned minsupport;
    unsigned maxsupport;
    bool verbose;
    bool debug;
    bool good;
private:
    configuration();
};
#endif
