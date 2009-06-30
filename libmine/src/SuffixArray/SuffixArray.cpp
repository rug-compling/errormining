#include "SuffixArray.ih"

namespace errormining
{

template <>
vector<size_t> const *SuffixArray<int>::genSuffixArray(
		vector<int> const &data, SortAlgorithm sortAlgorithm) const
{
	if (sortAlgorithm == STLSORT)
	{
		vector<size_t> *suffixArray = new std::vector<size_t>();

		// Initially fill a vector where the i-th element represents the
		// suffix starting at index i. This is an unsorted suffix array.
		for (size_t i = 0; i < data.size(); ++i)
			suffixArray->push_back(i);

		// Sort the suffix array.
		sort(suffixArray->begin(), suffixArray->end(), d_compareFun);

		return suffixArray;
	}

	// The caller does not want us to use STL sort, so use the suffix
	// sort algorithm by McIlroy and McIlroy.

	// ssort initially requires the original sequence of suffixes.
	QSharedPointer<vector<int> > suffixArray(new vector<int>(data));

	// The hash automaton returns [0..k-1] for k different words. ssort
	// uses 0 as an end of sequence marker and thus expects hashcodes
	// [1..k]. We can simply add 1 to all hash codes.
	transform(suffixArray->begin(), suffixArray->end(), suffixArray->begin(),
			bind2nd(plus<int>(), 1));

	// Add 0 to delimit the sequence.
	suffixArray->push_back(0);

	// Create the suffix array. While we'll pass the data array, ssort
	// will modify it to be the suffix array.
	errormining::util::ssort(suffixArray.data());

	suffixArray->pop_back();

	// ssort works on a vector of ints (amongst others because the algorithm
	// internally uses the sign bit), while the suffix array class uses a
	// vector of size_t as indexes into the data array. So, we'll convert
	// the vector.
	vector<size_t> *sizeTSuffixArray =
		new vector<size_t>(suffixArray->begin(), suffixArray->end());

	return sizeTSuffixArray;
}

}
