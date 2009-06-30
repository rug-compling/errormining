#ifndef HASHCORPUS_HH_
#define HASHCORPUS_HH_

#include <string>
#include <vector>

#include <QSharedPointer>

#include "HashAutomaton.hh"
#include "SentenceHandler.hh"

namespace errormining
{

/**
 * The HashedCorpus class extends the SentenceHandler abstract class. It will
 * store sentence tokens by their hash numbers, as defined by a perfect hash
 * automaton.
 */
class HashedCorpus : public SentenceHandler
{
public:
	/**
	 * Construct a HashedCorpus class.
	 * @param hashAutomaton A shared pointer to the perfect hash automaton
	 *  to use.
	 */
	HashedCorpus(QSharedPointer<HashAutomaton const> parsableHashAutomaton,
			QSharedPointer<HashAutomaton const> unparsableHashAutomaton) :
		d_parsableHashAutomaton(parsableHashAutomaton),
		d_unparsableHashAutomaton(unparsableHashAutomaton),
		d_goodCorpus(new std::vector<int>),
		d_badCorpus(new std::vector<int>) {}

	/**
	 * Get the corpus of unparsable sentences.
	 */
	QSharedPointer<std::vector<int> const> bad() const;

	/**
	 * Get the corpus of parsable sentences.
	 */
	QSharedPointer<std::vector<int> const> good() const;
	void handleSentence(std::vector<std::string> const &tokens,
			double error);
private:
	HashedCorpus(HashedCorpus const &other);
	HashedCorpus &operator=(HashedCorpus const &other);
	QSharedPointer<HashAutomaton const> d_parsableHashAutomaton;
	QSharedPointer<HashAutomaton const> d_unparsableHashAutomaton;
	QSharedPointer<std::vector<int> > d_goodCorpus;
	QSharedPointer<std::vector<int> > d_badCorpus;
};

inline QSharedPointer<std::vector<int> const> HashedCorpus::bad() const
{
	return d_badCorpus;
}

inline QSharedPointer<std::vector<int> const> HashedCorpus::good() const
{
	return d_goodCorpus;
}

}

#endif // HASHCORPUS_HH_
