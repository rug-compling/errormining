#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <QString>

QString const VENDOR = "RUG";
QString const APPLICATION = "Mining Viewer";

QString const SUSP_THRESHOLD_SETTING = "suspThreshold";
QString const AVG_MULTIPLIER_SETTING = "avgMultiplier";
QString const THRESHOLD_METHOD_SETTING = "thresholdMethod";
QString const UNPARSABLE_FREQ_THRESHOLD_SETTING = "unparsableFreqThreshold";
QString const FREQ_THRESHOLD_SETTING = "parsableFreqThreshold";

QString const AVG_MULTIPLIER_METHOD = "avgMultiplierMethod";
QString const SUSP_THRESHOLD_METHOD = "suspThresholdMethod";

QString const DEEP_FORM_REMOVAL = "deepFormRemoval";

double const SUSP_THRESHOLD_SETTING_DEFAULT = 0.1;
double const AVG_MULTIPLIER_SETTING_DEFAULT = 1.5;
QString const SUSP_THRESHOLD_METHOD_DEFAULT = SUSP_THRESHOLD_METHOD;
uint const UNPARSABLE_FREQ_THRESHOLD_DEFAULT = 1;
uint const FREQ_THRESHOLD_DEFAULT = 1;
bool const DEEP_FORM_REMOVAL_DEFAULT = false;

#endif // GLOBAL_HH
