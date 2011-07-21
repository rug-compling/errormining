#ifndef ERRORMINING_EXPANDER_HH
#define ERRORMINING_EXPANDER_HH

#include <utility>
#include <vector>

#include <QCache>
#include <QHash>
#include <QSharedPointer>

namespace errormining {
    // Forward declarations.
    template <typename T>
    class SuffixArray;
    
    class HashAutomaton;

    // Handy typedefs.
    typedef QSharedPointer<HashAutomaton const> HashAutomatonPtr;
    typedef QSharedPointer<SuffixArray<int> const> SuffixArrayPtr;
    
    typedef std::vector<int> Tokens;
    typedef std::vector<int>::const_iterator TokensIter;
    
    typedef std::pair<TokensIter, TokensIter> TokensIterPair;

    /**
     * This struct represents an expansion. An expander could choose to expand a unigram
     * to a longer n-gram. An n-gram is represented as an iterator pair.
     */
    struct Expansion
    {
        Expansion(TokensIterPair newIters,
            size_t newParsableFreq,
            size_t newUnparsableFreq)
        : iters(newIters),
          parsableFreq(newParsableFreq),
          unparsableFreq(newUnparsableFreq) {}
        
        TokensIterPair iters;
        size_t parsableFreq;
        size_t unparsableFreq;
    };
    
    /**
     * Base class for expanders. An expander expands a unigram (possibly) to a longer
     * n-gram.
     */
    class Expander
    {
    public:
        Expander(HashAutomatonPtr parsableHA, HashAutomatonPtr unparsableHA,
                 SuffixArrayPtr goodSA, SuffixArrayPtr badSA)
        : d_parsableHashAutomaton(parsableHA),
          d_unparsableHashAutomaton(unparsableHA),
          d_goodSuffixArray(goodSA),
          d_badSuffixArray(badSA),
          d_freqCache(new QCache<std::vector<int>, std::pair<size_t, size_t> >(1000000)) {}
        
        virtual ~Expander() {}
        
        /**
         * Perform an expansion.
         *
         * @begin Iterator pointing to the unigram.
         * @end End iterator pointing beyond the largest allowed expansion. This is
         *      normally the end of the sentence.
         */
        virtual std::vector<Expansion> operator()(TokensIter begin,
            TokensIter end) = 0;

    protected:
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
        QSharedPointer<QCache<std::vector<int>, std::pair<size_t, size_t> > > d_freqCache;
    };

}

namespace std {
    template <typename T>
    inline uint qHash(std::vector<T> const &vec)
    {
        uint seed = ::qHash(*(vec.begin()));
        
        // Hash a vector of values, the hash combination method is derrived
        // from Boost hash_combine().
        for (typename std::vector<T>::const_iterator iter = vec.begin() + 1;
             iter != vec.end(); ++iter)
            seed ^= ::qHash(*iter) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        
        return seed;
    }
}

#endif // ERRORMINING_EXPANDER_HH