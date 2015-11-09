#include "default.h"
#include "configuration.h"
#include "input_reader.h"
#include <sdsl/suffix_trees.hpp> // TODO: replace with csa+lcp array
#include <sdsl/wt_algorithm.hpp>
#include <iostream>
#include <vector>
#include <cstdlib> // std::exit()
using namespace std;

typedef sdsl::cst_sct3<> cst_t;
typedef sdsl::wt_int<> wt_t;
typedef sdsl::bit_vector bitv_t;
typedef cst_t::char_type char_type;
typedef cst_t::node_type node_type;
typedef wt_t::size_type size_type;

/**
 * Construct the sequence labels
 *
 * Assumes that the number of input files is less than 2^DBITS.
 * The value of DBITS has to be set at compile time (in defaults.h).
 * Large DBITS values result in large memory requirements for wt_init().
 */
void wt_init(wt_t &wt, bitv_t &separator, cst_t &cst, input_reader *ir, configuration &config)
{
    uint64_t n = cst.csa.size();
    sdsl::int_vector<DBITS> labels(n, ~0u);
    separator = bitv_t(n, 0);
    uint64_t k = ir->size()-1;
    uint64_t j = cst.csa.wavelet_tree.select(1, 0);
    if (config.debug)
        cerr << "bwt end marker pos = " << j << endl;
    uint64_t bwtendpos = j;
    j = cst.csa.lf[j];
    labels[j] = 0;  // Label of last byte
    separator[n-1] = 0;
    separator[n-2] = 1;
    j = cst.csa.lf[j];
    for (uint64_t i = n-2; i > 0; i--) {
        char_type c = cst.csa.bwt[j];
        labels[j] = k;
        if (c == '$')
            k --;
        if (c == '$' || c == '#')
            separator[i-1] = 1;
        
        j = cst.csa.lf[j];
    }
    labels[j] = k;
    if (j != bwtendpos || k != 0) // Assert
    {
        cerr << "Labeling failed, j = " << j << ", k = " << k << endl;
        exit(1);
    }

    //TODO cleanup
    /*for (uint64_t i = 0; i < n; ++i) 
        cerr << cst.csa.text[i];
    cerr << endl;
    for (uint64_t i = 0; i < n; ++i) 
        cerr << separator[i];
    cerr << endl;
    for (uint64_t i = 0; i < n; ++i) 
        cerr << labels[cst.csa.isa[i]];
    cerr << endl;
    */  
        
    std::string tmp_file = sdsl::ram_file_name(sdsl::util::to_string(sdsl::util::pid())+"_"+sdsl::util::to_string(sdsl::util::id()));
    sdsl::store_to_file(labels, tmp_file);
    sdsl::int_vector_buffer<DBITS> text_buf(tmp_file);
    wt = wt_t(text_buf, labels.size());
    if (config.debug)
        cerr << "wt size = " << wt.size() << ", n = " << n << endl;
    j = 0;
    for (uint64_t i = 0; i < ir->size(); ++i)
        j += wt.rank(n, i);
    if (j != n) // Assert
    {
        cerr << "Label sum failed, j = " << j << ", n = " << n << endl;
        exit(1);
    }
    
}
    
int main(int argc, char ** argv)
{
    configuration config(argc, argv);
    if (!config.good)
        config.print_short_usage();

    if (config.verbose)
        cerr << "Reading input files..." << endl;
    input_reader *ir = input_reader::build(config);
    if (config.verbose)
        cerr << "Read " << ir->size() << " input files and " << ir->total_seqs() << " sequences of total length " << ir->total_size() << " (includes rev.compl. sequences)" << endl;
    
    /**
     * Initialize the data structures
     */
    if (config.verbose)
        cerr << "Constructing the data structures..." << endl;
    cst_t cst;    
    construct(cst, config.tmpfile + ".tmp", 1);
    if (!cst.csa.size())
    {
        cerr << "error: unable to construct the data structure; out of memory?" << endl; 
        abort();
    }
    
    wt_t label_wt;
    bitv_t separator;
    wt_init(label_wt, separator, cst, ir, config);

    bitv_t::rank_1_type sep_rank1(&separator);
    //bitv_t::select_1_type sep_select1(&separator); TODO Remove?
    assert(sep_rank1(cst.size()) == ir->total_seqs());
    
    size_type support = 0;
    vector<wt_t::value_type> labels(ir->size(), 0);
    vector<size_type> rank_sp(ir->size(), 0);
    vector<size_type> rank_ep(ir->size(), 0);

    if (config.verbose)
        cerr << "Construction complete, the main index requires " << size_in_mega_bytes(cst) << " MiB plus " << size_in_mega_bytes(label_wt) << " MiB for labels." << endl;
    
    /**
     * Main loop
     */
    node_type root = cst.root();
    vector<node_type> buffer;
    buffer.reserve(1024*1024);
    for (auto& child: cst.children(root))
        buffer.push_back(child);
    while (!buffer.empty())
    {
        node_type const node = buffer.back();
        buffer.pop_back();        
        unsigned depth = cst.depth(node);
        if (depth < config.maxlength)
            for (auto& child: cst.children(node))
                buffer.push_back(child);
        if (depth < config.minlength)
            continue;
        if (cst.is_leaf(node))
            continue;
        
        // Process the candidate node
        size_type sp = cst.lb(node);
        size_type ep = cst.rb(node);
        node_type wn = cst.wl(node, cst.csa.bwt[sp]);
        /*if (config.debug)
        {
            size_type pos = cst.csa[sp];
            auto s = extract(cst.csa, pos, pos + depth - 1);
            cerr << "at node = " << depth << "-[" << sp << "," << ep << "], wl = " << (wn != root);
            if (wn!=root)
                cerr << "[" << cst.rb(wn)-cst.lb(wn) << " vs " << ep-sp << "]";
            cerr << ", seq = " << s << endl;
            }*/
        if (wn == root && config.debug)
        {
            cerr << "warning: no Weiner-link at " << depth << "-[" << sp << "," << ep << "]" << endl;
            continue;
        }
        if (depth < config.maxlength && cst.rb(wn)-cst.lb(wn) == ep-sp)
            continue; // not left-branching

        sdsl::interval_symbols(label_wt, sp, ep+1, support, labels, rank_sp, rank_ep);
        if (support < config.minsupport || support > config.maxsupport)
            continue;
        
        size_type truesupp = 0;
        for (size_type i = 0; i < support; ++i)
            if (config.minfreq <= rank_ep[i]-rank_sp[i])
                ++truesupp;
        if (truesupp < config.minsupport)
            continue;
        
        if (depth > config.maxlength)
            depth = config.maxlength;
        size_type pos = cst.csa[sp];
        // Check for separator symbol TODO cleanup
        /*unsigned p_depth = cst.depth(cst.parent(node));
        if (sep_rank1(pos) != sep_rank1(pos + p_depth))
            continue; // Separator occurs above parent node
        if (sep_rank1(pos) != sep_rank1(pos + depth))
            depth = sep_select1(sep_rank1(pos)+1) - pos +1; // Separator above current node      
        */
        
        if (sep_rank1(pos) != sep_rank1(pos + depth))
            continue;
        auto s = extract(cst.csa, pos, pos + depth - 1);
        if (input_reader::smaller_than_rev_cmpl(s))
            continue;
        cout << s + " |";
        for (size_type i = 0; i < support; ++i)
            if (config.minfreq <= rank_ep[i]-rank_sp[i])
                cout << ' ' << ir->id(labels[i]) << ':' << rank_ep[i]-rank_sp[i];
        cout << '\n';
    }
    
    if (config.verbose)
        cerr << "All done." << endl;    
    delete ir; ir = 0;
    return 0;
}
