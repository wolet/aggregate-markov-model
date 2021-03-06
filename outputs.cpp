#include <cstdlib>		// for srand
#include "header.hpp"	// class declarations

using namespace std;

void Outputs::normalize (vector<float>& vec, float sum) {

	for (vector<float>::iterator it = vec.begin(); it != vec.end(); ++it) 
	{
		*it /= sum;
	}
	
}

// fill f and g with random floats
void Outputs::initialize (stringvec const& words, int c, int seed) {

	srand(seed);
	float* fsum = new float[c];
	
	for (stringvec::const_iterator it (words.begin()); it != words.end(); ++it) 
	{
	
		float gsum (0);
		
		for (int i (0); i < c; ++i) {
		
			float frand = ((float)rand() / RAND_MAX);
			float grand = ((float)rand() / RAND_MAX);
			
			f[*it].push_back(frand);
			g[*it].push_back(grand);
			
			fsum[i] += frand;
			gsum += grand;
			
		}
		
		normalize(g[*it], gsum);
		
	}
	
	//normalize f
	for (int i (0); i < c; ++i) {
		for (outmap::iterator it = f.begin(); it != f.end(); ++it) {
			it->second[i] /= fsum[i];
		}
	}
	
}

void Outputs::update_f (outmap& oldf, float smooth) {

	float dec (smooth / oldf.size());
	for (outmap::iterator it (oldf.begin()); it != oldf.end(); ++it) {
		for (vector<float>::iterator vit (it->second.begin());
				vit != it->second.end();
				++vit)
		{
			if (*vit < dec) {
				*vit = 0;
			}
			else {
				*vit -= dec;
			}
		}
	}
	
}

void Outputs::create_test_outputs 
		(stringvec const& words, Outputs& old, float smooth, int seed) 
{
	
	srand(seed);
	outmap::const_iterator it = old.f.begin();
	int c (it->second.size());

	for (stringvec::const_iterator it (words.begin()); it != words.end(); ++it) 
	{
		// if the word isn't already in the old maps...
		if (old.f.find(*it) == old.f.end()) {
		
			update_f(old.f, smooth);
			
			// fill f and g
			float sum (0);
			for (int i (0); i < c; ++i) {
				f[*it].push_back(smooth);
				float grand ((float)rand() / RAND_MAX);
				g[*it].push_back(grand);
				sum += grand;
			}
			
			normalize(g[*it], sum);
			
		}
	}
	
	f.insert(old.f.begin(), old.f.end());
	g.insert(old.g.begin(), old.g.end());
	
}

void Outputs::print_f () const {

	outmap::const_iterator it = f.begin();
	int c = it->second.size();

	cout << "f: \n" << setprecision(3) << fixed;
	for ( ; it != f.end(); ++it) {
		cout << it->first << '\t';
	}
	cout << endl;
	
	for (int i (0); i < c; ++i) {
		float sum (0);
		for (it = f.begin(); it != f.end(); ++it) {
			sum += it->second[i];
			cout << it->second[i] << '\t';
		}
		cout << "\t| " << sum << endl;
	}
	
}

void Outputs::print_g () const {

	cout << "g: \n" << setprecision(3) << fixed;
	for (outmap::const_iterator it = g.begin(); it != g.end(); ++it) {
		float sum (0);
		cout << it->first << '\t';
		for (vector<float>::const_iterator vit = it->second.begin();
				vit != it->second.end();
				++vit)
		{
			sum += *vit;
			cout << *vit << '\t';
		}
		cout << "\t| " << sum << endl;
	}
	
}

/*
// unit test - debugging
int main (int argc, char** argv) {

	using namespace std;
	
	int c = atoi(argv[1]);
	ifstream ftrain(argv[2]);
	ifstream ftest(argv[3]);
	Corpus* train = new Corpus(c, ftrain);
	Outputs o(train);
	o.printout();
	
}
*/
