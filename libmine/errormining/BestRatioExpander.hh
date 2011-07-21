#ifndef ERRORMINING_BESTRATIOEXPANDER_HH
#define ERRORMINING_BESTRATIOEXPANDER_HH

#include <utility>
#include <vector>

#include <QCache>
#include <QSharedPointer>

#include "Expander.hh"

namespace errormining {    
    /**
     */
    class BestRatioExpander : public Expander
    {
    public:
        BestRatioExpander(HashAutomatonPtr parsableHA,
            HashAutomatonPtr unparsableHA,
            SuffixArrayPtr goodSA, SuffixArrayPtr badSA,
            size_t n, double expansionFactorAlpha)
            : Expander(parsableHA, unparsableHA, goodSA, badSA),
              d_n(n),
              d_expansionFactorAlpha(expansionFactorAlpha)
              {}
        
        virtual ~BestRatioExpander() {}
        
        std::vector<Expansion> operator()(TokensIter begin, TokensIter end);
        
        // Calculate the expansion factor of an n-gram.
        double expansionFactor(size_t unparsableFreq) const;
        
    private:
        size_t d_n;
        double d_expansionFactorAlpha;
    };

}

#endif // ERRORMINING_BESTRATIOEXPANDER_HH