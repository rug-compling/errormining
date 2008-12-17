#include "Sentence.ih"

Sentence &Sentence::operator=(Sentence const &other)
{
	if (this != &other)
		copy(other);
	
	return *this;
}

void Sentence::copy(Sentence const &other)
{
	d_error = other.d_error;
	d_forms.reset(new vector<Form const *>(*other.d_forms));
}
