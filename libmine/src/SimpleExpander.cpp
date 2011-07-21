#include <utility>
#include <vector>

#include <errormining/Expander.hh>
#include <errormining/SimpleExpander.hh>

namespace errormining {
    std::vector<Expansion> SimpleExpander::operator()(TokensIter begin, TokensIter end)
    {
        std::vector<Expansion> expansions;
        
        // Simple n-gram collection: add all n to m-grams in a given sentence.
        for (size_t len = d_n; len <= d_m && begin + len <= end; ++len)
        {            
            std::pair<size_t, size_t> freq = ngramFreqs(begin, begin + len);
            expansions.push_back(Expansion(std::make_pair(begin, begin + len), freq.first, freq.second));
        }
        
        return expansions;
    }
                                 
}

