#include "header.hpp"	// class declarations

void printError()
{
  std::cout << "usage: executable #OfClasses #OfIteration trainCorp testCorp option\n";
  std::cout << "options : { p | i | gr | gen  }\n";
  std::cout << " p : print testCorpus perplexity\n i : induce POS tags\n gr : print groupings\n gen : generate text\n";
  exit(0);
}
int main(int argc, char** argv) {

	using namespace std;

	/**************************** read command line **************************/
        std::set<std::string>OPT ={"p","i","gen","gr"};
	std::set<std::string>tasks;
	if (argc < 6)printError();
	for(int i=5;i<argc;i++)
	  {
	    if(OPT.find(argv[i]) == OPT.end())printError();
	    tasks.insert(argv[i]);
	  }

	int c = atoi(argv[1]);
	int it = atoi(argv[2]);
	ifstream ftrain(argv[3]);
	ifstream ftest(argv[4]);


	/**************************** initialize objects **************************/
	Corpus *train, *test;
	try {
		train = new Corpus(c, ftrain);
		if(tasks.find("p") != tasks.end())
		  test = new Corpus(c, ftest, train);
	}
	catch (logic_error& e) {
		cout << e.what();
		exit(1);
	}

	Outputs out(*train);
	Algorithm alg(*train, *test, out,it);

	alg.Run();
	if(tasks.find("p") != tasks.end())
	  {
        	cout << " After " << it << " iteration(s) with "
			<< "perplexity " << alg.train_perplexity() << endl;
	        cout << "Perplexity of test set: " << alg.test_perplexity() << endl;
	  }
	Analysis an(&alg);
	if(tasks.find("gr") != tasks.end())
	  {
	    cerr << "Groupings are:" << endl;
	    an.print_grouping(10);
	  }
	if(tasks.find("gen") != tasks.end())
	  {
	    cerr << "Generation:" << endl;
	    an.generate();
	  }

	if(tasks.find("i") != tasks.end())
	  {
	    string token,line;
	    stringvec s;
	    int sent(0);
	    ifstream ftest(argv[4]);

	    cerr << "Sentence are being processed:" << endl;
	    while(getline( ftest, line ))
	      {
		istringstream lstream(line);

		while(lstream >> token)
		  s.push_back(token);
		cerr << ++sent << endl;
		an.tag(s);
		s.clear();
	      }
	  }
}
