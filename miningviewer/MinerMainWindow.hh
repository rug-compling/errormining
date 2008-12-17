#ifndef _MINER_MAINWINDOW_HH
#define _MINER_MAINWINDOW_HH

#include <memory>
#include <set>

#include <QListWidgetItem>
#include <QMainWindow>
#include <QModelIndex>
#include <QRegExp>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>

#include "ui_MinerMainWindow.h"

namespace miningviewer {

class MinerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MinerMainWindow(QWidget *parent = 0);

public slots:
	void close();

private slots:
	void copySentence();
	void scoringMethodChanged(int index);
	void formSelected(QTreeWidgetItem *item, QTreeWidgetItem *previousItem);
	void regExpChanged();
	void removeSelectedForms();
	void removeStaleForms(std::set<int> const &affectedFormIds);
	void sentenceRegExpChanged();
	void sentenceSelected(QListWidgetItem *item, QListWidgetItem *);

private:
	MinerMainWindow(MinerMainWindow const &other);
	MinerMainWindow &operator=(MinerMainWindow const &other);

	enum ScoringMethod { SCORING_SUSP, SCORING_SUSP_OBS, SCORING_SUSP_UNIQSENTS,
		SCORING_SUSP_LN_OBS, SCORING_SUSP_LN_UNIQSENTS };

	bool isValidForm(QString const &form) const;
	void readSettings();
	void removeForm(QString const &form);
	void showForms();
	ScoringMethod scoringMethod();
	void updateSentenceList();
	void writeSettings();

	Ui::MinerMainWindow d_minerMainWindow;
	std::auto_ptr<QRegExp> d_filterRegExp;
	std::auto_ptr<QRegExp> d_sentenceFilterRegExp;
};

}

#endif // _MINER_MAINWINDOW_HH

