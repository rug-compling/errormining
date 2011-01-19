#include "ScoringMethod.ih"

QSharedPointer<ScoreFun> errormining::selectScoreFun(ScoringMethod scoringMethod)
{
	if (scoringMethod == SCORING_SUSP)
		return QSharedPointer<ScoreFun>(new ScoreSusp);
	else if (scoringMethod == SCORING_SUSP_OBS)
		return QSharedPointer<ScoreFun>(new ScoreSuspObs);
	else if (scoringMethod == SCORING_SUSP_UNIQSENTS)
		return QSharedPointer<ScoreFun>(new ScoreSuspUniqSents);
	else if (scoringMethod == SCORING_SUSP_LN_OBS)
		return QSharedPointer<ScoreFun>(new ScoreSuspLnObs);
	else if (scoringMethod == SCORING_SUSP_LN_UNIQSENTS)
		return QSharedPointer<ScoreFun>(new ScoreSuspLnUniqSents);
    else if (scoringMethod == SCORING_SUSP_DELTA)
        return QSharedPointer<ScoreFun>(new ScoreSuspDelta);
    else if (scoringMethod == SCORING_SUSP_LN_DELTA)
        return QSharedPointer<ScoreFun>(new ScoreSuspLnDelta);

	// Throw an exception?
	return QSharedPointer<ScoreFun>(new ScoreSusp);
}
