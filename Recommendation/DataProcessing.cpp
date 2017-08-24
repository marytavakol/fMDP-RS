#include "DataProcessing.h"


void items::initialize(string fileName) {
	fstream		fs;	
	int		i, temp;

	fs.open("config.txt", std::ios::in);
	fs >> nFeatures;
	for(i = 0; i < nFeatures; i++)
	{		
		fs >> temp;		
		featureIndexes.push_back(temp);
	}
	fs >> nHistory;	
	fs.close();
	readDomainValues(fileName);
}

void items::readDomainValues(string fileName) {
	fstream				fs;
	char				line[1024];
	int				i, cnt = 0;
	vector<int>			IDs;

	for(i = 0; i < nFeatures; i++)
		IDs.push_back(0);

	fs.open(fileName.c_str(), std::ios::in);
	while(!fs.eof())
	{		
		cnt++;
		fs.getline(line, 1024);
		if(strcmp(line, "") == 0)
			break;
		vector<string> tokens;	
		tokens.push_back(strtok(line, "\t"));
		for (i = 0; i < 13; i++)
			tokens.push_back(strtok(NULL, "\t"));

		for(i = 0; i < nFeatures; i++)
		{
			if(cnt == 1)
			{
				map<string, int>		temp;
				temp.insert(make_pair(tokens[featureIndexes[i]], IDs[i])).second;
				IDs[i]++;
				featureValuesMapping.push_back(temp);
			}
			else
			{
				bool p = featureValuesMapping[i].insert(make_pair(tokens[featureIndexes[i]], IDs[i])).second;
				if(p)
				{
					IDs[i]++;
				}
			}
		}
	}
	fs.close();
}

void items::readItemAttributes(string fileName)
{
	fstream		fs;
	char 		line[1024];
	int		i;

	fs.open(fileName.c_str(), ios::in);
	while(!fs.eof())
	{
		fs.getline(line, 1024);
		if(strcmp(line, "") == 0)
			break;
		vector<string> tokens;
		tokens.push_back(strtok(line, "\t"));
		for (i = 0; i < 13; i++)
			tokens.push_back(strtok(NULL, "\t"));

		vector<int> attr;
		for(i = 0; i < nFeatures; i++)
			attr.push_back(featureValuesMapping[i].find(tokens[featureIndexes[i]].c_str())->second);
		
		itemAttrMapping.insert(make_pair(tokens[0].c_str(), attr));
	}
}
