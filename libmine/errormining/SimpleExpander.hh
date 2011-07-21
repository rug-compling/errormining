#ifndef ERRORMINING_SIMPLEEXPANDER_HH
#define ERRORMINING_SIMPLEEXPANDER_HH

#include <utility>
#include <vector>

#include <QCache>
#include <QSharedPointer>

#include "Expander.hh"

namespace errormining {
    /**
     * Simple expander that includes all n to m-grams starting at the given unigram.
     */
    class SimpleExpander : public Expander
    {
    public:
        SimpleExpander(HashAutomatonPtr parsableHA, HashAutomatonPtr unparsableHA,
            SuffixArrayPtr goodSA, SuffixArrayPtr badSA,
            size_t n, size_t m)
        : Expander(parsableHA, unparsableHA, goodSA, badSA),
          d_n(n), d_m(m),
          d_freqCache(new QCache<std::vector<int>, std::pair<size_t, size_t> >(1000000)) {}
        
        virtual ~SimpleExpander() {}
        
        std::vector<Expansion> operator()(TokensIter begin, TokensIter end);
                
    private:
        size_t d_n;
        size_t d_m;
        QSharedPointer<QCache<std::vector<int>, std::pair<size_t, size_t> > > d_freqCache;
    };
    
}

#endif // ERRORMINING_SIMPLEEXPANDER_HH