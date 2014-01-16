/**********************************
DEEPA CHARI
FALL 2012
CS 490 - SENIOR PROJECT
YALE COLLEGE
**********************************/

#pragma once

#define _USE_MATH_DEFINES
#include <algorithm>	// for std::sort, std::find, std::inner_product
#include <cmath>		// for algorithm logic
#include <fstream>		// to read in words
#include <iostream>		// for debugging
#include <sstream>
#include <iomanip>		// for print formatting
#include <map>			// to represent pair counts 
#include <set>
#include <stdexcept>	// for exception thrown by Corpus::getwords()
#include <vector>		// to represent words

#define SEED		1
#define NWORDS 		12
#define DEBUG		1
#define GROUPSIZE	5
#define NITER		10
#define SMOOTH		.00001
#define EPSILON		.005

typedef std::vector<std::string>                                stringvec;
typedef std::map<std::pair<std::string, std::string>, float>   wordmap;
typedef std::map<std::pair<int, int>, float>                   indexmap;
typedef std::map<std::string, std::vector<float> >             outmap;
typedef std::map<std::string, int>                              intmap;

class Corpus {

	friend class Analysis;	// needs to access words
	friend class Outputs;	// needs to access words and c
	friend class Algorithm;

	// data members
	stringvec words;		// vector of distinct words (vocabulary)
	intmap counts;          // vector of distinct counts
        intmap word2id;          // word 2 word id
	wordmap W;				// map of word pairs to counts (smoothed)
	indexmap I;				// map of index pairs to counts (smoothed)
	int v;					// number of words in vocabulary
	int c;					// number of classes
	int vtot;				// total number of words processed (w/ repeats)

  public:
	
	Corpus (int c, std::ifstream& fin, Corpus* old=NULL, float smooth=SMOOTH);
	inline float& paircount (int i, int j);
	inline float& paircount (std::string w1, std::string w2);
	int getc () const { return c; }
	int getv () const { return v; }
	int getvtot () const { return vtot; }
        std::string getword (int x) const { return words[x]; }
	void print () const;

  private:
  
	void readin (std::string word);
	std::string process (std::string word, Corpus* old);
	void getwords (std::ifstream& fin, Corpus* old, float smooth);
	
};

inline float& Corpus::paircount (std::string w1, std::string w2) {
	return W[std::pair<std::string, std::string>(w1, w2)];
}

inline float& Corpus::paircount (int i, int j) {
	return I[std::pair<int, int>(i, j)];
}

class Outputs {

	// friend classes need to access f and g
	friend class Algorithm;
	friend class Analysis;

	// data members
	outmap f;	// f[w][m] : probability that word w follows a word of class m
	outmap g;	// g[w][m] : probability that word w belongs to class m

  public:
  
	Outputs (Corpus const& corp) : f(), g() { initialize(corp.words, corp.c); }
	Outputs (Corpus const& corp, Outputs old) : f (), g () {
		create_test_outputs(corp.words, old);
	}
	void printout () const {
		print_f();
		print_g();
	}
	void copyfrom (Outputs const& orig) {
		f = orig.f;
		g = orig.g;
	}
	
  private:
  
	void normalize (std::vector<float>& vec, float sum);
	void initialize (stringvec const& words, int c, int seed=SEED);
	void update_f (outmap& oldf, float smooth);
	void create_test_outputs 
			(stringvec const& words, 
			Outputs& old, 
			float smooth=SMOOTH, 
			int seed=SEED);
	void print_f () const;
	void print_g () const;
	
};

class Algorithm {

	friend class Analysis;	// needs to access everything
	
	// 3D array to hold intermediate probabilities
	class Array3D {
	  	Algorithm& parent;
                float ***arr;
	  public:
	        // Array3D (Algorithm& p) 
		// : parent (p), arr (new float[p.v * p.v * p.c]) 
		// {}
	  Array3D (Algorithm& p): parent (p) {

	    	std::cerr << "Allocation started\n" ;

		arr = (float ***)malloc(p.v*sizeof(float **));

		std::cerr << "#First part allocated\n" ;
		for(int i=0;i<p.v;i++)
		  {
		    arr[i] = (float **)malloc(p.v*sizeof(float *));
		    for(int j=0;j<p.v;j++)
		      arr[i][j] = (float *)malloc(p.c*sizeof(float ));
		    if(i % (p.v/10) == 0)
		      std::cerr << 100*i/p.v <<" # ";
		  }
		std::cerr << "\n#Temp allocated\n";

	  }
	  //	  : parent (p), arr (){}
	  //	        ~Array3D(){delete(arr);}
		float& operator() (int i, int j, int m);
		void print ();
	};
	
	// data members
	Corpus& train;			// training words/counts
	Corpus& test;			// test words/counts
	Outputs& outputs;		// two maps to store results
	const int v;
	const int c;
	int iter;				// number of iterations taken by the algorithm
	
  public:
	
	Algorithm (Corpus& tr, Corpus& te, Outputs& o)
	: train(tr), test(te), outputs(o), v(tr.getv()), c(tr.getc()), iter()
	{}
	void Run ();
	int num_iter () const		{ return iter; }
	float train_perplexity ()	{ return calc_perplexity(train, outputs); }
	float test_perplexity () { 
		//Outputs test_outputs(test, outputs);
		//test_outputs.printout();
		return calc_perplexity(test, outputs); 
	}
	
  private:
  
	// logic to test for convergence
	void calc_h (Array3D& tmp);
	float transprob (int i, int j, float* f, float* g);
	float loglk (Corpus& c, Outputs const& o);
	float calc_perplexity (Corpus& c, Outputs const& o) {
		return pow(M_E, loglk(c, o) * -1.0 / c.getvtot()); 
	}
	bool isConverged (float old, float neww, float eps=EPSILON);
	
};

class Analysis {

	Algorithm* alg;
	const int v;
	const int c;
	class Sortbyfreq
	{
		Analysis& an;
	  public:
		Sortbyfreq (Analysis& an_) : an(an_) {}
		bool operator() (std::pair<std::string, float> x, 
				std::pair<std::string, float> y) const 
		{
			return (an.alg->train.counts[x.first] 
					> an.alg->train.counts[y.first]);
		}
	};
	class Sortbyprob
	{
		Analysis& an;
	  public:
		Sortbyprob (Analysis& an_) : an(an_) {}
		bool operator() (std::pair<std::string, float> x, 
				std::pair<std::string, float> y) const 
		{
			return x.second > y.second;
		}
	};

  public:

	Analysis (Algorithm* alg_) : alg(alg_), v(alg_->v), c(alg_->c) {
		#if (DEBUG)
			std::cerr << "Analysis initiated.\n";
		#endif
	}
	void print_grouping (unsigned int groupsz=GROUPSIZE);
	void generate (int nwords=NWORDS, int seed=SEED) const;
        void tag (stringvec sentence) const;

  private:

  	static int select_weighted (std::vector<float> vec);

};
