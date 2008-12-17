#include "TokenizedSentenceReader.ih"

#include <iostream>

void TokenizedSentenceReader::read(istream &parsable, istream &unParsable)
{
	readSentences(unParsable, 1.0);
	readSentences(parsable, 0.0);
}

void TokenizedSentenceReader::readSentences(istream &in, double error)
{
	string line;
	while (getline(in, line)) {
		// Extract tokens.
		istringstream lineStream(line);
		vector<string> sentence((istream_iterator<string>(lineStream)),
			istream_iterator<string>());

		// Call handlers.
		for (vector<SentenceHandler *>::const_iterator iter = d_handlers->begin();
				iter != d_handlers->end(); ++iter)
			(*iter)->handleSentence(sentence, error);
	}
}
