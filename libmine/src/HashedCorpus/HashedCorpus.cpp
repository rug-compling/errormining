#include "HashedCorpus.ih"

void HashedCorpus::handleSentence(vector<string> const &tokens,
		double error)
{
	// Select the corpus and automaton based on the error rate of the
	// sentence (0.0 is parsable).
	vector<int> *corpus = error == 0.0 ? d_goodCorpus.data() : d_badCorpus.data();
	HashAutomaton const *hashAutomaton = error == 0.0 ?
			d_parsableHashAutomaton.data() : d_unparsableHashAutomaton.data();

	// Hash the sentence.
	transform(tokens.begin(), tokens.end(), back_inserter(*corpus),
			*hashAutomaton);
}
