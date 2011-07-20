#ifndef ERRORMINING_BESTRATIOEXPANDER_HH
#define ERRORMINING_BESTRATIOEXPANDER_HH

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
    class BestRatioExpander : public Expander
    {
    public:
        BestRatioExpander(HashAutomatonPtr parsableHA, HashAutomatonPtr unparsableHA,
            SuffixArrayPtr goodSA, SuffixArrayPtr badSA,
            size_t n, double expansionFactorAlpha)
            : d_parsableHashAutomaton(parsableHA),
              d_unparsableHashAutomaton(unparsableHA),
              d_goodSuffixArray(goodSA), d_badSuffixArray(badSA),
              d_n(n),
              d_expansionFactorAlpha(expansionFactorAlpha),
              d_freqCache(new QCache<std::vector<int>, std::pair<size_t, size_t> >(1000000)) {}
        
        virtual ~BestRatioExpander() {}
        
        std::vector<Expansion> operator()(TokensIter begin, TokensIter end);
        
        // Calculate the expansion factor of an n-gram.
        double expansionFactor(size_t unparsableFreq) const;
        
        // Retrieve the parsable/unparsable frequencies of an n-gram.
        std::pair<size_t, size_t> ngramFreqs(TokensIter const &ngramBegin,
                          TokensIter const &ngramEnd) const;
        
        // Translate a sequence from the unparsable corpus to the parsable corpus. This
        // is necessary, because both corpera use a different hash automaton.
        Tokens unparsableToParsableHashCodes(
            TokensIter const &unparsableNgramBegin,
            TokensIter const &unparsableNgramEnd) const;

    private:
        HashAutomatonPtr d_parsableHashAutomaton;
        HashAutomatonPtr d_unparsableHashAutomaton;
        SuffixArrayPtr d_goodSuffixArray;
        SuffixArrayPtr d_badSuffixArray;
        size_t d_n;
        double d_expansionFactorAlpha;
        QSharedPointer<QCache<std::vector<int>, std::pair<size_t, size_t> > > d_freqCache;
    };

}

#endif // ERRORMINING_BESTRATIOEXPANDER_HH