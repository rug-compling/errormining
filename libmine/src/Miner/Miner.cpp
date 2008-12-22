#include "Miner.ih"

bool FormProbComp::operator()(Form const &lhs, Form const &rhs) const
{
	if (lhs.suspicion() == rhs.suspicion())
		return lhs < rhs;
	return lhs.suspicion() > rhs.suspicion();
}

uint errormining::qHash(FormPtr const &formPtr)
{
	size_t seed = formPtr.value->ngram()[0];

	// Hash a vector of strings, the hash combination method is derrived
	// from Boost hash_combine().
	for (vector<int>::const_iterator iter = formPtr.value->ngram().begin() + 1;
			iter != formPtr.value->ngram().end(); ++iter)
		seed ^= *iter + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

void Miner::destroy()
{
	for (FormPtrSet::const_iterator formIter = d_forms->begin();
			formIter != d_forms->end(); ++formIter)
		delete formIter->value;
}

void Miner::calculateInitialFormSuspicions(double suspThreshold)
{
	QHash<Form *, double> formSuspSums;

	// Calculate the initial observation suspicions.
	for (QVector<Sentence>::const_iterator sentenceIter = d_sentences->begin();
		sentenceIter != d_sentences->end(); ++sentenceIter)
	{
		for (Sentence::const_iterator formIter = sentenceIter->begin();
			formIter != sentenceIter->end(); ++formIter)
		{
			// The initial suspicions of observations are uniformly divided over
			// the observations within a sentence.
			double suspicion = sentenceIter->error() /
				sentenceIter->observedForms().size();
			formSuspSums[const_cast<Form *>(*formIter)] += suspicion;
		}
	}

	// Calculate the initial form suspicions.
	for (QHash<Form *, double>::const_iterator formIter =
		formSuspSums.begin(); formIter != formSuspSums.end(); ++formIter)
	{
		// The suspicion of a form is the average of all suspicions of
		// observations of the form. Since all observations within parsable
		// sentences have a suspicion of 0.0, they don't add to the sum,
		// only the total number of observations.
		double suspicion = formIter.value() / formIter.key()->nSuspObservations();
		formIter.key()->setSuspicion(suspicion);
	}

	if (d_smoothing)
	{
		double suspicionSum = accumulate(d_forms->begin(), d_forms->end(), 0.0,
				FormPtrSuspSum());
		double avgSuspicion = suspicionSum / d_forms->size();

		for (FormPtrSet::const_iterator iter = d_forms->begin();
				iter != d_forms->end(); ++iter)
		{
			double smoothedSuspicion = smootheSuspicion(iter->value->suspicion(),
					avgSuspicion, iter->value->nSuspObservations());
			iter->value->setSuspicion(smoothedSuspicion);
		}
	}

	// If a suspicion threshold is used, remove all observations that
	// dropped below this threshold. This speeds up the mining process
	// considerably, and with a near-zero value it is not likely to
	// do harm.
	if (suspThreshold > 0.0)
		removeLowSuspForms(suspThreshold);
}

double Miner::calculateFormSuspicions(double suspThreshold)
{
	QHash<Form *, double> formSuspSums;
	QHash<Form *, double> oldSusps;

	// Calculate suspicions of observations of a form within a sentence.
	for (QVector<Sentence>::const_iterator sentenceIter = d_sentences->begin();
		sentenceIter != d_sentences->end(); ++sentenceIter)
	{
		double sentenceSuspSum = 0.0;

		// Get the sum of all observations within a sentence for sentence-level.
		for (Sentence::const_iterator formIter = sentenceIter->begin();
				formIter != sentenceIter->end(); ++formIter)
			sentenceSuspSum += (*formIter)->suspicion();

		for (Sentence::const_iterator formIter = sentenceIter->begin();
			formIter != sentenceIter->end(); ++formIter)
		{
			// Calculate the suspicion of an observation, which is the suspicion of
			// the form with sentence-level normalization.
			double suspicion = sentenceIter->error() *
				((*formIter)->suspicion() / sentenceSuspSum);
			formSuspSums[const_cast<Form *>(*formIter)] += suspicion;
		}
	}

	for (QHash<Form *, double>::const_iterator formIter =
		formSuspSums.begin(); formIter != formSuspSums.end(); ++formIter)
	{
		// The suspicion of a form is the average of all suspicions of
		// observations of the form. Since all observations within parsable
		// sentences have a suspicion of 0.0, they don't add to the sum,
		// only the total number of observations.
		double suspicion = formIter.value() / formIter.key()->nObservations();

		oldSusps[formIter.key()] = formIter.key()->suspicion();
		formIter.key()->setSuspicion(suspicion);
	}

	if (d_smoothing)
	{
		// Calculate the average pre-smoothing suspicion for this cycle.
		double suspicionSum = accumulate(d_forms->begin(), d_forms->end(), 0.0,
				FormPtrSuspSum());
		double avgSuspicion = suspicionSum / d_forms->size();

		for (FormPtrSet::const_iterator iter = d_forms->begin();
				iter != d_forms->end(); ++iter)
		{
			// Smoothe the suspicion.
			double smoothedSuspicion = smootheSuspicion(iter->value->suspicion(),
					avgSuspicion, iter->value->nSuspObservations());
			iter->value->setSuspicion(smoothedSuspicion);
		}
	}

	// Check the suspicions delta for each form, and store it, if it is
	// the highest delta that we have seen. The caller can use the highest
	// delta of a learning cycle to determine when to stop mining.
	double maxDelta = 0.0;
	for (QHash<Form *, double>::const_iterator iter = oldSusps.begin();
			iter != oldSusps.end(); ++iter)
	{
		double delta = abs(iter.value() - iter.key()->suspicion());
		if (delta > maxDelta)
			maxDelta = delta;
	}

	// If a suspicion threshold is used, remove all observations that
	// dropped below this threshold. This speeds up the mining process
	// considerably, and with a near-zero value it is not likely to
	// do harm.
	if (suspThreshold > 0.0)
		removeLowSuspForms(suspThreshold);

	return maxDelta;
}

double Miner::expansionFactor(std::vector<int>::const_iterator const &ngramBegin,
		std::vector<int>::const_iterator const &ngramEnd) const
{
	SuffixArray<int>::IterPair badIters = d_badSuffixArray->find(ngramBegin,
			ngramEnd);
	size_t badFreq = distance(badIters.first, badIters.second);

	return 1 + exp(-d_expansionFactorAlpha * static_cast<double>(badFreq));
}


set<Form, FormProbComp> Miner::forms() const
{
	set<Form, FormProbComp> forms;

	// Copy all forms to a set that is ordered by descending suspicion.
	for (FormPtrSet::const_iterator formIter = d_forms->begin();
			formIter != d_forms->end(); ++formIter)
		forms.insert(*(formIter->value));

	return forms;
}

void Miner::handleSentence(vector<string> const &tokens, double error)
{
	// The prescanner can give the necessary frequency information for
	// occurances of forms in parsable sentences.
	if (error == 0.0)
		return;

	Sentence sentence(error);

	vector<int> hashedTokens;
	transform(tokens.begin(), tokens.end(), back_inserter(hashedTokens),
			*d_unparsableHashAutomaton);

	for (vector<int>::const_iterator iter = hashedTokens.begin();
		iter + (d_n - 1) < hashedTokens.end(); ++iter)
	{
		// Initially, the best n-gram is the shortest one we start with.
		// 'Best n-gram' is defined as the n-gram with the highest
		// unparsable:parsable ratio.
		IntVecIterPair bestNgram(iter, iter + d_n);

		if (d_ngramExpansion)
		{
			double bestNgramRatio = ngramRatio(bestNgram.first, bestNgram.second);

			for (vector<int>::const_iterator endIter = iter + d_n;
				endIter != hashedTokens.end(); ++endIter)
			{
				// Get the n+1-gram, which we will call a m-gram.
				IntVecIterPair mgram(iter, endIter + 1);
				double mgramRatio = ngramRatio(mgram.first, mgram.second);

				// Get the second n-gram within the current m-gram.
				IntVecIterPair ngram(iter + 1, endIter + 1);

				// Calculate the expansion factor, if it is senseful.
				double factor = 1.0;
				if (d_expansionFactorAlpha != 0.0)
					factor = expansionFactor(mgram.first, mgram.second);

				// If the m-gram has a worse ratio (in terms of relatively more
				// occurrances in unparsable sentences), we'll extend the n-gram.
				// The ratio of 'ngram' is not precalculated - the evaluation is
				// shortwired if mgramRatio < bestNgramRatio, so we may not
				// actually need it.
				if (mgramRatio > factor * bestNgramRatio
						&& mgramRatio > factor * ngramRatio(ngram.first, ngram.second))
				{
					bestNgram = mgram;
					bestNgramRatio = mgramRatio;
				}
				else
					break;
			}
		}

		newSuspForm(bestNgram, &sentence);
	}

	d_sentences->push_back(sentence);
}

void Miner::mine(double threshold, double suspThreshold)
{
	// Initial form suspicion calculation.
	calculateInitialFormSuspicions(suspThreshold);

	// Cycle until the fixed-point is reached.
	while (calculateFormSuspicions(suspThreshold) > threshold) { notify(); }
	notify();
}

void Miner::newSuspForm(IntVecIterPair const &bestNgram, Sentence *sentence)
{
	vector<int> bestNgramVec(bestNgram.first, bestNgram.second);

	Form form(bestNgramVec);
	FormPtr formPtr(&form);

	// Check whether we have seen the current form before, if not, we'll
	// want to add it if the form is of interest to us.
	FormPtrSet::const_iterator formIter = d_forms->find(formPtr);
	if (formIter == d_forms->end()) {
		vector<int> parsableBestNgram = unparsableToParsableHashCodes(
				bestNgramVec.begin(), bestNgramVec.end());

		// Look up the number of unsuspicious observations of the ngram
		// with the highest unparsable:parsable ratio.
		SuffixArray<int>::IterPair goodIters =
			d_goodSuffixArray->find(parsableBestNgram.begin(),
					parsableBestNgram.end());
		size_t unsuspObservations = distance(goodIters.first, goodIters.second);

		formPtr.value = new Form(bestNgramVec, 0.0, unsuspObservations);
		d_forms->insert(formPtr);
		formIter = d_forms->find(formPtr);
	}

	// Store observations of the Form in a sentence-representation. This
	// is used by the miner to calculate observations suspicions.
	sentence->addObservedForm(formIter->value);
	formIter->value->newSuspObservation();
}

double Miner::ngramRatio(
		std::vector<int>::const_iterator const &ngramBegin,
		std::vector<int>::const_iterator const &ngramEnd) const
{
	// See if we have cached the ratio if this is a unigram.
	if (distance(ngramBegin, ngramEnd) == 1)
	{
		QHash<int, double>::const_iterator iter =
			d_unigramRatioCache->find(*ngramBegin);
		if (iter != d_unigramRatioCache->end())
			return iter.value();
	}

	// Since a different hashing function is used for parsable sentences,
	// we'll have to convert the n-gram in unparsable hash codes to
	// parsable hash codes.
	vector<int> parsableNgram = unparsableToParsableHashCodes(ngramBegin,
			ngramEnd);

	SuffixArray<int>::IterPair goodIters = d_goodSuffixArray->find(parsableNgram.begin(),
			parsableNgram.end());
	size_t goodFreq = distance(goodIters.first, goodIters.second);

	SuffixArray<int>::IterPair badIters = d_badSuffixArray->find(ngramBegin,
			ngramEnd);
	size_t badFreq = distance(badIters.first, badIters.second);

	double ratio = static_cast<double>(badFreq) / (goodFreq + badFreq);

	// Cache if this was a unigram.
	if (distance(ngramBegin, ngramEnd) == 1)
		(*d_unigramRatioCache)[*ngramBegin] = ratio;

	return ratio;
}

void Miner::removeLowSuspForms(double suspThreshold)
{
	// Remove all observations of a form that have a near-zero suspicion.
	for (QVector<Sentence>::iterator sentenceIter = d_sentences->begin();
		sentenceIter != d_sentences->end(); ++sentenceIter)
	{
		Sentence &sentence = *sentenceIter;
		sentence.erase(remove_if(sentence.begin(), sentence.end(),
			FormSuspicionSmallerThan(suspThreshold)), sentence.end());
	}

	// Remove all forms with a near-zero suspicion.
	FormPtrSet::iterator iter = d_forms->begin();
	while (iter != d_forms->end())
	{
		if (iter->value->suspicion() < suspThreshold)
			iter = d_forms->erase(iter);
		else
			++iter;
	}
	//blockingFilter(d_forms, SuspAtLeast(suspThreshold));
}

double Miner::smootheSuspicion(double suspicion, double avgSuspicion,
		size_t suspFreq) const
{
	// Smoothing function.
	double lambda = 1.0 - exp(-d_smoothingBeta * suspFreq);

	// Smoothe, by increasing bias for the form suspicion when there are
	// more suspicious observations of that form.
	return lambda * suspicion + (1 - lambda) * avgSuspicion;
}


std::vector<int> Miner::unparsableToParsableHashCodes(
		std::vector<int>::const_iterator const &unparsableNgramBegin,
		std::vector<int>::const_iterator const &unparsableNgramEnd) const
{
	// Since different hash automata are used for parsable and unparsable
	// sentences, we often need to convert the hashcode for a token from
	// a 'parsable hashcode' to an 'unparsable hashcode'.

	// Convert from unparsable hash codes to strings.
	vector<string> stringNgram;
	transform(unparsableNgramBegin, unparsableNgramEnd,
			back_inserter(stringNgram), *d_unparsableHashAutomaton);

	// Convert from strings to parsable hash codes.
	vector<int> parsableNgram;
	transform(stringNgram.begin(), stringNgram.end(), back_inserter(parsableNgram),
			*d_parsableHashAutomaton);

	return parsableNgram;
}
