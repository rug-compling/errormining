#include "PreferencesDialog.ih"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
	d_preferencesDialog.setupUi(this);

	d_preferencesDialog.suspThresholdLineEdit->setValidator(new QDoubleValidator(0.0, 1.0, 3, this));
	d_preferencesDialog.avgMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, 1000.0, 3, this));
	d_preferencesDialog.unparsableFreqThresholdLineEdit->setValidator(
		new QIntValidator(1, std::numeric_limits<int>::max(), this));
	d_preferencesDialog.freqThresholdLineEdit->setValidator(
		new QIntValidator(1, std::numeric_limits<int>::max(), this));

	readSettings();
}

void PreferencesDialog::accept()
{
	// Read settings from the dialog.
	QString suspThreshold = d_preferencesDialog.suspThresholdLineEdit->text();
	QString avgMultiplier = d_preferencesDialog.avgMultiplierLineEdit->text();
	QString thresholdMethod = d_preferencesDialog.suspThresholdRadioButton->isChecked() ?
		SUSP_THRESHOLD_METHOD : AVG_MULTIPLIER_METHOD;
	QString unparsableFreqThreshold =
		d_preferencesDialog.unparsableFreqThresholdLineEdit->text();
	QString freqThreshold =	d_preferencesDialog.freqThresholdLineEdit->text();

	// Store settings.
	QSettings settings(VENDOR, APPLICATION);
	settings.setValue(SUSP_THRESHOLD_SETTING, suspThreshold);
	settings.setValue(AVG_MULTIPLIER_SETTING, avgMultiplier);
	settings.setValue(THRESHOLD_METHOD_SETTING, thresholdMethod);
	settings.setValue(UNPARSABLE_FREQ_THRESHOLD_SETTING, unparsableFreqThreshold);
	settings.setValue(FREQ_THRESHOLD_SETTING, freqThreshold);

	close();
}

void PreferencesDialog::readSettings()
{
	QSettings settings("RUG", "Mining Viewer");

	// Retrieve preferences.
	QString suspThreshold = settings.value(SUSP_THRESHOLD_SETTING,
		SUSP_THRESHOLD_SETTING_DEFAULT).toString();
	QString avgMultiplier = settings.value(AVG_MULTIPLIER_SETTING,
		AVG_MULTIPLIER_SETTING_DEFAULT).toString();
	QString suspThresholdMethod = settings.value(THRESHOLD_METHOD_SETTING,
		AVG_MULTIPLIER_METHOD).toString();
	QString unparsableFreqThreshold =
		settings.value(UNPARSABLE_FREQ_THRESHOLD_SETTING,
			UNPARSABLE_FREQ_THRESHOLD_DEFAULT).toString();
	QString freqThreshold =	settings.value(FREQ_THRESHOLD_SETTING,
		FREQ_THRESHOLD_DEFAULT).toString();

	d_preferencesDialog.suspThresholdLineEdit->setText(suspThreshold);
	d_preferencesDialog.avgMultiplierLineEdit->setText(avgMultiplier);
	d_preferencesDialog.unparsableFreqThresholdLineEdit->setText(unparsableFreqThreshold);
	d_preferencesDialog.freqThresholdLineEdit->setText(freqThreshold);

	if (suspThresholdMethod == SUSP_THRESHOLD_METHOD)
		d_preferencesDialog.suspThresholdRadioButton->setChecked(true);
	else
		d_preferencesDialog.avgMultiplierRadioButton->setChecked(true);
}
