#ifndef TOKENIZEDSENTENCEREADER_HH_
#define TOKENIZEDSENTENCEREADER_HH_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "SentenceHandler.hh"

namespace errormining
{

/**
 * A simple sentence reader class. Reads sentences from a stream which
 * has one sentence per line with whitespace-separated tokens.
 */
class TokenizedSentenceReader
{
public:
	/**
	 * Construct a TokenizedSentenceReader.
	 *
	 * @param handlers The handlers to which each sentence should be passed.
	 */
	TokenizedSentenceReader()
		: d_handlers(new std::vector<SentenceHandler *>()) {}

	/**
	 * Add a sentence handler to the reader.
	 */
	void addHandler(SentenceHandler *handler);

	/**
	 * Read a corpus/sentence file. Both parsable and unparsable sentences
	 * are passed in one go.
	 *
	 * @param parsable Stream with parsable sentences.
	 * @param unparsable Stream with unparsable sentences.
	 */
	void read(std::istream &parsable, std::istream &unParsable);

	/*
	 * Remove all registered occurances of a handler. The pointer to the
	 * handler is not dereferenced, so only pointers to the same handler
	 * instance are removed.
	 */
	void removeHandler(SentenceHandler *handler);
private:
	void readSentences(std::istream &in, double error);

	std::auto_ptr<std::vector<SentenceHandler *> > d_handlers;
};

inline void TokenizedSentenceReader::addHandler(SentenceHandler *handler)
{
	d_handlers->push_back(handler);
}

inline void TokenizedSentenceReader::removeHandler(SentenceHandler *handler)
{
	d_handlers->erase(std::remove(d_handlers->begin(), d_handlers->end(), handler),
			d_handlers->end());
}


}

#endif /*TOKENIZEDSENTENCEREADER_HH_*/
