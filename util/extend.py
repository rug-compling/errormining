#!/usr/bin/python
#
# Word and part of speech expansion preprocessor script.

import math
import sys

from optparse import OptionParser

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

            try:
                words[word].add(position)
            except KeyError:
                words[word] = set()
                words[word].add(position)
                

            try:
                tags[tag].add(position)
            except KeyError:
                tags[tag] = set()
                tags[tag].add(position)

            position += 1

    return (words, tags)

def extractWordTags(line):
    lineParts = line.strip().split()
    return map(lambda x: x.rsplit('/', 1), lineParts)

def sequenceRatio(bigramCache, wordsOk, tagsOk, wordsErr, tagsErr, seq):
    types = map(lambda x: x[0], seq)
    flat = map(lambda x: x[1], seq)

    okIdx = None
    errIdx = None
    fromCache = False

    if len(seq) > 1:
        okIdx = bigramCache.get((seq[0], seq[1], True))
        errIdx = bigramCache.get((seq[0], seq[1], False))

    if okIdx != None and errIdx != None:
        seq = seq[2:]
        types = types[2:]
        flag = flat[2:]
        fromCache = True

    for i in range(len(seq)):
        if types[i] == 'w':
            okMap = wordsOk
            errMap = wordsErr
        else:
            okMap = tagsOk
            errMap = tagsErr

        if i == 0 and not fromCache:
            okIdx = okMap.get(flat[i], set())
            errIdx = errMap.get(flat[i], set())
        else:
            newOkIdx = set(map(lambda x: x + 1, okIdx))
            newErrIdx = set(map(lambda x: x + 1, errIdx))
            
            okIdx = newOkIdx.intersection(okMap.get(flat[i], set()))
            errIdx = newErrIdx.intersection(errMap.get(flat[i], set()))

            if i == 1 and not fromCache and (len(okIdx) > 5 or len(errIdx) > 5):
                bigramCache[(seq[0], seq[1], True)] = okIdx
                bigramCache[(seq[0], seq[1], False)] = errIdx

    if (len(okIdx) + len(errIdx)) == 0:
        return 0.0

    return float(len(errIdx)) / (len(okIdx) + len(errIdx))


def expandCorpus(filename, wordsOk, tagsOk, wordsErr, tagsErr, sentFile, formFile, debug):
    corpusFile = open(filename, 'r')
    bigramCache = dict()

    cnt = 0

    for line in corpusFile:
        wordTags = extractWordTags(line)
        
        for i in range(len(wordTags)):
            ngram = [('w',wordTags[i][0])]
            okIdx = wordsOk.get(wordTags[i][0], set())
            errIdx = wordsErr[wordTags[i][0]]
            susp = float(len(errIdx)) / (len(errIdx) + len(okIdx))
            
            for j in range(i + 1, len(wordTags)):
                tag = wordTags[j][1]

                tagOkIdx = None
                tagErrIdx = None

                newOkIdx = None
                newErrIdx = None

                if len(ngram) == 1:
                    tagOkIdx = bigramCache.get((ngram[0], ('t', tag), True))
                    tagErrIdx = bigramCache.get((ngram[0], ('t', tag), False))

                if tagOkIdx == None or tagErrIdx == None:
                    newOkIdx = set(map(lambda x: x + 1, okIdx))
                    newErrIdx = set(map(lambda x: x + 1, errIdx))

                    tagOkIdx = tagsOk.get(tag, set()) & newOkIdx
                    tagErrIdx = tagsErr.get(tag, set()) & newErrIdx

                    if len(ngram) == 1 and (len(tagOkIdx) > 5 or len(tagErrIdx) > 5):
                        bigramCache[(ngram[0], ('t', tag), True)] = tagOkIdx
                        bigramCache[(ngram[0], ('t', tag), False)] = tagErrIdx

                if len(tagErrIdx) == 0:
                    newTagSusp = 0.0
                else:
                    newTagSusp = float(len(tagErrIdx)) / (len(tagErrIdx) + len(tagOkIdx))

                ef = expansionFactor(len(tagErrIdx))


                secondGram = ngram[1:]
                secondGram.append(('t', tag))
                susp2 = sequenceRatio(bigramCache, wordsOk, tagsOk, wordsErr,
                                      tagsErr, secondGram)

                if newTagSusp > susp * ef and newTagSusp > susp2 * ef:
                    ngram.append(('t',tag))
                    okIdx = tagOkIdx
                    errIdx = tagErrIdx
                    susp = newTagSusp
                    continue

                # Try word expansion
                word = wordTags[j][0]

                wordOkIdx = None
                wordErrIdx = None

                if len(ngram) == 1:
                    wordOkIdx = bigramCache.get((ngram[0], ('w', word), True))
                    wordErrIdx = bigramCache.get((ngram[0], ('w', word), False))

                if wordOkIdx == None or wordErrIdx == None:
                    if newOkIdx == None:
                        newOkIdx = set(map(lambda x: x + 1, okIdx))
                    if newErrIdx == None:
                        newErrIdx = set(map(lambda x: x + 1, errIdx))

                    wordOkIdx = wordsOk.get(word, set()) & newOkIdx
                    wordErrIdx = wordsErr.get(word, set()) & newErrIdx

                    if len(ngram) == 1 and (len(wordOkIdx) > 5 or len(wordErrIdx) > 5):
                        bigramCache[(ngram[0], ('w', word), True)] = wordOkIdx
                        bigramCache[(ngram[0], ('w', word), False)] = wordErrIdx

                if len(wordErrIdx) == 0:
                    newWordSusp = 0.0
                else:
                    newWordSusp = float(len(wordErrIdx)) / (len(wordErrIdx) + len(wordOkIdx))

                ef = expansionFactor(len(wordErrIdx))

                secondGram = ngram[1:]
                secondGram.append(('w', word))
                susp2 = sequenceRatio(bigramCache, wordsOk, tagsOk, wordsErr,
                                      tagsErr, secondGram)


                if newWordSusp > susp * ef and newWordSusp > susp2 * ef:
                    ngram.append(('w',word))
                    okIdx = wordOkIdx
                    errIdx = wordErrIdx
                    susp = newWordSusp
                    continue

                break

            ngramFlat = map(lambda x: x[1], ngram)
            ngramStr = '_'.join(ngramFlat)
            formFile.write("%s %f %d %d\n" % (ngramStr, susp, len(okIdx), len(errIdx)))
        
            sentFile.write(ngramStr)
            sentFile.write(' ')

        cnt += 1
        if cnt % 100 == 0:
            print '.',
            sys.stdout.flush()

        sentFile.write('\n')
    print
                    

def run(options, args):
    if len(args) != 4:
        print "Usage: %s good bad sent_out forms_out" % sys.argv[0]
        sys.exit(1)

    sys.stderr.write("Constructing hashtables...")
    (wordsOk, tagsOk) = readCorpus(args[0])
    (wordsErr, tagsErr) = readCorpus(args[1])
    sys.stderr.write(" done!\n")

    sys.stderr.write("Expanding sentences...")
    sentFile = open(args[2], "w")
    formFile = open(args[3], "w")

    expandCorpus(args[1], wordsOk, tagsOk, wordsErr, tagsErr, sentFile,
                 formFile, options.debug)

    sys.stderr.write(" done!\n")

    sentFile.close()
    formFile.close()

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-d", action="store_true", dest="debug",
                      default=False, help = "Enable debugging output")
    (options, args) = parser.parse_args()

    run(options, args)
