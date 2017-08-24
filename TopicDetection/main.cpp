#include "model.h"

int main()
{
	int i;
	string fileName = "Data/train.dat";
	items dataset;
	dataset.initialize(fileName);
	cout << "\nValue Mappiing has been Constructed!" << endl;
	vector<model*>* MDPs;
	MDPs = new vector<model*>;
	for(i = 0; i < dataset.nFeatures; i++)
	{
		cout << "size of domain: " << dataset.featureValuesMapping[i].size() << endl;
		model* temp = model::initialize(dataset.featureValuesMapping[i].size(), dataset.nHistory);
		MDPs->push_back(temp);
	}	
	cout << "\nMDP Models have been Initialized!" << endl;
	model::modelEstimation(MDPs, fileName, &dataset.featureValuesMapping, dataset.featureIndexes);	
	cout << "\nModel has been estimated!" << endl;
	for(i = 0; i < dataset.nFeatures; i++)
	{
		cout << "\nNormalization..." << endl;
		MDPs[0][i]->modelNormalization();
		cout << "\nValue Iteration..." << endl;
		MDPs[0][i]->valueIteration();
		cout << "\nTopic Specification..." << endl;
		MDPs[0][i]->topicDetermining(dataset.featureIndexes[i]);
	}
	for(i = 0; i < dataset.nFeatures; i++)
		delete MDPs[0][i];
	delete MDPs;
	return 0;
}
