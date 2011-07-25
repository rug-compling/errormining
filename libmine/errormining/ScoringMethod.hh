#ifndef SCORING_HH 
#define SCORING_HH

#include <cmath>

#include <QSharedPointer>

namespace errormining {

/**
 * Scoring methods.
 */
enum ScoringMethod { SCORING_SUSP, SCORING_SUSP_OBS, SCORING_SUSP_UNIQSENTS,
    SCORING_SUSP_LN_OBS, SCORING_SUSP_LN_UNIQSENTS, SCORING_SUSP_DELTA,
    SCORING_SUSP_LN_DELTA};

struct ScoreFun
{
	virtual ~ScoreFun() {}
    virtual double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const = 0;
};

struct ScoreSusp : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspObs : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspUniqSents : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspLnObs : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspLnUniqSents : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspDelta : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

struct ScoreSuspLnDelta : public ScoreFun
{
    double operator()(double suspicion, size_t freq, size_t suspFreq, size_t uniqSentsFreq) const;
};

QSharedPointer<ScoreFun> selectScoreFun(ScoringMethod scoringMethod);

inline double ScoreSusp::operator()(double suspicion, size_t, size_t, size_t) const
{
	return suspicion;
}

inline double ScoreSuspObs::operator()(double suspicion, size_t, size_t suspFreq, size_t) const
{
	return suspicion * suspFreq;
}

inline double ScoreSuspUniqSents::operator()(double suspicion, size_t, size_t, size_t uniqSentsFreq) const
{
	return suspicion * uniqSentsFreq;
}

inline double ScoreSuspLnObs::operator()(double suspicion, size_t, size_t suspFreq, size_t) const
{
	return suspicion * (1 + log(suspFreq));
}

inline double ScoreSuspLnUniqSents::operator()(double suspicion, size_t, size_t, size_t uniqSentsFreq) const
{
	return suspicion * log(uniqSentsFreq);
}

inline double ScoreSuspDelta::operator()(double suspicion, size_t freq, size_t suspFreq, size_t) const
{
    return suspicion * (static_cast<double>(suspFreq) - (static_cast<double>(freq) - static_cast<double>(suspFreq)));
}

inline double ScoreSuspLnDelta::operator()(double suspicion, size_t freq, size_t suspFreq, size_t) const
{
  double delta = static_cast<double>(suspFreq) - (static_cast<double>(freq) - static_cast<double>(suspFreq));

  if (delta < 0.0)
    return 0.0;
  else
    return suspicion * (1 + log(delta));
    //return suspicion * log(static_cast<double>(suspFreq) - (static_cast<double>(freq) - static_cast<double>(suspFreq)));
}

}

#endif // SCORING_HH
