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

double const SUSP_THRESHOLD_SETTING_DEFAULT = 0.5;
double const AVG_MULTIPLIER_SETTING_DEFAULT = 1.5;
uint const UNPARSABLE_FREQ_THRESHOLD_DEFAULT = 1;
uint const FREQ_THRESHOLD_DEFAULT = 1;

QString const AVG_MULTIPLIER_METHOD = "avgMultiplierMethod";
QString const SUSP_THRESHOLD_METHOD = "suspThresholdMethod";

#endif // GLOBAL_HH
