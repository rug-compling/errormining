#ifndef SCORING_HH 
#define SCORING_HH

#include <cmath>

#include <QSharedPointer>

namespace errormining {

/**
 * Scoring methods.
 */
enum ScoringMethod { SCORING_SUSP, SCORING_SUSP_OBS, SCORING_SUSP_UNIQSENTS,
	SCORING_SUSP_LN_OBS, SCORING_SUSP_LN_UNIQSENTS };

struct ScoreFun
{
	virtual ~ScoreFun() {}
	virtual double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const = 0;
};

struct ScoreSusp : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspObs : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspUniqSents : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspLnObs : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspLnUniqSents : public ScoreFun
{
	double operator()(double suspicion, size_t suspFreq, size_t uniqSentsFreq) const;
};

QSharedPointer<ScoreFun> selectScoreFun(ScoringMethod scoringMethod);

inline double ScoreSusp::operator()(double suspicion, size_t, size_t) const
{
	return suspicion;
}

inline double ScoreSuspObs::operator()(double suspicion, size_t suspFreq, size_t) const
{
	return suspicion * suspFreq;
}

inline double ScoreSuspUniqSents::operator()(double suspicion, size_t, size_t uniqSentsFreq) const
{
	return suspicion * uniqSentsFreq;
}

inline double ScoreSuspLnObs::operator()(double suspicion, size_t suspFreq, size_t) const
{
	return suspicion * log(suspFreq);
}

inline double ScoreSuspLnUniqSents::operator()(double suspicion, size_t, size_t uniqSentsFreq) const
{
	return suspicion * log(uniqSentsFreq);
}

}

#endif // SCORING_HH
