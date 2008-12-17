#include "HashAutomaton.ih"

HashAutomaton::HashAutomaton(string const &filename) :
	d_fsa(new fsa(filename.c_str()))
{
	// operator() of fsa returns the automaton state, which is 0 if the
	// automaton could not be initialized correctly.
	if (!*d_fsa)
		throw InvalidAutomatonException("Could not load automaton!");

	// Some fields of fsa_arc_ptr, such as entryl are static, apparently
	// the value is determined by the automaton that was loaded last (?).
	// At any rate, we can use it to check if the loaded automaton is
	// a perfect hash (number) automaton.
	fsa_arc_ptr test;
	if (test.entryl == 0)
		throw InvalidAutomatonException("Automaton is not a perfect hash automaton!");
}
