#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <errormining/SentenceHandler.hh>
#include <errormining/TokenizedSentenceReader.hh>

namespace errormining {

void TokenizedSentenceReader::read(std::istream &parsable, std::istream &unParsable)
{
	readSentences(unParsable, 1.0);
	readSentences(parsable, 0.0);
}

void TokenizedSentenceReader::readSentences(std::istream &in, double error)
{
    std::string line;
	while (std::getline(in, line)) {
        // Extract tokens.
        std::istringstream lineStream(line);
        std::vector<std::string> sentence((std::istream_iterator<std::string>(lineStream)),
            std::istream_iterator<std::string>());

		// Call handlers.
		for (std::vector<SentenceHandler *>::const_iterator iter = d_handlers->begin();
				iter != d_handlers->end(); ++iter)
			(*iter)->handleSentence(sentence, error);
	}
}

}