/**********************************
DEEPA CHARI
FALL 2012
CS 490 - SENIOR PROJECT
YALE COLLEGE
**********************************/

#include <cmath>		// for log
#include <cstring>		// for memcpy
#include <ctime>		// for debugging the algorithm
#include "header.hpp"	// class declarations

using namespace std;

inline float& Algorithm::Array3D::operator() (int i, int j, int m) {
  //	return arr[i*parent.v*parent.c + j*parent.c + m];
  return arr[i][j][m];
}

void Algorithm::Array3D::print () {

	for (int m=0; m<parent.c; ++m) {
		cerr << "m = " << m << ":\n";
		for (int i=0; i<parent.v; ++i) {
			for (int j=0; j<parent.v; ++j) {
				cerr << operator()(i, j, m) << ' ';
			}
			cerr << endl;
		}
	}

}

// e-step
void Algorithm::Run () {

  //	Outputs old(train);
  //	float old_perplexity, new_perplexity (calc_perplexity(train, outputs));
        float old_perplexity(0), new_perplexity(0);
	clock_t start, end;

	float* fdenoms;
	float* gdenoms;
	iter = 0;
	#if (DEBUG)
		cerr << "Run beginning\n";
		cerr << "v = " << v << "; c = " << c << endl;

	#endif

	Array3D tmp(*this);
	cerr << "#iter\tPerplexity\tTime\t\tTest Perplexity\n";
	do {

		++iter;
		//		old.copyfrom(outputs);

		// calculate full table of intermediate values (for speedup)
		calc_h(tmp);

		// calculate denominators
		fdenoms = new float[c];
		gdenoms = new float[v];
		for (int m=0; m<c; ++m) {
			fdenoms[m] = 0;
			for (int j=0; j<v; ++j) {
				for (int k=0; k<v; ++k) {
					fdenoms[m] += tmp(j, k, m);
				}
			}
		}
		for (int i=0; i<v; ++i) {
			gdenoms[i] = 0;
			for (int j=0; j<v; ++j) {
				for (int n=0; n<c; ++n) {
					gdenoms[i] += tmp(i, j, n);
				}
			}
		}

		// go through each possible word-class pair
		for (int i=0; i<v; ++i) {

			for (int m=0; m<c; ++m) {

				string w (train.getword(i));

				// compute f and g for word i and class m
				float fnum(0), gnum(0);
				for (int j=0; j<v; ++j) {
					fnum += tmp(j, i, m);
					gnum += tmp(i, j, m);
				}

				// update the arrays
				outputs.f[w][m] = fnum / fdenoms[m];
				outputs.g[w][m] = gnum / gdenoms[i];

			}

		}
		delete(fdenoms);
		delete(gdenoms);
		//old_perplexity = new_perplexity;
		//      		new_perplexity = calc_perplexity(train, outputs);
		#if (DEBUG)
           		cerr << iter << endl;
		#endif
	} while (!isConverged(old_perplexity, new_perplexity));
}

// computes the probability that word i belongs to class m given that word i
// precedes word j <-- M-STEP
void Algorithm::calc_h (Array3D& tmp) {

	for (int i=0; i<v; ++i) {
		for (int j=0; j<v; ++j) {
			float ct (train.paircount(i, j));
			vector<float> const& fvec (outputs.f[train.getword(j)]);
			vector<float> const& gvec (outputs.g[train.getword(i)]);
			float denom (inner_product(fvec.begin(), fvec.end(), 
					gvec.begin(), 0.0));
			for (int m=0; m<c; ++m) {
				tmp(i, j, m) = ((fvec[m] * gvec[m]) / denom) * ct;
			}
		}
	}

}

// computes the log likelihood of the test corpus
float Algorithm::loglk (Corpus& c, Outputs const& o) {

	float result (0);

	for (outmap::const_iterator git (o.g.begin()); git != o.g.end(); ++git) {
		string w1 (git->first);
		vector<float> v1 (git->second);
		for (outmap::const_iterator fit (o.f.begin()); fit != o.f.end(); ++fit) 
		{
			string w2 (fit->first);
			vector<float> v2 (fit->second);
			float tprob (inner_product(v1.begin(), v1.end(), v2.begin(), 0.0));
			result += c.paircount(w1, w2) * log(tprob);
		}

	}
	return result;

}

// tests for convergence
bool Algorithm::isConverged (float old, float neww, float eps) 
{
  	float change ((old - neww) / old);
	return NITER == 0 ? change < eps : iter == NITER;
}
