#include <cmath>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <errormining/BestRatioExpander.hh>
#include <errormining/Expander.hh>

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
                else
                    break;

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
}