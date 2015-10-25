#ifndef INPUT_READER_H
#define INPUT_READER_H
#include "configuration.h"
#include <string>
#include <vector>
#include <iostream>

class configuration;

/**
 * Base class for input
 *
 * Use input_reader::build() to construct an instance.
 */
class input_reader
{
public:
    enum input_format_t { input_unset, input_fasta };

    // Constructor
    static input_reader* build(configuration const &);
    
    // Accessors for metadata
    std::string const & id(std::size_t i) const
        { return id_[i]; }
    std::size_t size(std::size_t i) const
        { return size_[i]; }
    std::size_t size() const
        { return size_.size(); }
    std::size_t total_size() const
        { return total_size_; }
    std::size_t total_seqs() const
        { return total_seqs_; }

    
    virtual ~input_reader()
    { if (fp != &std::cin) delete fp; fp = 0; }
    
    static bool smaller_than_rev_cmpl(std::string const &);
protected:
    explicit input_reader(std::string);
    std::istream *fp;
    std::vector<std::string> id_;
    std::vector<std::size_t> size_;
    std::size_t total_size_;
    unsigned total_seqs_;
private:
    input_reader();
    // No copy constructor or assignment
    input_reader(input_reader const&);
    input_reader& operator = (input_reader const&);
};


/**
 * Simple FASTA reader
 */
class fasta_input_reader : public input_reader
{
public:
    explicit fasta_input_reader(configuration const &);
    virtual ~fasta_input_reader()
    {  }
private:
    fasta_input_reader();
    // No copy constructor or assignment
    fasta_input_reader(fasta_input_reader const&);
    fasta_input_reader& operator = (fasta_input_reader const&);
};
#endif
