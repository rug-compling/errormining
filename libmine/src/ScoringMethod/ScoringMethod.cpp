#include "ScoringMethod.ih"

shared_ptr<ScoreFun> errormining::selectScoreFun(ScoringMethod scoringMethod)
{
	if (scoringMethod == SCORING_SUSP)
		return shared_ptr<ScoreFun>(new ScoreSusp);
	else if (scoringMethod == SCORING_SUSP_OBS)
		return shared_ptr<ScoreFun>(new ScoreSuspObs);
	else if (scoringMethod == SCORING_SUSP_UNIQSENTS)
		return shared_ptr<ScoreFun>(new ScoreSuspUniqSents);
	else if (scoringMethod == SCORING_SUSP_LN_OBS)
		return shared_ptr<ScoreFun>(new ScoreSuspLnObs);
	else if (scoringMethod == SCORING_SUSP_LN_UNIQSENTS)
		return shared_ptr<ScoreFun>(new ScoreSuspLnUniqSents);

	// Throw an exception?
	return shared_ptr<ScoreFun>(new ScoreSusp);
}
