#include "Form.ih"

Form &Form::operator=(Form const &other)
{
	if (this != &other)
		copy(other);

	return *this;
}

ostream &errormining::operator<<(ostream &out, Form const &form)
{
	vector<int> ngram = form.ngram();
	std::copy(ngram.begin(), ngram.end(), ostream_iterator<int>(out, " "));
	out << form.suspicion() << " " << form.nObservations() << " " <<
		form.nSuspObservations();
	return out;
}

void Form::copy(Form const &other)
{
	d_ngram.reset(new vector<int>(*other.d_ngram));
	d_suspicion = other.d_suspicion;
	d_unsuspObservations = other.d_unsuspObservations;
	d_suspObservations = other.d_suspObservations;
}
