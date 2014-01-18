###AMM For POS Induction

I extend DEEPA CHARI's implementation of AMM. Following the fork source you can find her version
of the code. Now one can induce POS tags for word tokens. The input files consist of one sentences per line.
I provide new sample test and training input.

I optimized some part of the code since it takes excessive amounts of memory. Still,
for 1M token WSJ PTB corpus it takes 450GB memory. Obviously, there is still room for an 
improvement.

To compile and see command line options just run this:

     make PROG=amm
     ./amm

Command lines options are self-explanatory:

     usage: executable #OfClasses #OfIteration trainCorp testCorp option
     options : { p | i | gr | gen  }
       p : print testCorpus perplexity
       i : induce POS tags
       gr : print groupings
       gen : generate text

TODO:
- Add a better README file.
- Take random seed from command line
