#ifndef _FORM_HH
#define _FORM_HH

#include <iostream>
#include <memory>
#include <vector>

namespace errormining
{

/**
 * This class represents a form, which is normally an n-gram.
 */
class Form
{
public:
	/**
	 * Construct a form from an n-gram.
	 *
	 * @param ngram The ngram represented as a vector of strings.
	 * @param suspicion The initial suspicion of this form.
	 */
	Form(std::vector<int> const &ngram, double suspicion = 0.0,
			size_t unsuspObservations = 0, size_t suspObservations = 0)
		: d_ngram(new std::vector<int>(ngram)),
		d_suspicion(suspicion), d_unsuspObservations(unsuspObservations),
		d_suspObservations(suspObservations)
		{}

	Form(Form const &other);
	Form &operator=(Form const &other);
	bool operator==(Form const &rhs) const;
	bool operator<(Form const &rhs) const;

	/**
	 * Return the total number of observations (suspicious and unsuspicous)
	 * of this form.
	 */
	size_t nObservations() const;

	/**
	 * Return the number of suspicious observations.
	 */
	size_t nSuspObservations() const;

	/**
	 * Return the number of unsuspicious observations.
	 */
	size_t nUnsuspObservations() const;

	/**
	 * Register a suspicious observation.
	 */
	void newSuspObservation();

	/**
	 * Register an unsuspicious observation.
	 */
	void newUnsuspObservation();


	/**
	 * Return the n-gram this form represents.
	 */
	std::vector<int> const &ngram() const;

	/**
	 * Register the removal of an suspicious observation.
	 */
	void removeSuspObservation();

	/**
	 * Set the suspicion of this form.
	 */
	void setSuspicion(double suspicion);

	/**
	 * Return the suspicioun of this form.
	 */
	double suspicion() const;
private:
	void copy(Form const &other);

	std::auto_ptr<std::vector<int> > d_ngram;
	double d_suspicion;
	size_t d_unsuspObservations;
	size_t d_suspObservations;
};

/**
 * This is a small function object class, that returns true when the
 * suspicion of a form is smaller than a certain value.
 */
class FormSuspicionSmallerThan
{
public:
	/**
	 * Construct an instance with the value that this function object
	 * should check against.
	 */
	FormSuspicionSmallerThan(double value) : d_value(value) {}
	bool operator()(Form const *form) const;
private:
	double d_value;
};

std::ostream &operator<<(std::ostream &out, Form const &form);

inline bool Form::operator==(Form const &rhs) const {
	return *d_ngram == *rhs.d_ngram;
}

inline bool Form::operator<(Form const &rhs) const {
	return *d_ngram < *rhs.d_ngram;
}

inline Form::Form(Form const &other)
{
	copy(other);
}

inline size_t Form::nObservations() const
{
	return d_unsuspObservations + d_suspObservations;
}

inline size_t Form::nSuspObservations() const
{
	return d_suspObservations;
}

inline size_t Form::nUnsuspObservations() const
{
	return d_unsuspObservations;
}

inline void Form::newSuspObservation()
{
	++d_suspObservations;
}

inline void Form::newUnsuspObservation()
{
	++d_unsuspObservations;
}

inline std::vector<int> const &Form::ngram() const
{
	return *d_ngram;
}

inline void Form::removeSuspObservation()
{
	--d_suspObservations;
}

inline void Form::setSuspicion(double suspicion)
{
	d_suspicion = suspicion;
}

inline double Form::suspicion() const
{
	return d_suspicion;
}

inline bool FormSuspicionSmallerThan::operator()(Form const *form) const
{
	return form->suspicion() < d_value;
}

}

#endif // _FORM_HH
