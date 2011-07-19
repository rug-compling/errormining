#ifndef MINER_HH_
#define MINER_HH_

#include <functional>
#include <list>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <QCache>
#include <QHash>
#include <QSet>
#include <QSharedPointer>
#include <QVector>

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
	Miner(QSharedPointer<HashAutomaton const> parsableHashAutomaton,
			QSharedPointer<HashAutomaton const> unparsableHashAutomaton,
			QSharedPointer<SuffixArray<int> const > parsableSuffixArray,
			QSharedPointer<SuffixArray<int> const > unparsableSuffixArray,
			size_t n = 1, size_t m = 1, bool ngramExpansion = true,
			double expansionFactorAlpha = 0.0, bool smoothing = true, double smoothingBeta = 0.1) :
		d_parsableHashAutomaton(parsableHashAutomaton),
		d_unparsableHashAutomaton(unparsableHashAutomaton),
		d_goodSuffixArray(parsableSuffixArray),
		d_badSuffixArray(unparsableSuffixArray), d_n(n), d_m(m),
		d_ngramExpansion(ngramExpansion), d_expansionFactorAlpha(expansionFactorAlpha),
		d_smoothing(smoothing), d_smoothingBeta(smoothingBeta),
		d_forms(new QSet<FormPtr>()),
		d_sentences(new std::list<Sentence>()),
        d_ratioCache(new QCache<QVector<int>, double>(1000000)) {}

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

	// Implementing a copy constructor and assignment operator does not really seem
	// worth the effort.
	Miner(Miner const &other);
	Miner &operator=(Miner const &other);

	// Form deallocation.
	void destroy();

	// Perform the first mining cycle.
	void calculateInitialFormSuspicions(double suspThreshold = 0.0);

	// Perform a mining cycle.
	double calculateFormSuspicions(double suspThreshold = 0.0);

	// Traditional ngram collections (add all n to m-grams).
	Sentence collectNgrams(double error, std::vector<int> const &hashedTokens);

	// Calculate the expansion factor of an n-gram.
	double expansionFactor(std::vector<int>::const_iterator const &ngramBegin,
			std::vector<int>::const_iterator const &ngramEnd) const;

	// N-gram collection with n-gram expansion.
	Sentence expandNgrams(double error, std::vector<int> const &hashedTokens);

	// Create a new suspcious form.
	void newSuspForm(IntVecIterPair const &bestNgram, Sentence *sentence);

	// Calculate 'unparsability' ratio of an n-gram (f_unparsable / f_all).
	double ngramRatio(std::vector<int>::const_iterator const &ngramBegin,
			std::vector<int>::const_iterator const &ngramEnd) const;

	// Remove forms with a suspicion below the the specified threshold.
	void removeLowSuspForms(double suspThreshold);

	// Smoothe a suspicion.
	double smootheSuspicion(double suspicion, double avgSuspicion,
			size_t suspFreq) const;

	// Translate a sequence from the unparsable corpus to the parsable corpus. This
	// is necessary, because both corpera use a different hash automaton.
	std::vector<int> unparsableToParsableHashCodes(
			std::vector<int>::const_iterator const &unparsableNgramBegin,
			std::vector<int>::const_iterator const &unparsableNgramEnd) const;

	typedef QSet<FormPtr> FormPtrSet;

	QSharedPointer<HashAutomaton const> d_parsableHashAutomaton;
	QSharedPointer<HashAutomaton const> d_unparsableHashAutomaton;
	QSharedPointer<SuffixArray<int> const> d_goodSuffixArray;
	QSharedPointer<SuffixArray<int> const> d_badSuffixArray;
	size_t d_n;
	size_t d_m;
	bool d_ngramExpansion;
	bool d_expansionFactorAlpha;
	bool d_smoothing;
	double d_smoothingBeta;
	QSharedPointer<FormPtrSet> d_forms;
	QSharedPointer<std::list<Sentence> > d_sentences;
    QSharedPointer<QCache<QVector<int>, double> > d_ratioCache;
};

inline bool operator==(FormPtr lhs, FormPtr rhs)
{
	return *lhs.value == *rhs.value;
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

template <typename T>
inline uint qHash(QVector<T> const &vec)
{
    uint seed = qHash(*(vec.begin()));
    
    // Hash a vector of values, the hash combination method is derrived
    // from Boost hash_combine().
    for (typename QVector<T>::const_iterator iter = vec.begin() + 1;
         iter != vec.end(); ++iter)
        seed ^= qHash(*iter) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    
    return seed;
}


#endif /*MINER_HH_*/
