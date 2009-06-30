#ifndef HASH_AUTOMATON_HH
#define HASH_AUTOMATON_HH

#include <stdexcept>
#include <string>

#include <QSharedPointer>

#include <fadd/fadd.h>

namespace errormining
{

/**
 * This exception is thrown when no valid automaton could be constructed
 * from a to be loaded automaton.
 */
class InvalidAutomatonException : public std::runtime_error
{
public:
	/**
	 * Construct an InvalidAutomatonException.
	 * @param what An error message explaining why the automaton could not
	 *  be constructed.
	 */
	InvalidAutomatonException(std::string const &what) :
		std::runtime_error(what) {}
};

/**
 * This class represents a perfect hash automaton, it's actually a
 * convenient wrapper around the <i>fsa</i> class from the fadd
 * library.
 */
class HashAutomaton
{
public:
	/**
	 * Construct a perfect hash automaton from an automaton created with
	 * fsa_build. The hashing and hash to word functions are implemented
	 * as overloaded operator() methods, meaning that this class can be
	 * used as a function object for generic algorithms.
	 */
	HashAutomaton(std::string const &filename);

	/**
	 * Get the hash number for a word. Returns <i>-1</i> if the word
	 * is unknown.
	 */
	int operator()(std::string const &word) const;

	/**
	 * Get the word for a given hash number.
	 */
	std::string operator()(int number) const;

	/**
	 * This method indicates whether the automaton could be read correctly.
	 */
	bool good() const;
private:
	QSharedPointer<fsa> d_fsa;
};

inline int HashAutomaton::operator()(std::string const &word) const
{
	return d_fsa->number_word(word.c_str());
}

inline std::string HashAutomaton::operator()(int number) const
{
	return d_fsa->word_number(number);
}

}

#endif // HASH_AUTOMATON_HH
