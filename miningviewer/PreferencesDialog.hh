#ifndef PREFERENCESDIALOG_HH
#define PREFERENCESDIALOG_HH

#include <QDialog>
#include <QWidget>

#include "ui_PreferencesDialog.h"

class PreferencesDialog : public QDialog
{
	Q_OBJECT
public:
	PreferencesDialog(QWidget *parent = 0);
private slots:
	void accept();
private:
	PreferencesDialog(PreferencesDialog const &other);
	PreferencesDialog &operator=(PreferencesDialog const &other);
	void readSettings();

	Ui::PreferencesDialog d_preferencesDialog;
};

#endif // PREFERENCESDIALOG_HH
