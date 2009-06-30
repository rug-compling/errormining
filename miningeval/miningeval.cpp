#include <cmath>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <utility>

#include <QCoreApplication>
#include <QSet>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

#include <errormining/ScoringMethod.hh>

using namespace std;
using namespace errormining;

struct FormScore
{
	FormScore(QString &newForm, double newScore, size_t newSuspFreq) :
		form(newForm), score(newScore), suspFreq(newSuspFreq) {}

	QString form;
	double score;
	size_t suspFreq;
};

struct FormScoreCompare :
	public binary_function<FormScore, FormScore, bool>
{
	bool operator()(FormScore const &f1, FormScore const &f2)
	{
		if (f1.score == f2.score)
		{
			if (f1.suspFreq == f2.suspFreq)
				return f1.form < f2.form;
			else
				return f2.suspFreq < f1.suspFreq;
		}
		else
			return f2.score < f1.score;
	}
};

typedef set<FormScore, FormScoreCompare> FormScoreSet;

bool openDatabase(QString const &dbFilename)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbFilename);
	return db.open();
}

ScoringMethod selectScoringMethod(string const &scoringMethod)
{
		if (scoringMethod == "scoring_susp")
			return SCORING_SUSP;
		else if (scoringMethod == "scoring_susp_obs")
			return SCORING_SUSP_OBS;
		else if (scoringMethod == "scoring_susp_uniqsents")
			return SCORING_SUSP_UNIQSENTS;
		else if (scoringMethod == "scoring_susp_ln_obs")
			return SCORING_SUSP_LN_OBS;
		else if (scoringMethod == "scoring_susp_ln_uniqsents")
			return SCORING_SUSP_LN_UNIQSENTS;

		cerr << "Unknown scoring method: '" << scoringMethod <<
			"', using 'scoring_susp'." << endl;

		return SCORING_SUSP;
}

FormScoreSet scoreForms(ScoringMethod scoringMethod)
{
	FormScoreSet formScores;
	QSharedPointer<ScoreFun> scoreFun = selectScoreFun(scoringMethod);

	QSqlQuery query("SELECT form, suspicion, suspFreq, uniqSentsFreq"
		" FROM forms");// WHERE suspicion > 1.5 * (SELECT AVG(suspicion) FROM forms)");
	while (query.next())
	{
		QString form = query.value(0).toString();
		double suspicion = query.value(1).toDouble();
		uint suspFreq = query.value(2).toUInt();
		uint uniqSentsFreq = query.value(3).toUInt();

		double score = (*scoreFun)(suspicion, suspFreq, uniqSentsFreq);
		formScores.insert(FormScore(form, score, suspFreq));
	}

	return formScores;
}

QSet<size_t> allSentences(bool unparsable)
{
	QSet<size_t> sentenceIds;
	
	QSqlQuery query;
	query.prepare("SELECT sentences.rowid FROM sentences "
		"WHERE sentences.unparsable = :unparsable");
	query.bindValue(":unparsable", unparsable);

	query.exec();

	while (query.next())
		sentenceIds.insert(query.value(0).toUInt());

	return sentenceIds;
}

QSet<size_t> formSentences(QString const &form, bool unparsable)
{
	QSet<size_t> sentenceIds;

	QSqlQuery query;
	query.prepare("SELECT formSentence.sentenceId "
		"FROM forms, formSentence, sentences "
		"WHERE forms.form = :form AND forms.rowid = formSentence.formId AND "
		"sentences.rowid = formSentence.sentenceId AND "
		"sentences.unparsable = :unparsable");
	query.bindValue(":form", form);
	query.bindValue(":unparsable", unparsable);
	
	query.exec();

	while (query.next())
		sentenceIds.insert(query.value(0).toUInt());
	
	return sentenceIds;
}

void usage(string const &programName)
{
	cout << "Usage: " << programName << " database scoring_method" << endl;
}

int main(int argc, char *argv[])
{
	double const beta = 0.5;
	double const betaSquare = beta * beta;

	QCoreApplication app(argc, argv);

	if (argc != 3)
	{
		usage(argv[0]);
		return 1;
	}

	QString dbFilename = argv[1];
	if (!openDatabase(dbFilename))
	{
		cout << "Error opening: " << dbFilename.toLatin1().constData() <<
			endl;
		return 1;
	}

	FormScoreSet formScores(scoreForms(selectScoringMethod(argv[2])));

	QSet<size_t> allParsableFound;
	QSet<size_t> allUnparsableFound;

	QSet<size_t> allParsable(allSentences(false));
	QSet<size_t> allUnparsable(allSentences(true));

	size_t i = 0;
	FormScoreSet::const_iterator iter = formScores.begin();
	while (iter != formScores.end())
	{
		QSet<size_t> unparsableFound = formSentences(iter->form, true);
		allUnparsableFound.unite(unparsableFound);
		QSet<size_t> parsableFound = formSentences(iter->form, false);
		allParsableFound.unite(parsableFound);

		double recall = allUnparsableFound.size() /
			static_cast<double>(allUnparsable.size());
		double precision = allUnparsableFound.size() /
			static_cast<double>(allUnparsableFound.size() + allParsableFound.size());
		double fscore = (2.0 * precision * recall) / (precision + recall);
		double betaFscore = ((1.0 + betaSquare) *  precision * recall) /
			(betaSquare * precision + recall);

		cout << i + 1 << "\t" << recall << "\t" << precision <<
			"\t" << fscore << "\t" << betaFscore << "\t" <<
			iter->form.toLatin1().data() << endl;

		++i;
		++iter;
	}

	return 0;
}
