#ifndef ERRORMINING_READER_HH
#define ERRORMINING_READER_HH

#include <algorithm>
#include <vector>

#include <QSharedPointer>

#include "SentenceHandler.hh"

namespace errormining
{
    
    /**
     * A simple sentence reader class. Reads sentences from a stream which
     * has one sentence per line with whitespace-separated tokens.
     */
    class Reader
    {
    public:
        /**
         * Construct a TokenizedSentenceReader.
         *
         * @param handlers The handlers to which each sentence should be passed.
         */
        Reader() : d_handlers(new std::vector<SentenceHandler *>()) {}
        
        virtual ~Reader() {}
        
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
        virtual void read(std::istream &parsable, std::istream &unParsable) = 0;
        
        /*
         * Remove all registered occurances of a handler. The pointer to the
         * handler is not dereferenced, so only pointers to the same handler
         * instance are removed.
         */
        void removeHandler(SentenceHandler *handler);
    protected:
        QSharedPointer<std::vector<SentenceHandler *> > d_handlers;
    };
    
    inline void Reader::addHandler(SentenceHandler *handler)
    {
        d_handlers->push_back(handler);
    }
    
    inline void Reader::removeHandler(SentenceHandler *handler)
    {
        d_handlers->erase(std::remove(d_handlers->begin(), d_handlers->end(), handler),
                          d_handlers->end());
    }
    
    
}

#endif /* ERRORMINING_READER_HH */
