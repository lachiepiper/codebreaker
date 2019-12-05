# codebreaker
Contains my solution to Melbourne University Subject COMP30023 - Computer Systems Project 2

Specifications for this program:
See file password_cracker_v2.pdf

Instructions to Run this program:

1. compile in bash using command 'make'.

2. run crack.c executable on command line, adding the appropriate number of arguments

2.1. If crack is run with no arguments, it should generate guesses, and test them against the SHA256 hashes. When it finds
      one, it should print one line, consisting of the plaintext password, a space, and an integer 1-30 indicating which hash 
      it matches. 
      For example:
      abcd 3 Oops 5 adam 1 passwd 15
      
2.2 If crack is run with one integer argument, the argument specifies how many guesses it should produce. In this mode,
      password guesses should be printed to stdout, separated by newline characters (\n).

2.3 If crack is run with two argument, it should treat the first as the filename of a list of passwords (one per line), and 
      the second as the filename of a list of SHA256 hashes (in groups of 32 bytes, with no newline characters). It should           then test each of the passwords against each of the hashes, and produce output as for the case of no arguments. It 
      should print nothing else to stdout. If you want to produce other output, send it to stderr.
