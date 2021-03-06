Error mining in parsing results
===============================
Daniel de Kok <me@danieldk.eu>

Introduction
------------

This program implements error mining in parsing results, as described
in [1] and [2]. It is used to find n-grams that are suspicious for
causing incomplete parses. In addition to unigrams and bigrams, larger
problematic ngrams can also be mined.

A viewer for the results of the error mining is also included. The
error miner produces a plain text file with suspicious forms. These
forms can be stored in a sqlite database, along with unparsable
sentences. The viewer provides a comfortable interface to browse the
mining database.

Compilation
-----------

Requirements:

- Qt 4.5 or later - Qt4 is included with most GNU/Linux distributions.
  Installers for Mac OS X and Microsoft Windows are available from:

  http://www.qtsoftware.com/

Tested configurations:

- Debian GNU/Linux (Lenny), g++ 4.3.2, Qt 4.5.0
- Mac OS X 10.5, g++ 4.0.1, Qt 4.5.2

Qt includes a tool for generating native build infrastructure, named
'qmake'. Executing 'qmake' in the main source directory will generate
all necessary build files. Subsequently, 'make' can be invoked on most
UNIX platforms to compile the error miner and viewer. If you want to
create Makefiles, rather than an Xcode project on Mac OS X, invoke
qmake with the '-spec macx-g++' option.

To use the error miner, you will need perfect hash automata of the
types occurring in parsable and unparsable sentences. These automata
can be created with the fsa_build utility from Jan Daciuk [3].

Creating a perfect hash automaton
---------------------------------

The miner requires perfect hash automata for all types that occur
in parsable and unparsable sentences. An automaton can be created
in the following manner:

    tr -s '\012\011 ' '\012' < sentence_file | \
      LANG=POSIX LC_ALL=POSIX sort -u | fsa_build -N -o dict.fsa

This step should be performed for creating an automaton for parsable
sentences, and one for unparsable sentences.

Mining
------

Usage information can be obtained by running the miner without any
options:

    ./mine

If you don't want to adjust the default parameters, you can start
mining parses by providing a perfect hash automaton, and files
with parsable and unparsable sentences:

    ./mine parsable.fsa unparsable.fsa parsable-sentences unparsable-sentences

The output format consists of the following elements:

    [ngram] [suspicion] [f(ngram)] [f_unparsable(ngram)]

For example:

    me blijft 1.0 4 4

It is recommended to use the '-s threshold' option to exclude forms
with a very low suspicion, and the '-e factor' option to use an
expansion factor. A good default is:

    ./mine -s 0.001 -e 1.0 parsable.fsa unparsable.fsa parsable-sentences \
      unparsable-sentences

Viewing
-------

The viewer requires a SQLite database with all forms and unparsable
sentences. You can create this database with the 'createminedb'
utility:

    bin/createminedb mining_results unparsable_sentences minedb

This will create the 'minedb' database file. The database can now
be explored with the miningviewer:

    bin/miningviewer minedb

If you want to use the 'miningeval' tool for evaluation, you will have
to add relevant parsable sentences to the database as well:

 bin/createminedb mining_results parsable_sentences unparsable_sentences \
 	minedb

Example
-------

A small Wikipedia-based sample corpus and a Makefile that can be used
to automate mining, is included in the 'Examples' directory. After
setting the paths in the Makefile, you can mine the corpus, and view
the mining results with:

 cd Examples
 make nlwikipedia-sample.db
 ../bin/miningviewer nlwikipedia-sample.db

Licensing
---------

- Jan Daciuk's Fadd library, which is included in a slightly modified form
  in the 'fadd' directory, is licensed under the LGPL.
- The error miner and viewer are licensed under the LGPL. Please note that
  the miner requires the Qt library, which is available under the LGPL 2.1
  license.
- The sample corpus is licensed under the GNU Free Documentation License,
  please refer to Examples/README for more information.

The license text of the GNU Lesser General Public License 2.1 is provided
in the COPYING file.

Todo
----

- Separate preprocessor (n-gram expansion) and the miner.
- Clean up code and more error checking for database operations.

References
----------

1. Error mining in parsing results, Benoît Sagot and Éric de la Clergerie,
   Proceedings of the 21st International Conference on Computational
   Linguistics and the 44th annual meeting of the ACL, 2006
2. A generalized method for iterative error mining in parsing results,
   Daniël de Kok, Jianqiang Ma and Gertjan van Noord, ACL2009 Workshop
   Grammar Engineering Across Frameworks
3. http://www.jandaciuk.pl/fsa.html
