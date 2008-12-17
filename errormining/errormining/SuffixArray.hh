/*
 * Copyright (c) 2008 Daniel de Kok
 *
 * This file used to be part of langkit, my personal collection of
 * NLP-related utility classes/functions.
 *
 * This software is software is dual-licensed under the following
 * licenses:
 *
 * - Apache License, Version 2.0 (compatible with the GPLv3)
 * - GNU Lesser General Public License 2.1
 *
 * Which means you can use either of these licenses (or keep this
 * notice as-is).
 */

/*
 * This class implements a suffix array. I have previously written this
 * more elegantly, relying more on STL and Boost. Unfortunately, it was
 * too slow on larger arrays to be practical (the iterators and iterator
 * transformations made it slow). I have decided to reimplement this in
 * with a more traditional using two vectors and a simple binary search
 * function that is aware of the indirection that is applied.
 */

#ifndef SUFFIXARRAY_HH
#define SUFFIXARRAY_HH

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <tr1/memory>

namespace errormining {

template <typename T>
class SuffixArray
{
	class SuffixCompare
	{
		std::vector<T> const *d_sequencePtr;
	public:
		SuffixCompare(std::vector<T> const *sequence) :
			d_sequencePtr(sequence) {};
		bool operator()(size_t i, size_t j) const;
	};
	typedef std::vector<size_t>::const_iterator SizeTVectorIter;
public:
	typedef std::pair<SizeTVectorIter, SizeTVectorIter> IterPair;

	enum SortAlgorithm {STLSORT, SSORT};

	/**
	 * Construct a suffix array.
	 *
	 * @param data This vector will be copied, and used as the backing vector
	 *  for the suffix array.
	 */
	SuffixArray(std::tr1::shared_ptr<std::vector<T> const> const &data) :
		d_data(data), d_suffixArray(genSuffixArray(*d_data)) {}

	/**
	 * Specialized constructor for data arrays of type <i>vector&lt;int&gt;</i>.
	 * Normally, this class will use the STL sort generic algorithm, but if the
	 * data array consists of <i>int</i>s within the range 1..k, and each of
	 * of the numbers within the range occurs at least once, a specialized
	 * suffix sorting algorithm by McIlroy and McIlroy can be used. This
	 * algorithm is considerably faster, but has these strict requirements.
	 *
	 * @param data This vector will be copied, and used as the backing vector
	 *  for the suffix array.
	 */
	SuffixArray(std::tr1::shared_ptr<std::vector<int> const> const &data,
			SortAlgorithm sortAlgorithm) :
		d_data(data),
		d_suffixArray(genSuffixArray(*d_data, sortAlgorithm)) {}

	/**
	 * Recreate a suffix array from a previously created suffix array vector.
	 */
	SuffixArray(std::vector<T> const &data,
			std::vector<size_t> const &suffixArray) :
		d_data(new std::vector<T>(data)),
		d_suffixArray(new std::vector<size_t>(suffixArray)) {}

	/**
	 * Copy constructor.
	 */
	SuffixArray<T>(SuffixArray<T> const &other) :
		d_data(new std::vector<T>(*(other.d_data))),
		d_suffixArray(new std::vector<size_t>(*other.d_suffixArray)) {}

	SuffixArray &operator=(SuffixArray<T> const &other);

	/**
	 * Get the data array associated with this Suffix array.
	 */
	std::vector<T> const &data() const;

	/**
	 * Find a subsequence within the suffix array. If the subsequence
	 * could be found, the pair of iterators that to the suffix array
	 * that point to the first and last match of the subsequence.
	 *
	 * @param matchBeginIter Iterator pointing to the first element of
	 *  the subsequence to be searched.
	 * @param matchEndIter Iterator pointing to one element beyond the
	 *  last element of the subsequence to be searched.
	 */
	template <typename MatchIter>
	IterPair find(MatchIter matchBeginIter, MatchIter matchEndIter) const;

	/**
	 * Return the size of the suffix array.
	 */
	std::vector<size_t>::size_type size();

	/**
	 * Return the internal suffix array, which is a vector with indexes
	 * into the data vector, sorted by the sequences starting at the
	 * indexes.
	 */
	std::vector<size_t> const &suffixArray() const;
private:
	template <typename MatchIter>
	int compare(std::vector<size_t>::const_iterator element,
		MatchIter suffixBeginIter, MatchIter suffixEndIter) const;
	std::vector<size_t> const *genSuffixArray(std::vector<T> const &data) const;
	std::vector<size_t> const *genSuffixArray(std::vector<int> const &data,
			SortAlgorithm sortAlgorithm) const;
	template <typename MatchIter>
	SizeTVectorIter lower_bound(std::vector<size_t>::const_iterator start,
		MatchIter matchBeginIter, MatchIter matchEndIter) const;
	template <typename MatchIter>
	SizeTVectorIter upper_bound(std::vector<size_t>::const_iterator end,
		MatchIter matchBeginIter, MatchIter matchEndIter) const;

	std::tr1::shared_ptr<std::vector<T> const> d_data;
	std::auto_ptr<std::vector<size_t> const> d_suffixArray;
};

template <typename T>
SuffixArray<T> &SuffixArray<T>::operator=(SuffixArray<T> const &other)
{
	if (this != &other) {
		d_data.reset(new std::vector<T>(*other.d_data));
		d_suffixArray.reset(new std::vector<size_t>(*other.d_suffixArray));
	}

	return *this;
}

template <typename T>
std::vector<T> const &SuffixArray<T>::data() const
{
	return *data;
}

template <typename T>
template <typename MatchIter>
std::pair<std::vector<size_t>::const_iterator,
	std::vector<size_t>::const_iterator>
	SuffixArray<T>::find(MatchIter matchBeginIter, MatchIter matchEndIter) const
{
	// The whole array starts out as the search space.
	std::vector<size_t>::const_iterator low = d_suffixArray->begin();
	std::vector<size_t>::const_iterator high = d_suffixArray->end() - 1;
	std::vector<size_t>::const_iterator mid;

	while(low <= high)
	{
		// Peek at the element half-way our search space.
		mid = low + std::distance(low, high) / 2;

		// How does the current middle compare to the suffix we are looking for?
		int diff = compare(mid, matchBeginIter, matchEndIter);

		// Adjust search space.
		if (diff > 0)
			high = mid - 1;
		else if (diff < 0)
			low = mid + 1;
		else
			break;
	}

	if (low > high)
		return std::make_pair(d_suffixArray->end(), d_suffixArray->end());

	return std::make_pair(lower_bound(mid, matchBeginIter, matchEndIter),
		upper_bound(mid, matchBeginIter, matchEndIter));
}

template <typename T>
std::vector<size_t> const *SuffixArray<T>::genSuffixArray(
		std::vector<T> const &data) const
{
	std::vector<size_t> *suffixArray = new std::vector<size_t>();

	// Initially fill a vector where the i-th element represents the
	// suffix starting at index i. This is an unsorted suffix array.
	for (size_t i = 0; i < data.size(); ++i)
		suffixArray->push_back(i);

	// Sort the suffix array.
	SuffixCompare suffixCompare(&data);
	std::sort(suffixArray->begin(), suffixArray->end(), suffixCompare);

	return suffixArray;
}

template <typename T>
template <typename MatchIter>
std::vector<size_t>::const_iterator
	SuffixArray<T>::lower_bound(std::vector<size_t>::const_iterator start,
	MatchIter matchBeginIter, MatchIter matchEndIter) const
{
	// Scan to the start of the vector.
	while (--start >= d_suffixArray->begin() &&
	compare(start, matchBeginIter, matchEndIter) == 0) {}

	return start + 1;
}

template <typename T>
std::vector<size_t>::size_type SuffixArray<T>::size()
{
	return d_suffixArray->size();
}

template <typename T>
std::vector<size_t> const &SuffixArray<T>::suffixArray() const
{
	return *d_suffixArray;
}

template <typename T>
template <typename MatchIter>
int SuffixArray<T>::compare(std::vector<size_t>::const_iterator element,
	MatchIter suffixBeginIter, MatchIter suffixEndIter) const
{
	typename std::vector<T>::const_iterator dataIter = d_data->begin() + *element;
	MatchIter suffixIter = suffixBeginIter;

	// Try to match all in elements in the suffix at the data
	// array element pointed to by element.
	while(suffixIter != suffixEndIter)
	{
		// Avoid crossing the end of the array. If we can not get
		// a suffix of the length of the suffix that we are interested
		// in, this specific suffix is lexicographically before the
		// potential match.
		if (dataIter == d_data->end())
			return -1;

		if (*dataIter != *suffixIter) {
			if (*dataIter > *suffixIter)
				return 1;
			else
				return -1;
		}

		++dataIter;
		++suffixIter;
	}

	return 0;
}

template <typename T>
template <typename MatchIter>
std::vector<size_t>::const_iterator
	SuffixArray<T>::upper_bound(std::vector<size_t>::const_iterator end,
	MatchIter matchBeginIter, MatchIter matchEndIter) const
{
	// Scan down
	while (++end < d_suffixArray->end() &&
		compare(end, matchBeginIter, matchEndIter) == 0) {}

	return end;
}

template <typename T>
bool SuffixArray<T>::SuffixCompare::operator()(size_t i, size_t j) const
{
	// Compare two suffixes. A suffix is a sequence from an index
	// to (potentially) the end of the array.
	return lexicographical_compare(d_sequencePtr->begin() +  i,
		d_sequencePtr->end(), d_sequencePtr->begin() + j,
		d_sequencePtr->end());
}

}

#endif // SUFFIXARRAY_HH
