#include "default.h"
#include "input_reader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib> // abort()
#include <cassert>
using namespace std;

input_reader * input_reader::build(configuration const &config)
{
    switch (config.inputformat)
    {
    case input_fasta:
        return new fasta_input_reader(config);
        break;
    default:
        cerr << "Error: invalid file type at InputReader::build()" << endl; 
        abort();
    }
}

    std::istream *fp;
    std::vector<std::string> id_;
    std::vector<std::size_t> size_;
    std::size_t total_size_;
    bool good_;

input_reader::input_reader(string file)
    : fp(0), id_(), size_(), total_size_(0), total_seqs_(0)
{
    // Open file handle, "-" uses standard input
    if (file == "-")
        fp = &std::cin;
    else
        fp = new ifstream(file.c_str());
}

void error_reading_input(configuration const &config, char const *message)
{
    cerr << message << endl;
    config.print_short_usage();
    exit(1);
}

/**
 * Simple FASTA input
 */
void normalize(std::string &t)
{
    for (std::string::iterator it = t.begin(); it != t.end(); ++it)
    {
        switch (*it)
        {
        case('a'):
            *it = 'A';
            break;
        case('c'):
            *it = 'C';
            break;
        case('g'):
            *it = 'G';
            break;
        case('t'):
            *it = 'T';
            break;
        case('n'):
            *it = 'N';
            break;
        case('A'):
        case('C'):
        case('G'):
        case('T'):
        case('N'):
            break;
        case('0'):
        case('1'):
        case('2'):
        case('3'):
        case('.'):
            break;
        default:
            *it = 'N';
            break;
        }
    }
}

void revcmpl(std::string &t)
{
    char c;
    std::size_t n = t.size();
    for (std::size_t i = 0; i < n / 2; ++i) {
        c = t[i];
        t[i] = t[n - i - 1];
        t[n - i - 1] = c;
    }
    for (std::string::iterator it = t.begin(); it != t.end(); ++it)
    {
        switch (*it)
        {
        case('T'):
            *it = 'A'; break;
        case('G'):
            *it = 'C'; break;
        case('C'):
            *it = 'G'; break;
        case('A'):
            *it = 'T'; break;
        }
    }
}

bool input_reader::smaller_than_rev_cmpl(string const& path)
{
    // Check if given path is smaller than its rev.compl.
    string rc(path);
    revcmpl(rc);
    if (path < rc)
        return true;
    return false;
}

size_t append(ofstream &ofs, string const &fname, unsigned &nseqs)
{
    string row;
    string buffer;
    buffer.reserve(1024*1024);
    ifstream ifs(fname.c_str());
    bool first = true;
    size_t n = 0;
    while (std::getline(ifs, row).good())
    {
        if (row.empty())
            continue;
        if (row[0] == '>')
        {
            if (!first)
            {
                ofs << '#';                
                nseqs++;
                n ++;
                revcmpl(buffer);
                ofs << buffer << '#';
                nseqs++;
                n += buffer.size() + 1;
                buffer.clear();
            }
            first = false;
            continue;
        }
        if (row[row.size()-1] == '\n')
            row.pop_back();
        if (row[row.size()-1] == '\r')
            row.pop_back();
        normalize(row);
        ofs << row;
        buffer += row;
        n += row.size();
    }
    if (n == 0)
    {
        cerr << "error: Unable to read the input file " << fname << endl;
        exit(1);
    }
    ofs << '#';
    nseqs ++;
    n ++;
    revcmpl(buffer);
    ofs << buffer << '$';
    nseqs++;
    n += buffer.size() + 1;
    buffer.clear();
    return n;
}

bool file_exists (string const &name)
{
    ifstream f(name.c_str());
    if (f.good())
    {
        f.close();
        return true;
    }
    else
    {
        f.close();
        return false;
    }   
}

fasta_input_reader::fasta_input_reader(configuration const &config)
    : input_reader(config.listfile)
{
    string tmpfile = config.tmpfile + ".tmp";
    string metafile = config.tmpfile + ".meta";
//    if (file_exists(tmpfile) && file_exists(metafile))
// TODO        cerr << "warning: tmp files under " << config.tmpfile << " will be rebuilt" << endl;
        
    ofstream ofs(tmpfile.c_str());
    ofstream ofsmeta(metafile.c_str());
    // Parse the list file
    string id, fname;
    while (fp->good())
    {
        *fp >> id;
        if (id.empty() || !fp->good())
            break;
        *fp >> fname;
        if (fname.empty())
            error_reading_input(config, "error: Unable to read the -l,--list input file: please check the list file format.");

        id_.push_back(id);
        size_t s = append(ofs, fname, total_seqs_);
        size_.push_back(s);
        total_size_ += s;
        ofsmeta << id << "\t" << s << "\t" << endl;
    }
    if (id_.empty() || total_size_ == 0) 
        error_reading_input(config, "error: Unable to read the -l,--list input file: please check the list file format.");
    if (!ofs.good()) 
        error_reading_input(config, "error: Unable to write the temporary files under the location -i,--index.");
    if (!ofsmeta.good()) 
        error_reading_input(config, "error: Unable to write the temporary files under the location -i,--index.");
    
    if ((1 << DBITS) <= size_.size())
    {
        cerr << "error: increase DBITS value to support more than " << (1<<DBITS)
             << " input files; see the file default.h for more details and recompile." << endl;
        exit(1);
    }

    ofsmeta.close();
    ofs.close();
}

