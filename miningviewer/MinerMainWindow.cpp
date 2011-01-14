#include "MinerMainWindow.ih"

MinerMainWindow::MinerMainWindow(QWidget *parent) : QMainWindow(parent)
{
	d_minerMainWindow.setupUi(this);

	readSettings();
	updateStatistics();

	d_minerMainWindow.formsTreeWidget->sortByColumn(0, Qt::DescendingOrder);

	showForms();

	d_minerMainWindow.formsTreeWidget->setSortingEnabled(true);

	connect(d_minerMainWindow.copyAction, SIGNAL(activated()),
		this, SLOT(copySentence()));
	connect(d_minerMainWindow.preferencesAction, SIGNAL(activated()),
		this, SLOT(showPreferences()));
	connect(d_minerMainWindow.scoringComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(scoringMethodChanged(int)));
	connect(d_minerMainWindow.formsTreeWidget,
		SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
		this, SLOT(formSelected(QTreeWidgetItem *, QTreeWidgetItem *)));
    connect(d_minerMainWindow.removeFormPushButton, SIGNAL(clicked()),
		this, SLOT(removeSelectedForms()));
	connect(d_minerMainWindow.saveAction, SIGNAL(activated()),
		this, SLOT(saveForms()));
	

	// Regular expression LineEdits.
	connect(d_minerMainWindow.regExpLineEdit, SIGNAL(returnPressed()),
		this, SLOT(regExpChanged()));
	connect(d_minerMainWindow.sentenceRegExpLineEdit, SIGNAL(returnPressed()),
		this, SLOT(sentenceRegExpChanged()));

    d_proxySentenceModel.setSourceModel(&d_sentenceModel);
    d_minerMainWindow.sentenceView->setModel(&d_proxySentenceModel);
}

void MinerMainWindow::close()
{
	writeSettings();
	QMainWindow::close();
}

void MinerMainWindow::copySentence()
{
    //d_minerMainWindow.sentenceTextEdit->copy();
}

bool MinerMainWindow::isValidForm(QString const &form) const
{
	QSqlQuery formQuery;
	formQuery.prepare("SELECT COUNT(*) FROM forms WHERE form = :form");
	formQuery.bindValue(":form", form);
	formQuery.exec();
	formQuery.next();

	if (formQuery.value(0).toInt() < 1)
		return false;

	return true;
}

void MinerMainWindow::formSelected(QTreeWidgetItem *item, QTreeWidgetItem *)
{
	if (d_minerMainWindow.allSentenceMatchCheckBox->isChecked())
	{
		d_minerMainWindow.sentenceRegExpLineEdit->clear();
		d_sentenceFilterRegExp.clear();
	}

	if (item == 0) {
		d_minerMainWindow.suspicionLabel->clear();
		d_minerMainWindow.freqLabel->clear();
		d_minerMainWindow.suspFreqLabel->clear();
		d_minerMainWindow.uniqSentsFreqLabel->clear();
		
		updateSentenceList();

        return;
	}

	QString form = item->text(1);
	
	{
		QSqlQuery formInfoQuery;
		formInfoQuery.prepare("SELECT suspicion, freq, suspFreq, uniqSentsFreq"
			" FROM forms WHERE form = :form");
		formInfoQuery.bindValue(":form", form);
		formInfoQuery.exec();
		formInfoQuery.next();
		
		d_minerMainWindow.suspicionLabel->setText(formInfoQuery.value(0).toString());
		d_minerMainWindow.freqLabel->setText(formInfoQuery.value(1).toString());
		d_minerMainWindow.suspFreqLabel->setText(formInfoQuery.value(2).toString());
		d_minerMainWindow.uniqSentsFreqLabel->setText(
			formInfoQuery.value(3).toString());
	}

    updateSentenceList();
}

void MinerMainWindow::readSettings()
{
	QSettings settings("RUG", "Mining Viewer");

	// Window geometry.
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(800, 500)).toSize();
	resize(size);

	// Splitter.
	d_minerMainWindow.splitter->restoreState(
		settings.value("splitterSizes").toByteArray());
	move(pos);
}

void MinerMainWindow::regExpChanged()
{
	QString regexStr = d_minerMainWindow.regExpLineEdit->text();

	// If the regexp line edit widget has a zero-length, we suppose
	// that the user doesn't want to apply any filtering. If so,
	// deallocate the current regexp (if any).
	if (regexStr.size() == 0)
		d_filterRegExp.clear();
	else {
		QRegExp *filterRegExp = new QRegExp(regexStr);

		// Check if the regexp is valid. If not, we leave the existing
		// regexp as-is.
		if (!filterRegExp->isValid()) {
			d_minerMainWindow.statusbar->showMessage(
				QString("Compilation of regular expression failed: ") +
				filterRegExp->errorString(), 5000);
			delete filterRegExp;
			return;
		}

		d_filterRegExp = QSharedPointer<QRegExp>(filterRegExp);
	}

	// There could still be a message in the status bar about a
	// previously incorrect regexp. Clear the message for clarity.
	d_minerMainWindow.statusbar->clearMessage();

	// Redo the list of forms, with the new regex as a filter.
	showForms();
}

void MinerMainWindow::removeSelectedForms()
{
	// This method removes all selected forms. After removing forms, we'll
	// want to select an item close to the removed items, to avoid scrolling
	// back to the beginning after a removal. This is harder than it seems,
	// because the underlying queries can also remove forms preceeding or
	// succeeding the selected forms (namely orphaned forms). For this
	// reason, we can't use absolute row numbers. Additionally, pointers
	// can't be used either, since the list items are newly allocated when
	// the list of forms is refreshed. Setting the correct position is
	// a kludge, but is now done in the following manner:
	//
	// 1. We take the first item of the selected items, and get it
	//    index, named 'newIndex'.
	// 2. After removing the forms from the database, we still have
	//    the old form list around. We go up this list until we find
	//    a form that is still in the database. We store the literal
	//    name of the form.
	// 3. After the list of forms is refreshed, we search for the
	//    literal found in the previous step, and make the first item
	//    that was found (only one should be found) the currently
	//    selected item.

	QList<QTreeWidgetItem *> selectedItems =
		d_minerMainWindow.formsTreeWidget->selectedItems();
	
	QTreeWidgetItem *treeRootItem =
		d_minerMainWindow.formsTreeWidget->invisibleRootItem();

	// Do nothing when no forms are selected.
	if (selectedItems.size() == 0)
		return;

	// Get the absolute index of the first item in the selection.
	int newIndex = treeRootItem->indexOfChild(selectedItems[0]);

    // Perform deep or shallow removal?
    QSettings settings("RUG", "Mining Viewer");
    bool deepRemoval = settings.value(DEEP_FORM_REMOVAL,
        DEEP_FORM_REMOVAL_DEFAULT).toBool();

	// Remove the forms represented by the selected items.
	for (QList<QTreeWidgetItem *>::iterator iter = selectedItems.begin();
			iter != selectedItems.end(); ++iter)
    {
        bool success;
        QString form = (*iter)->text(1);
        success = deepRemoval ? removeForm(form) : removeFormShallow(form);

        if (!success)
            return;
    }

    updateStatistics();

	// Scan up until we find a form that is still valid (exists in the
	// database).
	while (newIndex > 0 &&
		!isValidForm(treeRootItem->child(newIndex)->text(1))) { --newIndex; }

	// Get the valid form as a string.
	QString selectForm = treeRootItem->child(newIndex)->text(1);
	
	// Redisplay forms.
	showForms();

	// Find a pointer to the item representing the form that we've found to
	// be valid, and close to a previously selected form.
	QList<QTreeWidgetItem *> items =
		d_minerMainWindow.formsTreeWidget->findItems(selectForm, Qt::MatchExactly);

	// If the form was found (should always happen), select it.
	if (items.size() > 0)
		d_minerMainWindow.formsTreeWidget->setCurrentItem(items[0]);
}

bool MinerMainWindow::removeForm(QString const &form)
{
	set<int> affectedFormIds;
	{
		QSqlQuery affectedFormIdsQuery;
		affectedFormIdsQuery.prepare("SELECT DISTINCT aux.formId"
			" FROM formSentence LEFT OUTER JOIN formSentence as aux"
			" ON formSentence.sentenceId = aux.sentenceId"
			" WHERE formSentence.formId = "
			" (SELECT rowid FROM forms WHERE forms.form = :form)") ;
		affectedFormIdsQuery.bindValue(":form", form);
		affectedFormIdsQuery.exec();

		while (affectedFormIdsQuery.next())
			affectedFormIds.insert(affectedFormIdsQuery.value(0).toInt());
	}

	{
		QSqlQuery removeSentencesQuery;
		removeSentencesQuery.prepare("DELETE FROM sentences"
			" WHERE sentences.rowid IN (SELECT DISTINCT formSentence.sentenceId"
			" FROM formSentence, forms WHERE formSentence.formId = forms.rowid AND"
			" forms.form = :form)");

		removeSentencesQuery.bindValue(":form", form);
		removeSentencesQuery.exec();

		if (removeSentencesQuery.lastError().isValid())
		{
			QMessageBox errorMessage(QMessageBox::Critical, "Could not remove form(s)",
				"Could not remove the selected form(s)! Do you have write access "
				"to this database?", QMessageBox::Ok, this);
			errorMessage.exec();
			return false;
		}
	}

	{
		QSqlQuery removeLinksQuery;
		removeLinksQuery.prepare("DELETE FROM formSentence"
			" WHERE formSentence.sentenceId IN"
			" (SELECT DISTINCT formSentence.sentenceId"
			" FROM formSentence, forms WHERE formSentence.formId = forms.rowid AND"
			" forms.form = :form)");

		removeLinksQuery.bindValue(":form", form);
		removeLinksQuery.exec();
	}

	removeStaleForms(affectedFormIds);
	
	return true;
}

bool MinerMainWindow::removeFormShallow(const QString &form)
{
    QSqlQuery removeFormQuery;
    removeFormQuery.prepare("DELETE FROM forms"
        " WHERE forms.form = :form");
    removeFormQuery.bindValue(":form", form);
    removeFormQuery.exec();

    return true;
}

void MinerMainWindow::removeStaleForms(std::set<int> const &affectedFormIds)
{
	QSqlQuery removeFormsQuery;
	removeFormsQuery.prepare("DELETE FROM forms"
		" WHERE forms.rowid = :formId AND"
		" (SELECT COUNT(*) FROM formSentence WHERE formSentence.formId = :formId2) = 0");

	QSqlDatabase::database().transaction();

	for (std::set<int>::const_iterator iter = affectedFormIds.begin();
		iter != affectedFormIds.end(); ++iter)
	{
		removeFormsQuery.bindValue(":formId", *iter);
		removeFormsQuery.bindValue(":formId2", *iter);
		removeFormsQuery.exec();
	}

	QSqlDatabase::database().commit();
}

void MinerMainWindow::saveForms()
{
	QString filename = QFileDialog::getSaveFileName(this);
	if (filename.isNull())
		return;

	QFile formsFile(filename);
	if (!formsFile.open(QIODevice::WriteOnly))
	{
		QErrorMessage msg(this);
		msg.showMessage("Could not open file for writing!");
		return;
	}

	QTextStream formsOut(&formsFile);

	ScoringMethod curScoringMethod = scoringMethod();
	QSharedPointer<ScoreFun> scoreFun = selectScoreFun(curScoringMethod);

	// Retrieve threshold preferences.
	QSettings settings("RUG", "Mining Viewer");
	double suspThreshold = settings.value(SUSP_THRESHOLD_SETTING,
		SUSP_THRESHOLD_SETTING_DEFAULT).toDouble();
	double avgMultiplier = settings.value(AVG_MULTIPLIER_SETTING,
		AVG_MULTIPLIER_SETTING_DEFAULT).toDouble();
	QString suspThresholdMethod = settings.value(THRESHOLD_METHOD_SETTING,
		SUSP_THRESHOLD_METHOD_DEFAULT).toString();
	uint unparsableFreqThreshold =
		settings.value(UNPARSABLE_FREQ_THRESHOLD_SETTING,
			UNPARSABLE_FREQ_THRESHOLD_DEFAULT).toUInt();
	uint parsableFreqThreshold =
		settings.value(FREQ_THRESHOLD_SETTING, FREQ_THRESHOLD_DEFAULT).toUInt();

	QSharedPointer<QSqlQuery> query;
	if (suspThresholdMethod == AVG_MULTIPLIER_METHOD)
	{
		query = QSharedPointer<QSqlQuery>(new QSqlQuery("SELECT form, suspicion, suspFreq, uniqSentsFreq"
			" FROM forms WHERE suspicion >= :avgMultiplier * (SELECT AVG(suspicion) FROM forms) "
			" AND suspFreq >= :unparsableFreqThreshold AND freq >= :unparsableFreqThreshold"));
		query->bindValue("avgMultiplier", avgMultiplier);
	}
	else
	{
		query = QSharedPointer<QSqlQuery>(new QSqlQuery("SELECT form, suspicion, freq, suspFreq, uniqSentsFreq"
			" FROM forms WHERE suspicion >= :suspThreshold"
			" AND suspFreq >= :unparsableFreqThreshold AND freq >= :parsableFreqThreshold"));
		query->bindValue("suspThreshold", suspThreshold);
	}

	query->bindValue(":unparsableFreqThreshold", unparsableFreqThreshold);
	query->bindValue(":parsableFreqThreshold", parsableFreqThreshold);
	query->exec();

	while (query->next())
	{
		QString form = query->value(0).toString();
		double suspicion = query->value(1).toDouble();
		uint freq = query->value(2).toUInt();
		uint suspFreq = query->value(3).toUInt();
		uint uniqSentsFreq = query->value(4).toUInt();

		// If a regular expression was allocated, use it to filter forms;
		// if this form does not match, skip it.
		if (d_filterRegExp.data() != 0)
			if (d_filterRegExp->indexIn(form) == -1)
				continue;

		// Score this form.
		double score = (*scoreFun)(suspicion, suspFreq, uniqSentsFreq);

		formsOut << form << " " << score << " " << freq << " " << suspFreq << "\n";
	}

	formsOut.flush();
}

void MinerMainWindow::scoringMethodChanged(int)
{
	showForms();
}

void MinerMainWindow::sentenceRegExpChanged()
{
	QString regexStr = d_minerMainWindow.sentenceRegExpLineEdit->text();

	// When the regexp string length is 0, change the pointer to the
	// regexp to a null pointer to signal that no regexp should be used.
    if (regexStr.size() == 0) {
		d_sentenceFilterRegExp.clear();
        d_proxySentenceModel.setFilterRegExp(QRegExp());
        d_sentenceModel.setQuery(d_sentenceModel.query());
    }
	else {
		QRegExp *sentenceFilterRegExp = new QRegExp(QString("(") + regexStr + ")");

		// Check if the regexp is valid. If not, we leave the existing
		// regexp as-is.
		if (!sentenceFilterRegExp->isValid()) {
			d_minerMainWindow.statusbar->showMessage(
				QString("Compilation of regular expression failed: ") +
				sentenceFilterRegExp->errorString(), 5000);
			delete sentenceFilterRegExp;
			return;
		}

		d_sentenceFilterRegExp = QSharedPointer<QRegExp>(sentenceFilterRegExp);
        d_proxySentenceModel.setFilterRegExp(*sentenceFilterRegExp);
        d_sentenceModel.setQuery(d_sentenceModel.query());
    }

	// There could still be a message in the status bar about a
	// previously incorrect regexp. Clear the message for clarity.
	d_minerMainWindow.statusbar->clearMessage();

	// Regenerate the sentence list with the current regexp.
	updateSentenceList();
}

void MinerMainWindow::showForms()
{
	d_minerMainWindow.formsTreeWidget->clear();

	ScoringMethod curScoringMethod = scoringMethod();
	QSharedPointer<ScoreFun> scoreFun = selectScoreFun(curScoringMethod);

	// Retrieve threshold preferences.
	QSettings settings("RUG", "Mining Viewer");
	double suspThreshold = settings.value(SUSP_THRESHOLD_SETTING,
		SUSP_THRESHOLD_SETTING_DEFAULT).toDouble();
	double avgMultiplier = settings.value(AVG_MULTIPLIER_SETTING,
		AVG_MULTIPLIER_SETTING_DEFAULT).toDouble();
	QString suspThresholdMethod = settings.value(THRESHOLD_METHOD_SETTING,
		SUSP_THRESHOLD_METHOD_DEFAULT).toString();
	uint unparsableFreqThreshold =
		settings.value(UNPARSABLE_FREQ_THRESHOLD_SETTING,
			UNPARSABLE_FREQ_THRESHOLD_DEFAULT).toUInt();
	uint parsableFreqThreshold =
		settings.value(FREQ_THRESHOLD_SETTING, FREQ_THRESHOLD_DEFAULT).toUInt();

	QList<QTreeWidgetItem *> items;

	QSharedPointer<QSqlQuery> query;
	if (suspThresholdMethod == AVG_MULTIPLIER_METHOD)
	{
		query = QSharedPointer<QSqlQuery>(new QSqlQuery("SELECT form, suspicion, suspFreq, uniqSentsFreq"
			" FROM forms WHERE suspicion >= :avgMultiplier * (SELECT AVG(suspicion) FROM forms) "
			" AND suspFreq >= :unparsableFreqThreshold AND freq >= :unparsableFreqThreshold"));
		query->bindValue("avgMultiplier", avgMultiplier);
	}
	else
	{
		query = QSharedPointer<QSqlQuery>(new QSqlQuery("SELECT form, suspicion, suspFreq, uniqSentsFreq"
			" FROM forms WHERE suspicion >= :suspThreshold"
			" AND suspFreq >= :unparsableFreqThreshold AND freq >= :parsableFreqThreshold"));
		query->bindValue("suspThreshold", suspThreshold);
	}

	query->bindValue(":unparsableFreqThreshold", unparsableFreqThreshold);
	query->bindValue(":parsableFreqThreshold", parsableFreqThreshold);
	query->exec();

	while (query->next())
	{
		QString form = query->value(0).toString();
		double suspicion = query->value(1).toDouble();
		uint suspFreq = query->value(2).toUInt();
		uint uniqSentsFreq = query->value(3).toUInt();

		// If a regular expression was allocated, use it to filter forms;
		// if this form does not match, skip it.
		if (d_filterRegExp.data() != 0)
			if (d_filterRegExp->indexIn(form) == -1)
				continue;

		// Score this form.
		double score = (*scoreFun)(suspicion, suspFreq, uniqSentsFreq);

		// Create and add an item for this form.
		QTreeWidgetItem *item = new FormTreeWidgetItem(0);
		item->setText(0, QString::number(score));
		item->setText(1, form);
		items.append(item);
	}
		
	d_minerMainWindow.formsTreeWidget->insertTopLevelItems(0, items);
	d_minerMainWindow.formsTreeWidget->setCurrentItem(0);
}

void MinerMainWindow::showPreferences()
{
	if (d_preferencesDialog.exec() == QDialog::Accepted)
		showForms();
}

ScoringMethod MinerMainWindow::scoringMethod()
{
	int index = d_minerMainWindow.scoringComboBox->currentIndex();

	switch (index)
	{
	case 0:
		return SCORING_SUSP;
	case 1:
		return SCORING_SUSP_OBS;
	case 2:
		return SCORING_SUSP_UNIQSENTS;
	case 3:
		return SCORING_SUSP_LN_OBS;
	case 4:
		return SCORING_SUSP_LN_UNIQSENTS;
	default:
		// Unknown scoring method.
		return SCORING_SUSP;
	}
}

void MinerMainWindow::updateSentenceList()
{
    // Todo: Underline regex, escape HTML

    QSqlQuery sentenceQuery;

    d_minerMainWindow.sentenceView->scrollToTop();
	if (d_minerMainWindow.allSentenceMatchCheckBox->isChecked() &&
		d_sentenceFilterRegExp.data() != 0)
	{
		sentenceQuery.prepare("SELECT sentences.sentence FROM sentences"
			" WHERE sentences.unparsable = 'true'");
		sentenceQuery.exec();
	}
	else {
		if (d_minerMainWindow.formsTreeWidget->currentItem() == 0)
			return;

		QString form = d_minerMainWindow.formsTreeWidget->currentItem()->text(1);

		sentenceQuery.prepare("SELECT sentences.sentence FROM sentences, formSentence"
            " WHERE formSentence.formId = ("
            "  SELECT forms.rowid FROM forms WHERE forms.form = :form LIMIT 1"
            ") AND"
			" sentences.rowid = formSentence.sentenceId AND"
			" sentences.unparsable = 'true'");
		sentenceQuery.bindValue(":form", form);
        sentenceQuery.exec();
	}

    d_sentenceModel.setQuery(sentenceQuery);
}

void MinerMainWindow::updateStatistics()
{
	QSqlQuery formsQuery;
	formsQuery.exec("SELECT COUNT(*) FROM forms");
	formsQuery.next();
	QString formFreq = formsQuery.value(0).toString();
	d_minerMainWindow.statusbar->showMessage("Database contains " + formFreq + " forms");
}

void MinerMainWindow::writeSettings()
{
	QSettings settings("RUG", "Mining Viewer");

	// Window geometry
	settings.setValue("pos", pos());
	settings.setValue("size", size());

	// Splitter
	settings.setValue("splitterSizes", d_minerMainWindow.splitter->saveState());
}
