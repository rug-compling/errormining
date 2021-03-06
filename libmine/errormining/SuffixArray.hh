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
 * a more fashing traditional using two vectors and a simple binary search
 * function that is aware of the indirection that is applied.
 */

#ifndef SUFFIXARRAY_HH
#define SUFFIXARRAY_HH

#include <algorithm>
#include <utility>
#include <vector>

#include <QSharedPointer>

namespace errormining {

template <typename T>
class SuffixCompare
{
	std::vector<T> const *d_sequencePtr;
public:
	SuffixCompare(std::vector<T> const *sequence) :
		d_sequencePtr(sequence) {};
	bool operator()(size_t i, size_t j) const;
	bool operator()(size_t i, std::vector<T> const &value) const;
	bool operator()(std::vector<T> const &value, size_t i) const; // Hmpf.
};

template <typename T>
class SuffixArray
{
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
	SuffixArray(QSharedPointer<std::vector<T> const> const &data) :
		d_data(data), d_suffixArray(genSuffixArray(*d_data)),
		d_compareFun(SuffixCompare<T>(d_data.get())) {}

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
	SuffixArray(QSharedPointer<std::vector<int> const> const &data,
			SortAlgorithm sortAlgorithm) :
		d_data(data),
		d_suffixArray(genSuffixArray(*d_data, sortAlgorithm)),
		d_compareFun(SuffixCompare<int>(d_data.data())) {}

	/**
	 * Recreate a suffix array from a previously created suffix array vector.
	 */
	SuffixArray(std::vector<T> const &data,
			std::vector<size_t> const &suffixArray) :
		d_data(new std::vector<T>(data)),
		d_suffixArray(new std::vector<size_t>(suffixArray)),
		d_compareFun(SuffixCompare<T>(d_data.get())) {}

	/**
	 * Copy constructor.
	 */
	SuffixArray<T>(SuffixArray<T> const &other) :
		d_data(new std::vector<T>(*(other.d_data))),
		d_suffixArray(new std::vector<size_t>(*other.d_suffixArray)),
		d_compareFun(SuffixCompare<T>(d_data.get())) {}

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
	std::vector<size_t> const *genSuffixArray(std::vector<T> const &data) const;
	std::vector<size_t> const *genSuffixArray(std::vector<int> const &data,
			SortAlgorithm sortAlgorithm) const;

	QSharedPointer<std::vector<T> const> d_data;
	QSharedPointer<std::vector<size_t> const> d_suffixArray;
	SuffixCompare<T> d_compareFun;
};

template <typename T>
SuffixArray<T> &SuffixArray<T>::operator=(SuffixArray<T> const &other)
{
	if (this != &other) {
		d_data = QSharedPointer<std::vector<T> >(new std::vector<T>(*other.d_data));
		d_suffixArray = QSharedPointer<std::vector<size_t> >(new std::vector<size_t>(*other.d_suffixArray));
		d_compareFun = SuffixCompare<T>(d_data.get());
	}

	return *this;
}

template <typename T>
std::vector<T> const &SuffixArray<T>::data() const
{
	return *d_data;
}

template <typename T>
template <typename MatchIter>
std::pair<std::vector<size_t>::const_iterator,
	std::vector<size_t>::const_iterator>
	SuffixArray<T>::find(MatchIter matchBeginIter, MatchIter matchEndIter) const
{
	std::vector<T> toMatch(matchBeginIter, matchEndIter);
	return equal_range(d_suffixArray->begin(), d_suffixArray->end(), toMatch,
		d_compareFun);
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
	std::sort(suffixArray->begin(), suffixArray->end(), d_compareFun);

	return suffixArray;
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
inline bool SuffixCompare<T>::operator()(size_t i, size_t j) const
{
	// Compare two suffixes. A suffix is a sequence from an index
	// to (potentially) the end of the array.
	return lexicographical_compare(d_sequencePtr->begin() +  i,
		d_sequencePtr->end(), d_sequencePtr->begin() + j,
		d_sequencePtr->end());
}

template <typename T>
inline bool SuffixCompare<T>::operator()(size_t i, std::vector<T> const &value) const
{
	typename std::vector<T>::const_iterator subSequenceEnd =
		std::min(d_sequencePtr->begin() + i + value.size(), d_sequencePtr->end());

	return lexicographical_compare(d_sequencePtr->begin() + i,
		subSequenceEnd, value.begin(), value.end());
}

template <typename T>
inline bool SuffixCompare<T>::operator()(std::vector<T> const &value, size_t i) const
{
	typename std::vector<T>::const_iterator subSequenceEnd =
		std::min(d_sequencePtr->begin() + i + value.size(), d_sequencePtr->end());

	return lexicographical_compare(value.begin(), value.end(),
		d_sequencePtr->begin() + i, subSequenceEnd);
}

}

#endif // SUFFIXARRAY_HH
