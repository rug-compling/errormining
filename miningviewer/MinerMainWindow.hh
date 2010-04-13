#ifndef _MINER_MAINWINDOW_HH
#define _MINER_MAINWINDOW_HH

#include <set>

#include <QListWidgetItem>
#include <QMainWindow>
#include <QModelIndex>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>

#include <errormining/ScoringMethod.hh>

#include "PreferencesDialog.hh"
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
	void saveForms();
	void sentenceRegExpChanged();
	void showPreferences();

private:
	MinerMainWindow(MinerMainWindow const &other);
	MinerMainWindow &operator=(MinerMainWindow const &other);

	bool isValidForm(QString const &form) const;
	void readSettings();
	bool removeForm(QString const &form);
	void showForms();
	errormining::ScoringMethod scoringMethod();
	void updateSentenceList();
	void updateStatistics();
	void writeSettings();

	Ui::MinerMainWindow d_minerMainWindow;
	PreferencesDialog d_preferencesDialog;
	QSharedPointer<QRegExp> d_filterRegExp;
	QSharedPointer<QRegExp> d_sentenceFilterRegExp;
};

}

#endif // _MINER_MAINWINDOW_HH

