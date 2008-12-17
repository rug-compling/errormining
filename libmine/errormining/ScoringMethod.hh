#ifndef SCORING_HH 
#define SCORING_HH

#include <cmath>

#include <tr1/memory>

namespace errormining {

/**
 * Scoring methods.
 */
enum ScoringMethod { SCORING_SUSP, SCORING_SUSP_OBS, SCORING_SUSP_UNIQSENTS,
	SCORING_SUSP_LN_OBS, SCORING_SUSP_LN_UNIQSENTS };

struct ScoreFun
{
	virtual ~ScoreFun() {}
	virtual double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) = 0;
};

struct ScoreSusp : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq);
};

struct ScoreSuspObs : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq);
};

struct ScoreSuspUniqSents : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq);
};

struct ScoreSuspLnObs : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq);
};

struct ScoreSuspLnUniqSents : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq);
};

std::tr1::shared_ptr<ScoreFun> selectScoreFun(ScoringMethod scoringMethod);

inline double ScoreSusp::operator()(double suspicion, size_t, size_t)
{
	return suspicion;
}

inline double ScoreSuspObs::operator()(double suspicion, size_t suspFreq, size_t)
{
	return suspicion * suspFreq;
}

inline double ScoreSuspUniqSents::operator()(double suspicion, size_t, size_t uniqSentsFreq)
{
	return suspicion * uniqSentsFreq;
}

inline double ScoreSuspLnObs::operator()(double suspicion, size_t suspFreq, size_t)
{
	return suspicion * log(suspFreq);
}

inline double ScoreSuspLnUniqSents::operator()(double suspicion, size_t, size_t uniqSentsFreq)
{
	return suspicion * log(uniqSentsFreq);
}

}

#endif // SCORING_HH
