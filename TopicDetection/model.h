#ifndef MODEL_H
#define MODEL_H

#include "DataProcessing.h"
#include <algorithm>
#include <cmath>

#define SESSION 10
#define ACTION	11


class state {
public:
	vector<int>		clickedHistory;
	bool operator< (const state& s) const;	
};

class model {
private:
	int			nHistory;
	int			nValues;

	map<state, double* >*			transition;
	map<state, double* >*			Qvalues;
	map<state, vector<int> >*		topicDomains;

public:
	static model* initialize(int nValues, int nHistory);
	static void modelEstimation(vector<model* >* MDPs, string fileName, vector<map<string, int> >* mapping, vector<int> featureIndexes);
	void valueIteration();
	void modelNormalization();
	void computingQvalues(map<state, double> *valueFunction, double gamma);
	void topicDetermining(int a);
	void topicDetermining2(int a);
};

#endif
