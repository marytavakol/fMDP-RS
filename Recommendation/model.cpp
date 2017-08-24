#include "model.h"


bool state::operator< (const state& s) const{
	return (clickedHistory < s.clickedHistory);
}

model* model::initialize(int nValues, int nHistory) {
	model* initialModel = new model();
	initialModel->nHistory = nHistory;	
	initialModel->nValues = nValues;
	initialModel->transition = new map<state, double*>;
	return initialModel;
}

double getReward(string action) {
	double rew = 0;
	if(strcmp(action.c_str(), "VIEW") == 0)
		rew = 0;
	else if(strcmp(action.c_str(), "VIEWRECO") == 0)
		rew = 2;
	else if(strcmp(action.c_str(), "DELETE_FROM_CART") == 0)
		rew = -1;
	else if(strcmp(action.c_str(), "SALE") == 0)
		rew = 0;
	else if(strcmp(action.c_str(), "CART") == 0)
		rew = 1;
	else if(strcmp(action.c_str(), "DELETE_FROM_WISHLIST") == 0)
		rew = -1;
	else if(strcmp(action.c_str(), "WISH") == 0)
		rew = 1;
	return rew;
}

void model::modelEstimation(vector<model*>* MDPs, string fileName, vector<map<string, int> >* mapping, vector<int> featureIndexes) {
	fstream			fs;
	char			line[1024];
	string			lastSessionID = "", sessionID, action;
	vector<state>		currentState;
	int			i, j, nFeatures;

	nFeatures = MDPs->size();
	fs.open(fileName.c_str(), ios::in);
	while(!fs.eof())
	{		
		fs.getline(line, 1024);
		if(strcmp(line, "") == 0)
		{
			for (i = 0; i < nFeatures; i++)
			{
				map<state, double*>::iterator it = MDPs[0][i]->transition->find(currentState[i]);
				if(it == MDPs[0][i]->transition->end())
				{
					double* val = new double[MDPs[0][i]->nValues + 3];										
					for(j = 0; j < MDPs[0][i]->nValues + 3; j++)					
						val[j] = 0;												
					val[MDPs[0][i]->nValues + 2]++;						
					MDPs[0][i]->transition->insert(make_pair(currentState[i], val));
				}
				else											
					it->second[MDPs[0][i]->nValues + 2]++;										
			}
			break;
		}

		vector<string> tokens;
		tokens.push_back(strtok(line, "\t"));
		for(i = 0; i < 13; i++)
			tokens.push_back(strtok(NULL, "\t"));
		
		sessionID = tokens[SESSION];
		int*	clicked = new int[nFeatures];		
		for(i = 0; i < nFeatures; i++)
			clicked[i] = mapping[0][i].find(tokens[featureIndexes[i]].c_str())->second;
		action = tokens[ACTION];
		
		if(strcmp(sessionID.c_str(), lastSessionID.c_str()) != 0)
		{
			if(currentState.size() > 0)
			{
				for (i = 0; i < nFeatures; i++)
				{
					map<state, double*>::iterator it = MDPs[0][i]->transition->find(currentState[i]);
					if(it == MDPs[0][i]->transition->end())
					{
						double* val = new double[MDPs[0][i]->nValues + 3];						
						for(j = 0; j < MDPs[0][i]->nValues + 3; j++)						
							val[j] = 0;													
						val[MDPs[0][i]->nValues + 2]++;						
						MDPs[0][i]->transition->insert(make_pair(currentState[i], val));
					}
					else											
						it->second[MDPs[0][i]->nValues + 2]++;										
				}
			}
			currentState.clear();
			for (i = 0; i < nFeatures; i++)
			{
				state st;
				for (j = 0; j < MDPs[0][i]->nHistory; j++)
					st.clickedHistory.push_back(-1);					
				currentState.push_back(st);
			}			
		}
		
		//update:
		for(i = 0; i < nFeatures; i++)
		{			
			map<state, double*>::iterator it = MDPs[0][i]->transition->find(currentState[i]);
			if(it == MDPs[0][i]->transition->end())
			{
				double* val = new double[MDPs[0][i]->nValues + 3];				
				for(j = 0; j < MDPs[0][i]->nValues + 3; j++)				
					val[j] = 0;													
				val[clicked[i]]++;
//				val[MDPs[0][i]->nValues] += getReward(action);
				val[MDPs[0][i]->nValues + 1]++;
				MDPs[0][i]->transition->insert(make_pair(currentState[i], val));
			}
			else
			{				
				it->second[clicked[i]]++;
//				it->second[MDPs[0][i]->nValues] += getReward(action);
				it->second[MDPs[0][i]->nValues + 1]++;				
			}						
		}			

		for(i = 0; i < nFeatures; i++)
		{
			for(j = 1; j < currentState[i].clickedHistory.size(); j++)
				currentState[i].clickedHistory[j - 1] = currentState[i].clickedHistory[j];
			currentState[i].clickedHistory[j - 1] = clicked[i];

			map<state, double*>::iterator it = MDPs[0][i]->transition->find(currentState[i]);
			if(it == MDPs[0][i]->transition->end())
			{
				double* val = new double[MDPs[0][i]->nValues + 3];				
				for(j = 0; j < MDPs[0][i]->nValues + 3; j++)				
					val[j] = 0;
				val[MDPs[0][i]->nValues] += getReward(action);
			}
			else
				it->second[MDPs[0][i]->nValues] += getReward(action);
		}
		lastSessionID = sessionID;
	}
	fs.close();
}

void model::modelNormalization() 
{	
	int j, sum = 0;
	for(map<state, double*>::iterator itr = this->transition->begin(); itr != this->transition->end(); ++itr)
	{		
		int total = itr->second[this->nValues + 1] + itr->second[this->nValues + 2];
		for(j = 0; j < this->nValues; j++)		
		{			
			itr->second[j] = (double)itr->second[j]/total;		
		}
		itr->second[this->nValues] = (double)itr->second[this->nValues]/total;
	}
}

void model::valueIteration()
{
	int		i, j, k;
	double  	theta = 0.001;
	double  	gamma = 0.95;
	double		alpha = 2, beta = 0.001;

	map<state, double> *valueFunction = new map<state, double>;
	for (map<state, double*>::iterator it = this->transition->begin(); it != this->transition->end(); ++it)
		valueFunction->insert(make_pair(it->first, 0));

	while(true)
	{		
		double delta = 0;
		for (map<state, double>::iterator it = valueFunction->begin(); it != valueFunction->end(); ++it)
		{
			double temp = it->second;
			//comuting action values			
			double maxVal = -1000;			
			map<state, double*>::iterator it2 = this->transition->find(it->first);
			for(i = 0; i < this->nValues; i++)
			{
				double *tVal = new double[this->nValues];
				double sum = (double)it2->second[this->nValues + 2]/(it2->second[this->nValues + 1] + it2->second[this->nValues + 2]);
				sum = (double)sum/this->nValues; 
				for(j = 0; j < this->nValues; j++)
				{
					if(j == i)
						tVal[j] = alpha * it2->second[j];
					else
						tVal[j] = beta * it2->second[j];
					sum += tVal[j];
				}
				double Q = 0;
				for(j = 0; j < this->nValues; j++)
				{
					tVal[j] = (double)tVal[j]/sum;					
					if(tVal[j] > 0)
					{
						state nextState;				
						for(k = 1; k < this->nHistory; k++)
							nextState.clickedHistory.push_back(it->first.clickedHistory[k]);
						nextState.clickedHistory.push_back(j);							
						if(j == i)
							Q += tVal[j] * (this->transition->find(nextState)->second[this->nValues] + gamma * valueFunction->find(nextState)->second);
						else
							Q += tVal[j] * (-20 + gamma * valueFunction->find(nextState)->second);
					}
				}								
				delete tVal;
				if(Q > maxVal)
					maxVal = Q;
			}

			//computing new values
			it->second = maxVal;
			delta = max(delta, abs(temp - it->second));
		}
		if(delta < theta)
			break;
	}

	cout << "\nComputing Q Values..." << endl;
	computingQvalues(valueFunction, gamma);

	valueFunction->clear();
	delete valueFunction;
}

void model::computingQvalues(map<state, double> *valueFunction, double gamma)
{
	int		i, j, k;
	double  	alpha = 2, beta = 0.001;

	this->Qvalues = new map<state, double*>;
	for (map<state, double>::iterator it = valueFunction->begin(); it != valueFunction->end(); ++it)	
	{
		double	*temp = new double[this->nValues];
		this->Qvalues->insert(make_pair(it->first, temp));
	}
	for (map<state, double*>::iterator it = this->Qvalues->begin(); it != this->Qvalues->end(); ++it)
	{
		map<state, double*>::iterator it2 = this->transition->find(it->first);

		double summ= 0;
		for(i = 0; i < this->nValues; i++)
		{	
			double *tVal = new double[this->nValues];
			double sum = (double)it2->second[this->nValues + 2]/(it2->second[this->nValues + 1] + it2->second[this->nValues + 2]);
			sum = (double)sum/this->nValues; 
			for(j = 0; j < this->nValues; j++)
			{
				if(j == i)
					tVal[j] = alpha * it2->second[j];
				else
					tVal[j] = beta * it2->second[j];
				sum += tVal[j];
			}
			it->second[i] = 0;
			for(j = 0; j < this->nValues; j++)
			{
				tVal[j] = (double)tVal[j]/sum;
				if(tVal[j] > 0)
				{
					state nextState;				
					for(k = 1; k < this->nHistory; k++)
						nextState.clickedHistory.push_back(it->first.clickedHistory[k]);
					nextState.clickedHistory.push_back(j);
					if(j == i)
						it->second[i] += tVal[j] * (this->transition->find(nextState)->second[this->nValues] + gamma * valueFunction->find(nextState)->second);
					else
						it->second[i] += tVal[j] * (-20 + gamma * valueFunction->find(nextState)->second);
				}
			}
			summ += exp(it->second[i]);
		}
		for(i = 0; i < this->nValues; i++)
		{				
			if(summ != 0)
				it->second[i] = (double)exp(it->second[i])/summ;
		}
	}
}

void model::evaluator(vector<model* >* MDPs, string fileName, items* dataset)
{
	fstream			fs;
	char			line[1024];
	string			lastSessionID = "", sessionID;
	vector<state>		currentState;
	int			i, j, nFeatures, nHistory, timeStep;
	vector<fstream*>	outFiles;

	nFeatures = dataset->nFeatures;
	nHistory = dataset->nHistory;
	fs.open(fileName.c_str(), ios::in);
	while(!fs.eof())
	{
		fs.getline(line, 1024);
		if(strcmp(line, "") == 0)
			break;
		
		vector<string> tokens;
		tokens.push_back(strtok(line, "\t"));
		for(i = 0; i < 13; i++)
			tokens.push_back(strtok(NULL, "\t"));
		
		sessionID = tokens[SESSION];
		int*	clicked = new int[nFeatures];
		for(i = 0; i < nFeatures; i++)
			clicked[i] = dataset->featureValuesMapping[i].find(tokens[dataset->featureIndexes[i]].c_str())->second;

		if(strcmp(sessionID.c_str(), lastSessionID.c_str()) != 0)
		{
			timeStep = 1;
			currentState.clear();
			for (i = 0; i < nFeatures; i++)
			{
				state st;
				for (j = 0; j < nHistory; j++)
					st.clickedHistory.push_back(-1);					
				currentState.push_back(st);
			}
		}		
		else if(timeStep < 10)
		{
			timeStep++;
			//Ranking
			int rank = dataset->itemAttrMapping.size() + 1;
			vector<double*> Qvalues;
			bool flag = true;
			for(i = 0; i < nFeatures; i++)
			{
				map<state, double*>::iterator itemp = MDPs[0][i]->Qvalues->find(currentState[i]);
				if(itemp == MDPs[0][i]->Qvalues->end())
				{
					flag = false;
					break;
				}
				Qvalues.push_back(itemp->second);
			}
			if(flag)
			{
				double currScore = 1;
				for(i = 0; i < nFeatures; i++)
					currScore = currScore * Qvalues[i][clicked[i]];						
				for(map<string, vector<int> >::iterator itAt = dataset->itemAttrMapping.begin(); itAt != dataset->itemAttrMapping.end(); ++itAt)
				{		
					double s = 1;
					for(i = 0; i < nFeatures; i++)
						s = s * Qvalues[i][itAt->second[i]];
					if(currScore >= s)
						rank--;
				}
			}
			if(timeStep - 1 > outFiles.size())
			{
				char name[100];
				sprintf(name, "rank.t%d.dat", timeStep);
				fstream *os = new fstream(name, ios::out);
				*os << rank << endl;
				outFiles.push_back(os);
			}			
			else
				*(outFiles[timeStep - 2]) << rank << endl;
		}
		for(i = 0; i < nFeatures; i++)
		{
			for(j = 1; j < currentState[i].clickedHistory.size(); j++)
				currentState[i].clickedHistory[j - 1] = currentState[i].clickedHistory[j];
			currentState[i].clickedHistory[j - 1] = clicked[i];
		}
		lastSessionID = sessionID;
	}
	fs.close();
	for(i = 0; i < outFiles.size(); i++)
		outFiles[i]->close();
}


