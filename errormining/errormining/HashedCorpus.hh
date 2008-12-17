#ifndef HASHCORPUS_HH_
#define HASHCORPUS_HH_

#include <string>
#include <vector>

#include <tr1/memory>

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
	HashedCorpus(std::tr1::shared_ptr<HashAutomaton const> parsableHashAutomaton,
			std::tr1::shared_ptr<HashAutomaton const> unparsableHashAutomaton) :
		d_parsableHashAutomaton(parsableHashAutomaton),
		d_unparsableHashAutomaton(unparsableHashAutomaton),
		d_goodCorpus(new std::vector<int>),
		d_badCorpus(new std::vector<int>) {}

	/**
	 * Get the corpus of unparsable sentences.
	 */
	std::tr1::shared_ptr<std::vector<int> const> bad() const;

	/**
	 * Get the corpus of parsable sentences.
	 */
	std::tr1::shared_ptr<std::vector<int> const> good() const;
	void handleSentence(std::vector<std::string> const &tokens,
			double error);
private:
	HashedCorpus(HashedCorpus const &other);
	HashedCorpus &operator=(HashedCorpus const &other);
	std::tr1::shared_ptr<HashAutomaton const> d_parsableHashAutomaton;
	std::tr1::shared_ptr<HashAutomaton const> d_unparsableHashAutomaton;
	std::tr1::shared_ptr<std::vector<int> > d_goodCorpus;
	std::tr1::shared_ptr<std::vector<int> > d_badCorpus;
};

inline std::tr1::shared_ptr<std::vector<int> const> HashedCorpus::bad() const
{
	return d_badCorpus;
}

inline std::tr1::shared_ptr<std::vector<int> const> HashedCorpus::good() const
{
	return d_goodCorpus;
}

}

#endif // HASHCORPUS_HH_
