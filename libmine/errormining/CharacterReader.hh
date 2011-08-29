#ifndef CHARACTERREADER_HH
#define CHARACTERREADER_HH

#include <algorithm>
#include <iostream>
#include <vector>

#include <QSharedPointer>

#include "Reader.hh"
#include "SentenceHandler.hh"

namespace errormining
{
    /**
     * A simple sentence reader class. Reads sentences from a stream which
     * has one sentence per line with whitespace-separated tokens.
     */
    class CharacterReader : public Reader
    {
    public:
        /**
         * Construct a TokenizedSentenceReader.
         *
         * @param handlers The handlers to which each sentence should be passed.
         */
        CharacterReader() : Reader() {}
        
        /**
         * Read a corpus/sentence file. Both parsable and unparsable sentences
         * are passed in one go.
         *
         * @param parsable Stream with parsable sentences.
         * @param unparsable Stream with unparsable sentences.
         */
        void read(std::istream &parsable, std::istream &unParsable);
        
    private:
        void readSentences(std::istream &in, double error);
    };
    
}

#endif /* CHARACTERREADER_HH */