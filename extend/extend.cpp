#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>

#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QSet>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>

/*
 * WARNING: this is just a proof of concept, containing a lot of duplicate
 * code and assumptions. Rewrite for serious use :p.
 */

typedef QSet<int> IndexSet;
typedef QHash<QString, QSharedPointer<IndexSet> > PositionHash;

inline QSharedPointer<IndexSet> defaultSet()
{
  return QSharedPointer<IndexSet>(new IndexSet);
}

struct Positions {
  PositionHash wordPositions;
  PositionHash tagPositions;
};

struct Unigram {
  Unigram () {}
  Unigram(QString const &newType, QString const &newUnigram) :
      type(newType), unigram(newUnigram) {}
  QString type;
  QString unigram;
};

inline bool operator==(Unigram const &u1, Unigram const &u2)
{
  return u1.type == u2.type && u1.unigram == u2.unigram;
}

inline uint qHash(Unigram const &unigram)
{
  uint h1 = qHash(unigram.type);
  uint h2 = qHash(unigram.unigram);
  return ((h1 << 16) | (h1 >> 16)) ^ h2;
}

template <typename T>
QSharedPointer<QSet<T> > intersect_set(QSharedPointer<QSet<T> > a,
    QSharedPointer<QSet<T> > b) {
  QSharedPointer<QSet<T> > small;
  QSharedPointer<QSet<T> > big;

  if (a->size() > b->size()) {
    big = a;
    small = b;
  } else {
    big = b;
    small = a;
  }

  QSharedPointer<QSet<T> > inter(new QSet<T>);
  for (typename QSet<T>::const_iterator iter = small->begin();
      iter != small->end();
      ++iter)
    if (big->contains(*iter))
      inter->insert(*iter);

  return inter;
}

typedef QPair<Unigram, Unigram> Bigram;

typedef QHash<Bigram, QSharedPointer<IndexSet> > BigramCache;

double expansionFactor(int badFreq) {
  return 1.0 + exp(-0.5 * static_cast<double>(badFreq));
}

bool readCorpus(QTextStream &corpusStream, Positions *positions) {
  int position = 0;
  QString line;

  while (true) {
    line = corpusStream.readLine();
    if (line.isNull())
      break;

    QStringList lineParts = line.split(" ", QString::SkipEmptyParts);

    for (QStringList::const_iterator iter = lineParts.begin();
        iter != lineParts.end(); ++iter) {
      QString wordTag = *iter;

      int sepIndex = wordTag.lastIndexOf("/");
      QString word(wordTag.left(sepIndex));
      QString tag(wordTag.mid(sepIndex + 1));

      if (positions->wordPositions[word].isNull())
        positions->wordPositions[word] = QSharedPointer<IndexSet>(new IndexSet);
      positions->wordPositions[word]->insert(position);
      if (positions->tagPositions[tag].isNull())
        positions->tagPositions[tag] = QSharedPointer<IndexSet>(new IndexSet);
      positions->tagPositions[tag]->insert(position);

      ++position;
    }

  }

  return true;
}


double sequenceRatio(BigramCache *goodCache, BigramCache *badCache,
    Positions const &goodPositions, Positions const &badPositions,
    QVector<Unigram> seq)
{
  bool fromCache = false;

  QSharedPointer<IndexSet> goodIdx(new IndexSet);
  QSharedPointer<IndexSet> badIdx(new IndexSet);

  if (seq.size() > 1) {
      BigramCache::iterator badIter;
      BigramCache::iterator goodIter;
      Bigram bigram(seq[0], seq[1]);
      if ((badIter = badCache->find(bigram)) !=
          badCache->end() &&
          (goodIter = goodCache->find(bigram)) !=
           goodCache->end()) {
        badIdx = *badIter;
        goodIdx = *goodIter;
        seq.pop_front();
        seq.pop_front();
        fromCache = true;
      }
  }

  for (int i = 0; i < seq.size(); ++i) {
   
    PositionHash const *goodHash = 0;
    PositionHash const *badHash = 0;

    if (seq[i].type == "w") {
      goodHash = &goodPositions.wordPositions;
      badHash = &badPositions.wordPositions;
    } else {
      goodHash = &goodPositions.tagPositions;
      badHash = &badPositions.tagPositions;
    }

    if (i == 0 && !fromCache) {
      goodIdx = goodHash->value(seq[i].unigram, defaultSet());
      badIdx = badHash->value(seq[i].unigram, defaultSet());
    } else {

      QSharedPointer<IndexSet> newGoodIdx(new IndexSet);
      for (IndexSet::const_iterator iter = goodIdx->begin();
            iter != goodIdx->end(); ++iter)
        newGoodIdx->insert(*iter + 1);

      QSharedPointer<IndexSet> newBadIdx(new IndexSet);
      for (IndexSet::const_iterator iter = badIdx->begin();
            iter != badIdx->end(); ++iter)
        newBadIdx->insert(*iter + 1);

      goodIdx = intersect_set(newGoodIdx, goodHash->value(seq[i].unigram,
        defaultSet()));

      badIdx = intersect_set(newBadIdx, badHash->value(seq[i].unigram,
        defaultSet()));

      if (i == 1 && !fromCache && (goodIdx->size() > 5 || badIdx->size() > 5)) {
        Bigram bigram(seq[0], seq[1]);
        (*goodCache)[bigram] = goodIdx;
        (*badCache)[bigram] = badIdx;
      }
    }
  }

  if (goodIdx->size() + badIdx->size() == 0)
    return 0.0;

  return static_cast<double>(badIdx->size())
    / (goodIdx->size() + badIdx->size());
}

void expandCorpus(QTextStream *corpusStream, Positions const &goodPositions,
    Positions const &badPositions, QTextStream *sentStream)
{
  BigramCache goodBigramCache;
  BigramCache badBigramCache;

  int count = 0;

  QString line;
  while (true) {
    line = corpusStream->readLine();
    if (line.isNull())
      break;
   
    QStringList lineParts = line.split(" ", QString::SkipEmptyParts);

    QVector<Unigram> tags;
    QVector<Unigram> words;
    for (QStringList::const_iterator iter = lineParts.begin();
        iter != lineParts.end(); ++iter) {
      QString wordTag = *iter;

      int sepIndex = wordTag.lastIndexOf("/");
      QString word(wordTag.left(sepIndex));
      QString tag(wordTag.mid(sepIndex + 1));

      words.push_back(Unigram("w", word));
      tags.push_back(Unigram("t", tag));
    }

    for (int i = 0; i < words.size(); ++i) {
      // First word suspicion
      QSharedPointer<IndexSet> goodIdx =
        goodPositions.wordPositions.value(words[i].unigram, defaultSet());
      QSharedPointer<IndexSet> badIdx =
        badPositions.wordPositions.value(words[i].unigram, defaultSet());
      double susp = static_cast<double>(badIdx->size()) /
        (badIdx->size() + goodIdx->size());

      QVector<Unigram> ngram;
      ngram.push_back(words[i]);

      for (int j = i + 1; j < words.size(); ++j) {
        QSharedPointer<IndexSet> tagGoodIdx(new IndexSet);
        QSharedPointer<IndexSet> tagBadIdx(new IndexSet);
        QSharedPointer<IndexSet> newTagGoodIdx(new IndexSet);
        QSharedPointer<IndexSet> newTagbadIdx(new IndexSet);

        bool fromCache = false;
        if (ngram.size() == 1) {
            BigramCache::const_iterator goodIter;
            BigramCache::const_iterator badIter;
            if ((goodIter = goodBigramCache.find(Bigram(ngram[0], tags[j]))) !=
                goodBigramCache.end() &&
                (badIter = badBigramCache.find(Bigram(ngram[0], tags[j]))) !=
                badBigramCache.end()) {
              tagGoodIdx = *goodIter;
              tagBadIdx = *badIter;
              fromCache = true;
            }
        }

        if (!fromCache) {
          QSharedPointer<IndexSet> newGoodIdx(new IndexSet);
          for (IndexSet::const_iterator iter = goodIdx->begin();
              iter != goodIdx->end(); ++iter)
            newGoodIdx->insert(*iter + 1);

          QSharedPointer<IndexSet> newBadIdx(new IndexSet);
          for (IndexSet::const_iterator iter = badIdx->begin();
              iter != badIdx->end(); ++iter)
            newBadIdx->insert(*iter + 1);

          tagGoodIdx = intersect_set(newGoodIdx,
              goodPositions.tagPositions.value(tags[j].unigram, defaultSet()));

          tagBadIdx = intersect_set(newBadIdx,
            badPositions.tagPositions.value(tags[j].unigram, defaultSet()));

          if (ngram.size() == 1 && (tagGoodIdx->size() > 5 || tagBadIdx->size() > 5)) {
            Bigram bigram(ngram[0], tags[1]);
            goodBigramCache[bigram] = tagGoodIdx;
            badBigramCache[bigram] = tagBadIdx;
          }
        }

        double newTagSusp = 0.0;
        if (tagBadIdx->size() > 0)
          newTagSusp = static_cast<double>(tagBadIdx->size()) /
            (tagBadIdx->size() + tagGoodIdx->size());

        double ef = expansionFactor(tagBadIdx->size());

        QVector<Unigram> secondGram = ngram.mid(1);
        secondGram.push_back(tags[j]);
        double susp2 = sequenceRatio(&goodBigramCache, &badBigramCache,
            goodPositions, badPositions, secondGram);

        if (newTagSusp > susp * ef && newTagSusp > susp2 * ef) {
          ngram.push_back(tags[j]);
          goodIdx = tagGoodIdx;
          badIdx = tagBadIdx;
          susp = newTagSusp;
          continue;
        }

        ////////////////////
        // Word expansion //
        ////////////////////
        
        QSharedPointer<IndexSet> wordGoodIdx(new IndexSet);
        QSharedPointer<IndexSet> wordBadIdx(new IndexSet);

        fromCache = false;
        if (ngram.size() == 1) {
            BigramCache::const_iterator goodIter;
            BigramCache::const_iterator badIter;
            if ((goodIter = goodBigramCache.find(Bigram(ngram[0], words[j]))) !=
                goodBigramCache.end() &&
                (badIter = badBigramCache.find(Bigram(ngram[0], words[j]))) !=
                badBigramCache.end()) {
              wordBadIdx = *badIter;
              wordGoodIdx = *goodIter;
              fromCache = true;
            }
        }

        if (!fromCache) {
          QSharedPointer<IndexSet> newGoodIdx(new IndexSet);
          for (IndexSet::const_iterator iter = goodIdx->begin();
              iter != goodIdx->end(); ++iter)
            newGoodIdx->insert(*iter + 1);

          QSharedPointer<IndexSet> newBadIdx(new IndexSet);
          for (IndexSet::const_iterator iter = badIdx->begin();
              iter != badIdx->end(); ++iter)
            newBadIdx->insert(*iter + 1);

          wordGoodIdx = intersect_set(newGoodIdx,
            goodPositions.wordPositions.value(words[j].unigram, defaultSet()));

          wordBadIdx = intersect_set(newBadIdx,
            badPositions.wordPositions.value(words[j].unigram, defaultSet()));

          if (ngram.size() == 1 && (wordGoodIdx->size() > 5 || wordBadIdx->size() > 5)) {
            Bigram bigram(ngram[0], words[1]);
            goodBigramCache[bigram] = wordGoodIdx;
            badBigramCache[bigram] = wordBadIdx;
          }
        }

        double newWordSusp = 0.0;
        if (wordBadIdx->size() > 0)
          newWordSusp = static_cast<double>(wordBadIdx->size()) /
            (wordBadIdx->size() + wordGoodIdx->size());

        ef = expansionFactor(wordBadIdx->size());

        secondGram = ngram.mid(1);
        secondGram.push_back(words[j]);
        susp2 = sequenceRatio(&goodBigramCache, &badBigramCache,
            goodPositions, badPositions, secondGram);

        if (newWordSusp > susp * ef && newWordSusp > susp2 * ef) {
          ngram.push_back(words[j]);
          goodIdx = wordGoodIdx;
          badIdx = wordBadIdx;
          susp = newWordSusp;
          continue;
        }

        break;
      }

      QStringList ngramFlat;
      for (QVector<Unigram>::const_iterator iter = ngram.begin();
          iter != ngram.end(); ++iter)
        ngramFlat.push_back(iter->unigram);
      QString ngramStr(ngramFlat.join("_"));

      *sentStream << ngramStr << " ";
      QTextStream out(stdout);
      out << ngramStr << " ";
    }

    count +=1;
    if (count % 2 == 0) {
      QTextStream out(stdout);
      out << ".";
     }

     *sentStream << "\n";
     QTextStream out(stdout);
     out << "\n";
  }
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  if (argc != 4)
    return 1;

  QFile goodCorpus(argv[1]);
  goodCorpus.open(QFile::ReadOnly);
  QTextStream goodStream(&goodCorpus);

  QFile badCorpus(argv[2]);
  badCorpus.open(QFile::ReadOnly);
  QTextStream badStream(&badCorpus);

  QFile sents(argv[3]);
  sents.open(QFile::WriteOnly);
  QTextStream sentStream(&sents);

  QTextStream err(stderr);
  err << "Constructing indextables..." << endl;
  Positions goodPositions;
  readCorpus(goodStream, &goodPositions);

  Positions badPositions;
  readCorpus(badStream, &badPositions);
  
  err << "Expanding..." << endl;
  badStream.seek(0);
  expandCorpus(&badStream, goodPositions, badPositions, &sentStream);
}
