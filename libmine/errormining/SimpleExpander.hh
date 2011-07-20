#ifndef ERRORMINING_SIMPLEEXPANDER_HH
#define ERRORMINING_SIMPLEEXPANDER_HH

#include <utility>
#include <vector>

#include <QCache>
#include <QSharedPointer>

#include "Expander.hh"

namespace errormining {
    
    // Forward declarations.
    template <typename T>
    class SuffixArray;
    
    class HashAutomaton;
    
    //
    typedef QSharedPointer<HashAutomaton const> HashAutomatonPtr;
    typedef QSharedPointer<SuffixArray<int> const> SuffixArrayPtr;

    
    /**
     */
    class SimpleExpander : public Expander
    {
    public:
        SimpleExpander(HashAutomatonPtr parsableHA, HashAutomatonPtr unparsableHA,
            SuffixArrayPtr goodSA, SuffixArrayPtr badSA,
            size_t n, size_t m)
        : d_parsableHashAutomaton(parsableHA),
          d_unparsableHashAutomaton(unparsableHA),
          d_goodSuffixArray(goodSA), d_badSuffixArray(badSA),
          d_n(n), d_m(m),
          d_freqCache(new QCache<std::vector<int>, std::pair<size_t, size_t> >(1000000)) {}
        
        virtual ~SimpleExpander() {}
        
        std::vector<Expansion> operator()(TokensIter begin, TokensIter end);
                
    private:
        // Retrieve the parsable/unparsable frequencies of an n-gram.
        std::pair<size_t, size_t> ngramFreqs(TokensIter const &ngramBegin,
                                             TokensIter const &ngramEnd) const;
        
        // Translate a sequence from the unparsable corpus to the parsable corpus. This
        // is necessary, because both corpera use a different hash automaton.
        Tokens unparsableToParsableHashCodes(
                                             TokensIter const &unparsableNgramBegin,
                                             TokensIter const &unparsableNgramEnd) const;

        HashAutomatonPtr d_parsableHashAutomaton;
        HashAutomatonPtr d_unparsableHashAutomaton;
        SuffixArrayPtr d_goodSuffixArray;
        SuffixArrayPtr d_badSuffixArray;
        size_t d_n;
        size_t d_m;
        QSharedPointer<QCache<std::vector<int>, std::pair<size_t, size_t> > > d_freqCache;
    };
    
}

#endif // ERRORMINING_SIMPLEEXPANDER_HH