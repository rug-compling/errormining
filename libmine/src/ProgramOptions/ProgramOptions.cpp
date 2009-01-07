#include "ProgramOptions.ih"

ProgramOptions::ProgramOptions(int argc, char *argv[])
	: d_n(1), d_m(1), d_ngramExpansion(true), d_expansionFactorAlpha(0.0),
	d_frequency(2), d_smoothing(false), d_smoothingBeta(0.1),
	d_sortAlgorithm(SuffixArray<int>::SSORT), d_suspFrequency(0),
	d_suspThreshold(0.0), d_threshold(0.001), d_verbose(false),
	d_arguments(new vector<string>())
{
	d_programName = argv[0];

	// We will do our own error reporting.
	opterr = 0;

	int opt;
	while ((opt = getopt(argc, argv, "b:ce:f:m:n:o:s:t:u:v")) != -1)
	{
		switch (opt)
		{
		case 'b':
			d_smoothing = true;
			d_smoothingBeta = parseString<double>(optarg);
			break;
		case 'c':
			d_ngramExpansion = false;
			break;
		case 'e':
			d_expansionFactorAlpha = parseString<double>(optarg);
			break;
		case 'f':
			d_frequency = parseString<size_t>(optarg);
			break;
		case 'm':
			d_m = parseString<size_t>(optarg);
			break;
		case 'n':
			d_n = parseString<size_t>(optarg);
			break;
		case 'o':
			{
				string algo(optarg);
				if (algo == "stlsort")
					d_sortAlgorithm = SuffixArray<int>::STLSORT;
				else if (algo != "ssort")
					throw string("Unknown suffix sorting algorithm: " + algo);
			}
			break;
		case 's':
			d_suspThreshold = parseString<double>(optarg);
			break;
		case 't':
			d_threshold = parseString<double>(optarg);
			break;
		case 'u':
			d_suspFrequency = parseString<size_t>(optarg);
			break;
		case 'v':
			d_verbose = true;
			break;
		case ':':
			throw string("Missing option argument for: -") +
				static_cast<char>(optopt);
			break;
		default:
			throw string("Unknown option: -") + static_cast<char>(optopt);
		}
	}

	copy(argv + optind, argv + argc, back_inserter(*d_arguments));
}
