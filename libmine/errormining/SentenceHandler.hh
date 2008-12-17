#ifndef SENTENCEHANDLER_HH_
#define SENTENCEHANDLER_HH_

#include <string>
#include <vector>

namespace errormining
{
	/**
	 * Abstract base class for classes that can handle sentences.
	 */
	class SentenceHandler
	{
	public:
		/**
		 * Handle a sentence.
		 * @param sentence The sentence represented as a vector of tokens.
		 * @param error The sentence error rate.
		 */
		virtual void handleSentence(std::vector<std::string> const &sentence,
			double error) = 0;
		virtual ~SentenceHandler() {};
	};
}

#endif /*SENTENCEHANDLER_HH_*/
