/**********************************
DEEPA CHARI
FALL 2012
CS 490 - SENIOR PROJECT
YALE COLLEGE
**********************************/

#include "header.hpp"	// class declarations
using namespace std;

void Analysis::print_grouping (unsigned int groupsz) {
	
	outmap::const_iterator it (alg->outputs.f.begin());
	int c (it->second.size());
	vector<pair<string, float> >* grouping = 
			new vector<pair<string, float> >[c];
	
	for (outmap::const_iterator it (alg->outputs.g.begin()); 
			it != alg->outputs.g.end();
			++it)
	{
		int index (0);
		float max (0);
		for (vector<float>::const_iterator vit (it->second.begin());
				vit != it->second.end();
				++vit)
		{
			if (*vit > max) {
				max = *vit;
				index = vit - it->second.begin();
			}
		}
		grouping[index].push_back(pair<string, float>(it->first, max));
	}
	
	Sortbyfreq sortbyfreq(*this);
	Sortbyprob sortbyprob(*this);
	for (int i (0); i < c; ++i) {
		sort(grouping[i].begin(), grouping[i].end(), sortbyfreq);
		vector<pair<string, float> >::iterator start;
		if (groupsz < grouping[i].size()) {
			start = grouping[i].begin() + groupsz;
		}
		else {
			start = grouping[i].end();
		}
		grouping[i].erase(start, grouping[i].end());
		sort(grouping[i].begin(), grouping[i].end(), sortbyprob);
	}
	
	for (int i (0); i < c; ++i) {
		cout << "Class " << i << ": ";
		for (vector<pair<string, float> >::const_iterator it 
				(grouping[i].begin());
				it != grouping[i].end(); 
				++it) 
		{
			cout << it->first << "(" << alg->train.counts[it->first] << ") ";
		}
		cout << endl;
	}
	
}
void Analysis::generate (int nwords, int seed) const {
	
	cout << nwords << " words generated from model: ";
	srand(SEED);
	
	// randomly pick the first word
	int w (rand() % alg->outputs.f.size());
	cout << alg->train.words[w] << ' ';
	
	for (int niter(0); niter < nwords; ++niter) {
		// randomly select a class for word w (weighted by probabilities)
		// and the next word based on that class (weighted)
		int m = select_weighted(alg->outputs.g[alg->train.getword(w)]);
		vector<float> fvec;
		for (outmap::const_iterator it (alg->outputs.f.begin());
				it != alg->outputs.f.end();
				++it)
		{
			fvec.push_back(it->second[m]);
		}
		w = select_weighted(fvec);
		cout << alg->train.getword(w) << ' ';
	}
	cout << endl;
	
}
void Analysis::tag (stringvec sentence) const {
  int L = sentence.size() -1,bestid,i;
  string token;
  float bestscore,s;
  vector<float> w1,w2;

  for(i=0;i<L;i++)
    {
      bestscore = 0;
      bestid = 0;

      for(int j=0;j< NWORDS; j++)
	{
	  if(alg->outputs.g.find( sentence[i]) == alg->outputs.g.end())
	    w1 = w1 = alg->outputs.g[ "*UNKNOWN*" ];
	  else
	    w1 = alg->outputs.g[ sentence[i] ];

	  if(alg->outputs.f.find( sentence[i+1]) == alg->outputs.f.end())
	      w2 = alg->outputs.f[ "*UNKNOWN*" ];
	  else
	    w2 = alg->outputs.f[ sentence[i+1] ];
	  s = w1[j] * w2[j];

	  if(s > bestscore)
	    {
	      bestscore = s;
	      bestid = j;
	    }
	}
      //cout << ">>>>>" << sentence[i] << " " << bestid << " " << bestscore << endl;
      cout << sentence[i] << " " << bestid  <<  endl;
    }
  bestscore = 0;
  bestid = 0;
  for(int j=0;j< NWORDS; j++)
    {
      if(alg->outputs.g.find( sentence[i]) == alg->outputs.g.end())
	w1 = w1 = alg->outputs.g[ "*UNKNOWN*" ];
      else
	w1 = alg->outputs.g[ sentence[i] ];

      s = w1[j];
      //      cout << sentence[i] << " " <<  " " << j << " " << s << " " << bestscore << endl;
      if(s > bestscore)
	{
	  bestscore = s;
	  bestid = j;
	}
    }
  //    cout << ">>>>>" << sentence[i] << " " << bestid << " " << bestscore << endl;
  cout << sentence[i] << "\t" << bestid << endl;

}

// declared static in header file
int Analysis::select_weighted (vector<float> vec) {

	float r = ((float)rand()) / RAND_MAX;
	float cumsum = 0;
	vector<float>::const_iterator it (vec.begin());
	for ( ; it != vec.end(); ++it)
	{
		cumsum += *it;
		if (cumsum >= r) {
			break;
		}
	}

	return (it == vec.end() ? it - vec.begin() - 1 : it - vec.begin());

}
