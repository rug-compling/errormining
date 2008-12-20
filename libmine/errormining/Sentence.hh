#ifndef SENTENCE_HH_
#define SENTENCE_HH_

#include "Form.hh"

#include <memory>
#include <vector>

namespace errormining
{

/**
 * This class represents a sentence as a sequence of forms.
 */
class Sentence
{
public:
	typedef std::vector<Form const *>::const_iterator const_iterator;
	typedef std::vector<Form const *>::iterator iterator;

	/**
	 * Construct a sentence.
	 * @param error The error rate of the sentence, typically 0.0 (parsable)
	 *  or 1.0 (unparsable).
	 */
	Sentence(double error = 0.0) : d_error(error),
		d_forms(new std::vector<Form const *>()) {}

	Sentence(Sentence const &other);

	Sentence &operator=(Sentence const &other);

	/**
	 * Add a form that was observed in this sentence.
	 */
	void addObservedForm(Form const *observedForm);
	
	const_iterator begin() const;
	iterator begin();
	const_iterator end() const;
	iterator end();
	iterator erase(iterator loc);
	iterator erase(iterator start, iterator end);
	
	/**
	 * Return the sentence error rate.
	 */
	double error() const;
	
	/**
	 * Return the forms observed in this sentence.
	 */
	std::vector<Form const *> const &observedForms() const;
private:
	void copy(Sentence const &other);

	double d_error;
	std::auto_ptr<std::vector<Form const *> > d_forms;
};

inline Sentence::Sentence(Sentence const &other)
{
	copy(other);
}

inline void Sentence::addObservedForm(Form const *observedForm)
{
	d_forms->push_back(observedForm);
}

inline Sentence::const_iterator Sentence::begin() const
{
	return d_forms->begin();
}

inline Sentence::iterator Sentence::begin()
{
	return d_forms->begin();
}

inline Sentence::const_iterator Sentence::end() const
{
	return d_forms->end();
}

inline Sentence::iterator Sentence::end()
{
	return d_forms->end();
}

inline Sentence::iterator Sentence::erase(iterator loc)
{
	return d_forms->erase(loc);
}

inline Sentence::iterator Sentence::erase(iterator start, iterator end)
{
	return d_forms->erase(start, end);
}

inline double Sentence::error() const
{
	return d_error;
}

inline std::vector<Form const *> const &Sentence::observedForms() const
{
	return *d_forms;
}

}

#endif /*SENTENCE_HH_*/
