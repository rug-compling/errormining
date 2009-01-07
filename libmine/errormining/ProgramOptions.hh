#ifndef PROGRAM_OPTIONS_HH_
#define PROGRAM_OPTIONS_HH_

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "SuffixArray.hh"

namespace errormining
{

class ProgramOptions
{
public:
	ProgramOptions(int argc, char *argv[]);
	std::vector<std::string> const &arguments() const;
	double expansionFactorAlpha() const;
	size_t n() const;
	size_t m() const;
	size_t ngramExpansion() const;
	size_t frequency() const;
	std::string const &programName() const;
	bool smoothing() const;
	double smoothingBeta() const;
	SuffixArray<int>::SortAlgorithm sortAlgorithm();
	size_t suspFrequency() const;
	double suspThreshold() const;
	double threshold() const;
	bool verbose() const;
private:
	ProgramOptions(ProgramOptions const &other);
	ProgramOptions &operator=(ProgramOptions const &other);

	std::string d_programName;
	size_t d_n;
	size_t d_m;
	bool d_ngramExpansion;
	double d_expansionFactorAlpha;
	size_t d_frequency;
	bool d_smoothing;
	double d_smoothingBeta;
	SuffixArray<int>::SortAlgorithm d_sortAlgorithm;
	size_t d_suspFrequency;
	double d_suspThreshold;
	double d_threshold;
	bool d_verbose;
	std::auto_ptr<std::vector<std::string> > d_arguments;
};

template <typename T>
T parseString(std::string const &str)
{
	std::istringstream iss(str);
	T val;
	iss >> val;

	if (!iss)
		throw std::invalid_argument("Error parsing option argument: " + str);

	return val;
}

inline std::vector<std::string> const &ProgramOptions::arguments() const
{
	return *d_arguments;
}

inline double ProgramOptions::expansionFactorAlpha() const
{
	return d_expansionFactorAlpha;
}

inline size_t ProgramOptions::m() const
{
	return d_m;
}

inline size_t ProgramOptions::n() const
{
	return d_n;
}

inline size_t ProgramOptions::ngramExpansion() const
{
	return d_ngramExpansion;
}

inline size_t ProgramOptions::frequency() const
{
	return d_frequency;
}

inline std::string const &ProgramOptions::programName() const
{
	return d_programName;
}

inline bool ProgramOptions::smoothing() const
{
	return d_smoothing;
}

inline double ProgramOptions::smoothingBeta() const
{
	return d_smoothingBeta;
}

inline SuffixArray<int>::SortAlgorithm ProgramOptions::sortAlgorithm()
{
	return d_sortAlgorithm;
}

inline size_t ProgramOptions::suspFrequency() const
{
	return d_suspFrequency;
}

inline double ProgramOptions::suspThreshold() const
{
	return d_suspThreshold;
}

inline double ProgramOptions::threshold() const
{
	return d_threshold;
}

inline bool ProgramOptions::verbose() const
{
	return d_verbose;
}

}

#endif // PROGRAM_OPTIONS_HH_
