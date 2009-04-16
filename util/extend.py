#!/usr/bin/python
#
# Word and part of speech expansion preprocessor script.
#

import math
import sys

def extractWordTags(line):
    wordTags = []

    return wordTags

def expansionFactor(badFreq):
    return 1 + math.exp(-0.5 * float(badFreq))

def readCorpus(filename):
    position = 0
    words = dict()
    tags = dict()

    corpusFile = open(filename, 'r')

    for line in corpusFile:
        lineParts = line.strip().split()

        for part in lineParts:
            (word, tag) = part.rsplit('/', 1)

            if not words.has_key(word):
                words[word] = set()
            words[word].add(position)

            if not tags.has_key(tag):
                tags[tag] = set()
            tags[tag].add(position)

            position += 1

    return (words, tags)

def extractWordTags(line):
    lineParts = line.strip().split()
    return map(lambda x: x.rsplit('/', 1), lineParts)

def expandCorpus(filename, wordsOk, tagsOk, wordsErr, tagsErr, sentFile, formFile):
    corpusFile = open(filename, 'r')
    for line in corpusFile:
        wordTags = extractWordTags(line)
        
        for i in range(len(wordTags)):
            ngram = [wordTags[i][0]]
            okIdx = wordsOk.get(wordTags[i][0], set())
            errIdx = wordsErr[wordTags[i][0]]
            susp = float(len(errIdx)) / (len(errIdx) + len(okIdx))
            
            for j in range(i + 1, len(wordTags)):
                newOkIdx = set(map(lambda x: x + 1, okIdx))
                newErrIdx = set(map(lambda x: x + 1, errIdx))

                # Expand with a tag?
                tag = wordTags[j][1]
                tagOkIdx = tagsOk.get(tag, set()).intersection(newOkIdx)
                tagErrIdx = tagsErr.get(tag, set()).intersection(newErrIdx)

                if len(tagErrIdx) + len(tagOkIdx) == 0:
                    newSusp = 0.0
                else:
                    newSusp = float(len(tagErrIdx)) / (len(tagErrIdx) + len(tagOkIdx))

                ef = expansionFactor(len(tagErrIdx))

                if newSusp > susp * ef:
                    ngram.append(tag)
                    okIdx = tagOkIdx
                    errIdx = tagErrIdx
                    susp = newSusp
                    continue

                # Try word expansion
                word = wordTags[j][0]
                wordOkIdx = wordsOk.get(word, set()).intersection(newOkIdx)
                wordErrIdx = wordsErr.get(word, set()).intersection(newErrIdx)

                if len(wordErrIdx) + len(wordOkIdx) == 0:
                    break
                else:
                    newSusp = float(len(wordErrIdx)) / (len(wordErrIdx) + len(wordOkIdx))

                ef = expansionFactor(len(wordErrIdx))

                if newSusp > susp:
                    ngram.append(word)
                    okIdx = wordOkIdx
                    errIdx = wordErrIdx
                    susp = newSusp
                    continue

                break

            ngramStr = ('_').join(ngram)
            formFile.write("%s %f %d %d\n" % (ngramStr, susp, len(okIdx), len(errIdx)))
            sentFile.write(('_').join(ngram))
            sentFile.write(' ')

        sentFile.write('\n')
                    


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print "Usage: %s good bad sent_out forms_out" % sys.argv[0]
        sys.exit(1)

    sys.stderr.write("Constructing hashtables...")
    (wordsOk, tagsOk) = readCorpus(sys.argv[1])
    (wordsErr, tagsErr) = readCorpus(sys.argv[2])
    sys.stderr.write(" done!\n")

    sentFile = open(sys.argv[3], "w")
    formFile = open(sys.argv[4], "w")

    expandCorpus(sys.argv[2], wordsOk, tagsOk, wordsErr, tagsErr, sentFile,
                 formFile)
