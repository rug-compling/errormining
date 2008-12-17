#include "HashedCorpus.ih"

void HashedCorpus::handleSentence(vector<string> const &tokens,
		double error)
{
	// Select the corpus and automaton based on the error rate of the
	// sentence (0.0 is parsable).
	vector<int> *corpus = error == 0.0 ? d_goodCorpus.get() : d_badCorpus.get();
	HashAutomaton const *hashAutomaton = error == 0.0 ?
			d_parsableHashAutomaton.get() : d_unparsableHashAutomaton.get();

	// Hash the sentence.
	transform(tokens.begin(), tokens.end(), back_inserter(*corpus),
			*hashAutomaton);
}
