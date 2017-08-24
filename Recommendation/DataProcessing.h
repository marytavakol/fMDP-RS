#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <string>

using namespace std;


class items
{
public:
	int			nFeatures;
	int			nHistory;
	vector<int>		featureIndexes;

	vector<map<string, int> >		featureValuesMapping;
	map<string, vector<int> >		itemAttrMapping;
	
	void	initialize(string fileName);
	void	readDomainValues(string fileName);
	void	readItemAttributes(string fileName);
};


#endif
