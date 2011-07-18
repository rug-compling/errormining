#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <errormining/SentenceHandler.hh>
#include <errormining/CharacterReader.hh>

namespace errormining {
    
    void CharacterReader::read(std::istream &parsable, std::istream &unParsable)
    {
        readSentences(unParsable, 1.0);
        readSentences(parsable, 0.0);
    }
    
    void CharacterReader::readSentences(std::istream &in, double error)
    {
        std::string line;
        while (std::getline(in, line)) {
            // Extract characters. Storing this in a vector of strings is not very efficient,
            // but currently expected.
            std::vector<std::string> chars;
            for (std::string::const_iterator iter = line.begin(); iter != line.end();
                    ++iter)
                chars.push_back(std::string(1, *iter));
                        
            // Call handlers.
            for (std::vector<SentenceHandler *>::const_iterator iter = d_handlers->begin();
                 iter != d_handlers->end(); ++iter)
                (*iter)->handleSentence(chars, error);
        }
    }
    
}