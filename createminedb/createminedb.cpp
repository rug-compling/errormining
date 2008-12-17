#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QtDebug>

using namespace std;

bool openDatabase(QString const &dbFilename)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbFilename);
	return db.open();
}

void createTables()
{
	QSqlDatabase::database().transaction();
	QSqlQuery createFormsTableQuery("CREATE TABLE forms ("
		"form TEXT, suspicion REAL, freq INTEGER, suspFreq INTEGER, "
		"uniqSentsFreq INTEGER)");
	createFormsTableQuery.exec();
	
	QSqlQuery createFormsIndexQuery("CREATE UNIQUE INDEX form_idx ON forms (form)");
	createFormsIndexQuery.exec();

	QSqlQuery createSentencesTableQuery("CREATE TABLE sentences ("
		"sentence TEXT, unparsable BOOLEAN)");
	createSentencesTableQuery.exec();
	
	QSqlQuery createFormSentenceTableQuery("CREATE TABLE formSentence ("
		"formId INTEGER, sentenceId INTEGER)");
	createFormSentenceTableQuery.exec();
	
	//QSqlQuery createFormSentenceIndexQuery("CREATE UNIQUE INDEX formSentence_idx ON formSentence (formId, sentenceId)");
	//createFormSentenceIndexQuery.exec();

	QSqlQuery createSentenceIdIndexQuery("CREATE INDEX sentenceId_idx ON formSentence (sentenceId)");
	createSentenceIdIndexQuery.exec();

	QSqlQuery createFormIdIndexQuery("CREATE INDEX formId_idx ON formSentence (formId)");
	createFormIdIndexQuery.exec();

	QSqlDatabase::database().commit();
}

size_t addResults(char const *resultsFilename)
{
	QSqlQuery query;
	query.prepare("INSERT INTO forms (form, suspicion, freq, suspFreq) "
		"VALUES (:form, :suspicion, :freq, :suspFreq)");

	QFile resultsFile(resultsFilename);
	if (!resultsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return 0;

	QSqlDatabase::database().transaction();

	size_t longestNgram = 0;
	while (!resultsFile.atEnd())
	{
		QString line = resultsFile.readLine().trimmed();
		QStringList lineParts = line.split(" ");
		
		// Extract and bind information.
		query.bindValue(":suspFreq", lineParts.takeLast());
		query.bindValue(":freq", lineParts.takeLast());
		query.bindValue(":suspicion", lineParts.takeLast());
		query.bindValue(":form", lineParts.join(" "));
		
		if (static_cast<size_t>(lineParts.size()) > longestNgram)
			longestNgram = lineParts.size();
		
		query.exec();
	}

	QSqlDatabase::database().commit();
	
	return longestNgram;
}

void addSentences(char const *filename, bool unparsable)
{
	QFile sentenceFile(filename);
	if (!sentenceFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	{
		// Prepare for inserting sentences.
		QSqlQuery insertSentenceQuery;
		insertSentenceQuery.prepare("INSERT INTO sentences (sentence, unparsable) "
									"VALUES (:sentence, :unparsable)");
		
		// Insert sentences.
		QSqlDatabase::database().transaction();
		while (!sentenceFile.atEnd())
		{
			QString sentence = sentenceFile.readLine().trimmed();
			insertSentenceQuery.bindValue(":sentence", sentence);
			insertSentenceQuery.bindValue(":unparsable", unparsable);
			insertSentenceQuery.exec();
		}
		QSqlDatabase::database().commit();
	}
}

vector<pair<uint, uint> > findFormSentencePairs(int n)
{
	vector<pair<uint, uint> > formSentencePairs;

	{
		// Prepare a query for finding forms.
		QSqlQuery formIdQuery;
		formIdQuery.prepare("SELECT rowid FROM forms WHERE form = :form");
		
		// Select all sentences.
		QSqlQuery sentenceQuery("SELECT rowid, sentence FROM sentences");
		sentenceQuery.exec();
		
		while (sentenceQuery.next())
		{
			uint sentenceId = sentenceQuery.value(0).toUInt();
			QString sentence = sentenceQuery.value(1).toString();
			
			QStringList words = sentence.split(" ");
			for (int i = 0; i < words.size(); ++i)
				for (int j = 0; j < n; ++j)
				{
					if (i + j == words.size())
						break;
					
					QStringList formList;
					copy(words.begin() + i, words.begin() + i + j + 1, back_inserter(formList));
					
					QString form = formList.join(" ");
					formIdQuery.bindValue(":form", form);
					formIdQuery.exec();
					
					// Sometimes a form that is encountered in a sentence is not known because
					// a frequency threshold is set in the miner. In this case, skip this form.
					if (!formIdQuery.next())
						continue;

					uint formId = formIdQuery.value(0).toUInt();
					
					formSentencePairs.push_back(make_pair(formId, sentenceId));
				}
		}
	}
	
	return formSentencePairs;
}

void populateLinkTable(vector<pair<uint, uint> > const &formSentencePairs)
{
	// Prepare query for insertion into the link table.
	QSqlQuery insertFormSentenceQuery;
	insertFormSentenceQuery.prepare("INSERT INTO formSentence (formId, sentenceId) "
		"VALUES (:formId, :sentenceId)");

	// Insert link table pairs.
	QSqlDatabase::database().transaction();
	for (vector<pair<uint, uint> >::const_iterator iter = formSentencePairs.begin();
		iter != formSentencePairs.end(); ++iter)
	{
		insertFormSentenceQuery.bindValue(":formId", iter->first);
		insertFormSentenceQuery.bindValue(":sentenceId", iter->second);
		insertFormSentenceQuery.exec();
	}
	if (insertFormSentenceQuery.lastError().isValid())
		qDebug() << insertFormSentenceQuery.lastError();
	QSqlDatabase::database().commit();
}

// Extract the set of form IDs, from the form-sentence link table.
set<uint> extractFormIds(
	vector<pair<uint, uint> > const &formSentencePairs)
{
	set<uint> formIds;

	for (vector<pair<uint, uint> >::const_iterator iter =
			formSentencePairs.begin(); iter != formSentencePairs.end();
			++iter)
		formIds.insert(iter->first);
	
	return formIds;
}

void calcUniqSents(set<uint> const &sentenceIds)
{
	map<uint,uint> uniqSentsFreqs;

	{
		// Find the number of unique unparsable sentences for a given form.
		QSqlQuery uniqueSentenceQuery;
		uniqueSentenceQuery.prepare("SELECT COUNT(DISTINCT(sentences.sentence)) "
			"FROM formSentence, sentences WHERE formSentence.formId = :formId "
			"AND formSentence.sentenceId = sentences.rowid "
			"AND sentences.unparsable = 'true'");

		for (set<uint>::const_iterator iter = sentenceIds.begin();
				iter != sentenceIds.end(); ++iter)
		{
			uniqueSentenceQuery.bindValue(":formId", *iter);
			uniqueSentenceQuery.exec();
			uniqueSentenceQuery.next();
			uniqSentsFreqs[*iter] = uniqueSentenceQuery.value(0).toUInt();
		}
	}

	// Store the unique sentence frequencies given a form.
	QSqlQuery updateUniqSentsQuery;
	updateUniqSentsQuery.prepare("UPDATE forms SET uniqSentsFreq = :freq "
		"WHERE forms.rowid = :formId");

	QSqlDatabase::database().transaction();
	for (map<uint, uint>::const_iterator iter = uniqSentsFreqs.begin();
		iter != uniqSentsFreqs.end(); ++iter)
	{
		updateUniqSentsQuery.bindValue(":formId", iter->first);
		updateUniqSentsQuery.bindValue(":freq", iter->second);
		updateUniqSentsQuery.exec();
	}
	QSqlDatabase::database().commit();
}

void populateDatabase(char const *resultsFilename,
	char const *unparsableFilename, char const *parsableFilename = 0)
{
	cerr << "Creating tables... ";
	createTables();
	cerr << "done!" << endl << "Adding mining results... ";
	size_t n = addResults(resultsFilename);
	cerr << "done!" << endl << "Adding sentences... ";
	addSentences(unparsableFilename, true);
	if (parsableFilename != 0)
		addSentences(parsableFilename, false);
	cerr << "done!" << endl << "Finding form-sentence links... ";
	vector<pair<uint, uint> > formSentencePairs = findFormSentencePairs(n);
	cerr << "done!" << endl << "Populating link table... ";
	populateLinkTable(formSentencePairs);
	cerr << "done!" << endl << "Calculating unique sentence frequencies... ";
	set<uint> formIds = extractFormIds(formSentencePairs);
	calcUniqSents(formIds);
	cerr << "done!" << endl;
}

void usage(string const &programName)
{
	cout << "Syntax: " << programName <<
		" mine_results [parsable_sentences] unparsable_sentences"
		" mine_database" << endl;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	if (argc != 4 && argc != 5)
	{
		usage(argv[0]);
		return 1;
	}

	QString dbFilename = argv[argc - 1];
	if (QFile::exists(dbFilename))
		QFile::remove(dbFilename);
		
	if (!openDatabase(dbFilename)) {
		cout << "Error opening: " << dbFilename.toLatin1().constData() <<
			endl;
		return 1;
	}

	if (argc == 4)
		populateDatabase(argv[1], argv[2]);
	else
		populateDatabase(argv[1], argv[3], argv[2]);
}
