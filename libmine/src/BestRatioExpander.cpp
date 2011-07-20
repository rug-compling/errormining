#include <algorithm>
#include <cmath>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <errormining/BestRatioExpander.hh>
#include <errormining/Expander.hh>
#include <errormining/HashAutomaton.hh>
#include <errormining/SuffixArray.hh>

namespace errormining {
    std::vector<Expansion> BestRatioExpander::operator()(TokensIter begin,
        TokensIter end)
    {
        TokensIterPair bestNgram(begin, begin + d_n);
        
        std::pair<size_t, size_t> freq = ngramFreqs(bestNgram.first, bestNgram.second);
        size_t bestParsableFreq = freq.first;
        size_t bestUnparsableFreq = freq.second;

        double bestNgramRatio = static_cast<double>(bestUnparsableFreq) /
            (bestParsableFreq + bestUnparsableFreq);
        
        for (TokensIter endIter = begin + d_n + 1; endIter <= end; ++endIter)
		{
			// Get the n+1-gram, which we will call a m-gram.
			TokensIterPair mgram(begin, endIter);
            
            // Calculate the m-gram ratio.
            std::pair<size_t, size_t> mgramFreq = ngramFreqs(begin, endIter);
            double mgramRatio = static_cast<double>(mgramFreq.second) /
                (mgramFreq.first + mgramFreq.second);
                        
			// Calculate the expansion factor, if it is senseful.
			double factor = 1.0;
			if (d_expansionFactorAlpha != 0.0)
				factor = expansionFactor(mgramFreq.second);
                        
			// If the m-gram has a worse ratio (in terms of relatively more
			// occurrences in unparsable sentences), we'll extend the n-gram.
            // We only calculate the ratio of the second n-gram when necessary.
			if (mgramRatio > factor * bestNgramRatio)
			{
                // Get the second n-gram within the current m-gram.
                TokensIterPair ngram(begin + 1, endIter);

                // Second n-gram ratio
                std::pair<size_t, size_t> secFreq = ngramFreqs(ngram.first, ngram.second);
                double secRatio = static_cast<double>(secFreq.second) / (secFreq.first + secFreq.second);            

                if (mgramRatio > factor * secRatio) {
                    bestNgram = mgram;
                    bestNgramRatio = mgramRatio;
                    bestParsableFreq = mgramFreq.first;
                    bestUnparsableFreq = mgramFreq.second;
                }

			}
			else
				break;
		}

        std::vector<Expansion> expansions;
        expansions.push_back(Expansion(bestNgram, bestParsableFreq, bestUnparsableFreq));
        return expansions;
    }
    
    double BestRatioExpander::expansionFactor(size_t unparsableFreq) const
    {
        return 1.0 + exp(-d_expansionFactorAlpha * static_cast<double>(unparsableFreq));
    }
    
    std::pair<size_t, size_t> BestRatioExpander::ngramFreqs(TokensIter const &ngramBegin,
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
    
    std::vector<int> BestRatioExpander::unparsableToParsableHashCodes(
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
