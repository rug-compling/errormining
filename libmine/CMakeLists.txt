set(LIBMINE_SOURCES
  fadd/fadd.cpp
  src/BestRatioExpander.cpp
  src/Form/Form.cpp
  src/HashAutomaton/HashAutomaton.cpp
  src/HashedCorpus/HashedCorpus.cpp
  src/Miner/Miner.cpp
  src/Observable/Observable.cpp
  src/ScoringMethod/ScoringMethod.cpp
  src/Sentence/Sentence.cpp
  src/SimpleExpander.cpp
  src/SuffixArray/SuffixArray.cpp
  src/TokenizedSentenceReader/TokenizedSentenceReader.cpp
  src/util/ssort/ssort.cpp
)

set(LIBMINE_HEADERS
  errormining/BestRatioExpander.hh
  errormining/Expander.hh
  errormining/HashedCorpus.hh
  errormining/SentenceHandler.hh
  errormining/SuffixArray.hh
  errormining/HashAutomaton.hh
  errormining/Form.hh
  errormining/Miner.hh
  errormining/Observer.hh
  errormining/ScoringMethod.hh
  errormining/Sentence.hh
  errormining/SimpleExpander.hh
  errormining/TokenizedSentenceReader.hh
  errormining/util/ssort.hh
  errormining/Observable.hh
)  

add_library(mine SHARED
  ${LIBMINE_HEADERS}
  ${LIBMINE_SOURCES}
)

target_link_libraries(mine ${QT_QTCORE_LIBRARY})

