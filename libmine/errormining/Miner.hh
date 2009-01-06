#ifndef MINER_HH_
#define MINER_HH_

#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <tr1/memory>

#include <QHash>
#include <QSet>

#include "Form.hh"
#include "HashAutomaton.hh"
#include "Observable.hh"
#include "Sentence.hh"
#include "SentenceHandler.hh"
#include "SuffixArray.hh"

namespace errormining
{

/**
  * FormPtr is a wrapper for pointers to Forms. QHash requires overloading
  * of operator== (which we'll want in this case, because we don't want to
  * compare addresses). But we can't overload operators for plain pointers.
  */
struct FormPtr {
	FormPtr(Form *newValue) : value(newValue) {}
	Form *value;
};

/**
 * Comparison for form pointers. This will compare the forms, rather than
 * their addresses.
 */
bool operator==(FormPtr lhs, FormPtr rhs);

/**
 * Hash a form by its n-gram value.
 */
uint qHash(FormPtr const &formPtr);

/**
 * Function objects of this type compare two forms by their suspicion.
 * If both forms have an equal suspicion, the equality operator of
 * Form is used.
 */
struct FormProbComp {
	bool operator()(Form const &lhs, Form const &rhs) const;
};

struct SuspAtLeast {
	SuspAtLeast(double newSuspicion) : suspicion(newSuspicion) {}
	double operator()(FormPtr const &formPtr);
	double suspicion;
};

/**
 * Function objects of this type will add the suspicion of a form to
 * a (accumulated) value. To be used with std::accumulator().
 */
struct FormPtrSuspSum : std::binary_function<double, double, FormPtr>
{
	double operator()(double acc, FormPtr const formPtr) const;
};

/**
 * A miner class holds a set of forms and sentences composed with these
 * forms. It can use this data to find forms that are suspicious of
 * causing parse errors.
 */
class Miner : public SentenceHandler, public Observable
{
public:
	/**
	 * Construct a miner.
	 * @param parsableHashAutomaton A shared pointer to a perfect hash
	 *  automaton for the words in the corpus of parsable sentences.
	 * @param unparsableHashAutomaton A shared pointer to a perfect hash
	 *  automaton for the words in the corpus of unparsable sentences.
	 * @param parsableSuffixArray A shared pointer to the suffix array
	 *  with all parsable sentences.
	 * @param unparsableSuffixArray A shared pointer to the suffix array
	 *  with all unparsable sentences.
	 * @param n The length of n-grams to analyze.
	 * @param allNgrams Analyze all n-grams (or just those that occur in
	 *  unparsable sentences).
	 */
	Miner(std::tr1::shared_ptr<HashAutomaton const> parsableHashAutomaton,
			std::tr1::shared_ptr<HashAutomaton const> unparsableHashAutomaton,
			std::tr1::shared_ptr<SuffixArray<int> const > parsableSuffixArray,
			std::tr1::shared_ptr<SuffixArray<int> const > unparsableSuffixArray,
			size_t n = 2, bool ngramExpansion = true, double expansionFactorAlpha = 0.0,
			bool smoothing = true, double smoothingBeta = 0.1) :
		d_parsableHashAutomaton(parsableHashAutomaton),
		d_unparsableHashAutomaton(unparsableHashAutomaton),
		d_goodSuffixArray(parsableSuffixArray),
		d_badSuffixArray(unparsableSuffixArray), d_n(n),
		d_ngramExpansion(ngramExpansion), d_expansionFactorAlpha(expansionFactorAlpha),
		d_smoothing(smoothing), d_smoothingBeta(smoothingBeta),
		d_forms(new QSet<FormPtr>()),
		d_sentences(new std::list<Sentence>()),
		d_unigramRatioCache(new QHash<int, double>()) {}

	~Miner();

	/**
	 * Return the set of forms known to this miner.
	 */
	std::set<Form, FormProbComp> forms() const;

	/**
	 * Handle a sentence. This means that forms are extracted, and that a
	 * representation of the sentence (if error > 0.0) is kept.
	 * @param sentence The sentence tokens.
	 * @param error The error rate of this sentence (normally 0.0 or 1.0).
	 */
	void handleSentence(std::vector<std::string> const &sentence,
		double error);

	/**
	 * Mine the current data set.
	 * @param threshold The stopping threshold. If the maximal change
	 *  in suspicion is below this threshold, the mining process is
	 *  stopped.
	 * @param suspThreshold If the suspicion of a form drops below this
	 *  threshold, remove observations of this form from further
	 *  analysis.
	 */
	void mine(double threshold = 0.001, double suspThreshold = 0.0);
private:
	typedef std::pair<std::vector<int>::const_iterator,
		std::vector<int>::const_iterator> IntVecIterPair;

	Miner(Miner const &other);
	Miner &operator=(Miner const &other);
	void destroy();
	void calculateInitialFormSuspicions(double suspThreshold = 0.0);
	double calculateFormSuspicions(double suspThreshold = 0.0);
	double expansionFactor(std::vector<int>::const_iterator const &ngramBegin,
			std::vector<int>::const_iterator const &ngramEnd) const;
	Sentence expandNgrams(double error, std::vector<int> const &hashedTokens);
	void newSuspForm(IntVecIterPair const &bestNgram, Sentence *sentence);
	double ngramRatio(std::vector<int>::const_iterator const &ngramBegin,
			std::vector<int>::const_iterator const &ngramEnd) const;
	void removeLowSuspForms(double suspThreshold);
	double smootheSuspicion(double suspicion, double avgSuspicion,
			size_t suspFreq) const;
	std::vector<int> unparsableToParsableHashCodes(
			std::vector<int>::const_iterator const &unparsableNgramBegin,
			std::vector<int>::const_iterator const &unparsableNgramEnd) const;

	typedef QSet<FormPtr> FormPtrSet;

	std::tr1::shared_ptr<HashAutomaton const> d_parsableHashAutomaton;
	std::tr1::shared_ptr<HashAutomaton const> d_unparsableHashAutomaton;
	std::tr1::shared_ptr<SuffixArray<int> const> d_goodSuffixArray;
	std::tr1::shared_ptr<SuffixArray<int> const> d_badSuffixArray;
	size_t d_n;
	bool d_ngramExpansion;
	bool d_expansionFactorAlpha;
	bool d_smoothing;
	double d_smoothingBeta;
	std::auto_ptr<FormPtrSet> d_forms;
	std::auto_ptr<std::list<Sentence> > d_sentences;
	std::auto_ptr<QHash<int, double> > d_unigramRatioCache;
};

inline bool operator==(FormPtr lhs, FormPtr rhs)
{
	return *lhs.value == *rhs.value;
}

inline double SuspAtLeast::operator()(FormPtr const &formPtr)
{
	return formPtr.value->suspicion() > suspicion;
}

inline double FormPtrSuspSum::operator()(double acc, FormPtr const formPtr) const
{
	return acc + formPtr.value->suspicion();
}

inline Miner::~Miner()
{
	destroy();
}

}

#endif /*MINER_HH_*/
