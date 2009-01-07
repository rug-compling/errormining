#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <memory>
#include <vector>

#include <tr1/memory>

#include <errormining/HashedCorpus.hh>
#include <errormining/Miner.hh>
#include <errormining/Observer.hh>
#include <errormining/SentenceHandler.hh>
#include <errormining/SuffixArray.hh>
#include <errormining/TokenizedSentenceReader.hh>

#include "ProgramOptions.hh"

using namespace std;
using namespace std::tr1;
using namespace errormining;

class CycleNotifier : public Observer
{
	size_t d_cycle;
public:
	CycleNotifier() : d_cycle(0) {}
	void update(Observable const *observable);
};

void CycleNotifier::update(Observable const *)
{
	cerr << ".";
}

void usage(string const &programName)
{
		cerr << "Usage: " << programName <<
			" [OPTION]... parsable_fsa unparsable_fsa parsable unparsable" << endl << endl <<
			"  -b val\tEnable smoothing, and set beta to val" << endl <<
			"  -c\t\tDisable ngram expansion" << endl <<
			"  -e val\tEnable use of an expansion factor, and set alpha to val" << endl <<
			"  -f freq\tShow forms observed >= freq" << endl <<
			"  -n n\t\tUse ngrams of length n" << endl <<
			"  -m m\t\tCreate ngrams upto length m (only used with -c)" << endl <<
			"  -o alg\tSort algorithm (stlsort or ssort, default: ssort)" << endl <<
			"  -s t\t\tSuspicion threshold for excluding suspicious observations" << endl <<
			"  -t t\t\tThreshold for determining the fixed-point" << endl <<
			"  -u freq\tShow forms observed >= freq in unparsable sentences" << endl <<
			"  -v \t\tVerbose output" << endl << endl <<
			"The perfect hash automata can be created with fsa_build:" << endl << endl <<
			"tr -s '\\012\\011 ' '\\012' < oks.txt | LANG=POSIX LC_ALL=POSIX sort -u | \\" <<
			endl << "  fsa_build -N -o oks.fsa" << endl << endl;
}

shared_ptr<HashedCorpus> readHashedCorpus(ProgramOptions const &programOptions,
		shared_ptr<HashAutomaton> parsableHashAutomaton,
		shared_ptr<HashAutomaton> unparsableHashAutomaton)
{
	ifstream badIn(programOptions.arguments()[3].c_str());
	if (!badIn.good())
		throw runtime_error("Could not read " + programOptions.arguments()[3]);

	ifstream goodIn(programOptions.arguments()[2].c_str());
	if (!goodIn.good())
		throw runtime_error("Could not read " + programOptions.arguments()[2]);

	TokenizedSentenceReader reader;

	shared_ptr<HashedCorpus> hashedCorpus(new HashedCorpus(parsableHashAutomaton,
			unparsableHashAutomaton));
	reader.addHandler(hashedCorpus.get());

	reader.read(goodIn, badIn);

	return hashedCorpus;
}

int main(int argc, char *argv[])
{
	auto_ptr<ProgramOptions> programOptions;
	try
	{
		programOptions.reset(new ProgramOptions(argc, argv));
	}
	catch (string error)
	{
		cerr << error << endl << endl;
		usage(argv[0]);
		return 1;
	}

	if (programOptions->arguments().size() != 4)
	{
		usage(programOptions->programName());
		return 1;
	}

	// Read the perfect hash automaton.
	shared_ptr<HashAutomaton> parsableHashAutomaton;
	shared_ptr<HashAutomaton> unparsableHashAutomaton;
	try {
		parsableHashAutomaton.reset(new HashAutomaton(programOptions->arguments()[0]));
		unparsableHashAutomaton.reset(new HashAutomaton(programOptions->arguments()[1]));
	} catch (InvalidAutomatonException e) {
		cout << e.what() << endl;
		return 1;
	}

	// Read the corpus as a sequence of hash codes.
	if (programOptions->verbose())
		cerr << "Reading and hashing the corpus... ";
	shared_ptr<HashedCorpus> hashedCorpus = readHashedCorpus(*programOptions,
			parsableHashAutomaton, unparsableHashAutomaton);
	if (programOptions->verbose())
		cerr << "Done!" << endl << "Creating suffix arrays... ";

	// Store the corpora as suffix arrays.
	shared_ptr<SuffixArray<int> >
		goodSuffixArray(new SuffixArray<int>(
				hashedCorpus->good(), programOptions->sortAlgorithm()));
	shared_ptr<SuffixArray<int> >
		badSuffixArray(new SuffixArray<int>(
				hashedCorpus->bad(), programOptions->sortAlgorithm()));

	if (programOptions->verbose())
		cerr << "Done!" << endl;

	// Construct a sentence reader.
	TokenizedSentenceReader reader;

	// Create a miner, and register it as a handler for the sentence reader.
	Miner miner(parsableHashAutomaton, unparsableHashAutomaton, goodSuffixArray,
			badSuffixArray, programOptions->n(), programOptions->m(),
			programOptions->ngramExpansion(), programOptions->expansionFactorAlpha(),
			programOptions->smoothing(), programOptions->smoothingBeta());
	reader.addHandler(&miner);

	// Observe the mining process, if we want verbose output.
	auto_ptr<CycleNotifier> cycleNotifier;
	if (programOptions->verbose()) {
		cycleNotifier.reset(new CycleNotifier());
		miner.attach(cycleNotifier.get());
	}

	ifstream badIn(programOptions->arguments()[3].c_str());
	if (!badIn.good())
	{
		cout << "Could not read '" << programOptions->arguments()[3] <<
			"'!" << endl;
		return 1;
	}

	ifstream goodIn(programOptions->arguments()[2].c_str());
	if (!goodIn.good()) {
		cout << "Could not read '" << programOptions->arguments()[2] << "'!" << endl;
		return 1;
	}

	if (programOptions->verbose())
		cerr << "Reading parsable and unparsable sentences... ";

	reader.read(goodIn, badIn);

	if (programOptions->verbose())
		cerr << "Done!" << endl;

	if (programOptions->verbose())
		cerr << "Mining";

	// Start mining.
	miner.mine(programOptions->threshold(), programOptions->suspThreshold());

	if (programOptions->verbose())
		cerr << " Done!" << endl;

	// Retrieve forms, ordered by descending suspicion.
	set<Form, FormProbComp> forms = miner.forms();

	// Print forms with the proper frequencies.
	for (set<Form>::const_iterator formIter = forms.begin();
			formIter != forms.end(); ++formIter)
		if (formIter->nObservations() >= programOptions->frequency() &&
				formIter->nSuspObservations() >= programOptions->suspFrequency())
		{
			transform(formIter->ngram().begin(), formIter->ngram().end(),
					ostream_iterator<string>(cout, " "), *unparsableHashAutomaton);
			cout << formIter->suspicion() << " " <<
				formIter->nObservations() << " " << formIter->nSuspObservations() <<
				endl;
		}
}
