FSABUILD=/storage/dekok/aps/32/bin/fsa_build
EMPATH=../

%.mined : %.oks %.mistakes %.oks-fsa %.mistakes-fsa
	$(EMPATH)/bin/mine -v -s 0.001  $*.oks-fsa $*.mistakes-fsa $*.oks $*.mistakes > $*.mined

%.db : %.mined %.mistakes
	$(EMPATH)/bin/createminedb $*.mined $*.mistakes $*.db

%.oks-fsa : %.oks
	cat $*.oks | tr -s '\012\011 ' '\012' |\
         LANG=POSIX LC_ALL=POSIX sort -u |\
         $(FSABUILD) -N -o $*.oks-fsa

%.mistakes-fsa : %.mistakes
	cat $*.mistakes | tr -s '\012\011 ' '\012' |\
         LANG=POSIX LC_ALL=POSIX sort -u |\
         $(FSABUILD) -N -o $*.mistakes-fsa

%.oks : %.q
	zcat $*.q | $(ALPINO_HOME)/bin/q -o | sort -u > $*.oks

%.mistakes : %.q
	zcat $*.q | $(ALPINO_HOME)/bin/q -m | sort -u > $*.mistakes
