TEMPLATE = lib
TARGET = ../lib/mine
CONFIG += static debug_and_release warn_on
QT =
QMAKE_CXXFLAGS += -Wextra -I. -DFLEXIBLE -DNUMBERS -DSTOPBIT -DNEXTBIT \
	-DMORPH_INFIX -DPOOR_MORPH -DLOOSING_RPM -DMULTICOLUMN

SOURCES=fadd/fadd.cpp src/Form/Form.cpp \
	src/HashAutomaton/HashAutomaton.cpp src/HashedCorpus/HashedCorpus.cpp \
	src/Miner/Miner.cpp src/Observable/Observable.cpp \
	src/ProgramOptions/ProgramOptions.cpp src/ScoringMethod/ScoringMethod.cpp \
	src/Sentence/Sentence.cpp src/SuffixArray/SuffixArray.cpp \
	src/TokenizedSentenceReader/TokenizedSentenceReader.cpp \
	src/util/ssort/ssort.cpp

HEADERS=errormining/HashedCorpus.hh errormining/SentenceHandler.hh \
	errormining/SuffixArray.hh errormining/HashAutomaton.hh \
	errormining/Form.hh errormining/Miner.hh errormining/Observer.hh \
	errormining/ScoringMethod.hh errormining/Sentence.hh \
	errormining/TokenizedSentenceReader.hh errormining/util/ssort.hh \
	errormining/Observable.hh errormining/ProgramOptions.hh

# Internal headers
HEADERS+=src/Observable/Observable.ih src/HashedCorpus/HashedCorpus.ih \
	src/ProgramOptions/ProgramOptions.ih \
	src/TokenizedSentenceReader/TokenizedSentenceReader.ih \
	src/ScoringMethod/ScoringMethod.ih src/Sentence/Sentence.ih \
	src/HashAutomaton/HashAutomaton.ih src/SuffixArray/SuffixArray.ih \
	src/Miner/Miner.ih src/Form/Form.ih src/util/ssort/ssort.ih

mac {
	CONFIG -= app_bundle
}
