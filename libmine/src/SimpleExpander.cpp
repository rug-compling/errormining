#include <utility>
#include <vector>

#include <errormining/Expander.hh>
#include <errormining/HashAutomaton.hh>
#include <errormining/SimpleExpander.hh>
#include <errormining/SuffixArray.hh>

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
                                 
    std::pair<size_t, size_t> SimpleExpander::ngramFreqs(TokensIter const &ngramBegin,
                                                         TokensIter const &ngramEnd) const
    {
        Tokens ngram;
        std::copy(ngramBegin, ngramEnd, std::back_inserter(ngram));
        
        // See if we have cached the frequencies.
        std::pair<size_t, size_t> *freqs;
        if ((freqs = d_freqCache->object(ngram)) != 0)
            return *freqs;
        
        // Since a different hashing function is used for parsable sentences,
        // we'll have to convert the n-gram in unparsable hash codes to
        // parsable hash codes.
        Tokens parsableNgram = unparsableToParsableHashCodes(ngramBegin,
                                                             ngramEnd);
        
        SuffixArray<int>::IterPair goodIters = d_goodSuffixArray->find(parsableNgram.begin(),
                                                                       parsableNgram.end());
        size_t goodFreq = distance(goodIters.first, goodIters.second);
        
        SuffixArray<int>::IterPair badIters = d_badSuffixArray->find(ngramBegin,
                                                                     ngramEnd);
        size_t badFreq = distance(badIters.first, badIters.second);
        
        // Cache if this was a unigram.
        if (distance(ngramBegin, ngramEnd) == 1)
            d_freqCache->insert(ngram, new std::pair<size_t, size_t>(goodFreq, badFreq),
                                ngram.size());
        
        return std::make_pair(goodFreq, badFreq);
    }
    
    std::vector<int> SimpleExpander::unparsableToParsableHashCodes(
                                                                      TokensIter const &unparsableNgramBegin,
                                                                      TokensIter const &unparsableNgramEnd) const
    {
        // Since different hash automata are used for parsable and unparsable
        // sentences, we often need to convert the hashcode for a token from
        // a 'parsable hashcode' to an 'unparsable hashcode'.
        
        // Convert from unparsable hash codes to strings.
        std::vector<std::string> stringNgram;
        transform(unparsableNgramBegin, unparsableNgramEnd,
                  back_inserter(stringNgram), *d_unparsableHashAutomaton);
        
        // Convert from strings to parsable hash codes.
        std::vector<int> parsableNgram;
        transform(stringNgram.begin(), stringNgram.end(), back_inserter(parsableNgram),
                  *d_parsableHashAutomaton);
        
        return parsableNgram;
    }
}

