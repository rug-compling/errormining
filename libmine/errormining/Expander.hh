#ifndef ERRORMINING_EXPANDER_HH
#define ERRORMINING_EXPANDER_HH

#include <vector>

#include <QHash>

namespace errormining {

    typedef std::vector<int> Tokens;
    typedef std::vector<int>::const_iterator TokensIter;
    
    typedef std::pair<TokensIter, TokensIter> TokensIterPair;

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
    
    class Expander
    {
    public:
        virtual ~Expander() {}
        virtual std::vector<Expansion> operator()(TokensIter begin,
            TokensIter end) = 0;
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