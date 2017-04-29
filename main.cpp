// bucket 中的leaves改用double pointer實作.


#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define SEQ_NUM 50

typedef struct positionInfo{
	int seq;		// 0 ~ 49 ;
	int index;
}PositionInfo;

typedef struct patternInfo{
	vector<PositionInfo> positionInfo;
}PatternInfo;

typedef struct seqInfo{
	vector<string> patterns ;
}SeqInfo;

struct patternNode{
	string pattern;
	int depth;			// 0 ~ 49 ;
	patternNode *parentNode;
};

typedef struct bucket{
	bool updated;
	vector<struct patternNode*> leaves;
	string rootPattern;

}Bucket;


void tracePath(struct patternNode *leaf)
{
	struct patternNode *currentNode = leaf;
	cout << "node tree :" << endl;
	cout << "leaf : " << leaf->pattern << endl;

	while(currentNode != NULL)
	{

		// cout << "In loop" << endl;
		cout << currentNode->pattern << ", depth :" << currentNode->depth << " - ";
		currentNode = currentNode->parentNode;
	}
	cout << endl;
}

void showSeqInfo(SeqInfo *seqInfoTable)
{

	vector<string>::iterator it ;

	for(int i = 0 ; i < SEQ_NUM ; i ++)
	{
		cout << "Seq : " << i << endl ;
		it = seqInfoTable[i].patterns.begin();

		while(it != seqInfoTable[i].patterns.end())
		{
			cout << *it << endl ;
			it ++ ;
		}

	}
}

void showBuckets(vector<Bucket> &buckets)
{
	vector<Bucket>::iterator it = buckets.begin();


	while(it != buckets.end())
	{
		vector<struct patternNode*>::iterator n_it = (*it).leaves.begin();
		while(n_it != (*it).leaves.end())
		{
		    
			tracePath((*n_it));
			n_it ++ ;
		}



		it ++;
	}
}

void showTree(struct patternNode &leaf)
{
	struct patternNode *currentNode = &leaf;
	// cout << pattern << endl;
	
	while(currentNode != NULL)
	{
		cout << currentNode->pattern <<  " depth : " << currentNode->depth << endl; 
		currentNode = currentNode->parentNode;
	}
}

/**
 * [preProcessing description]
 * initialize All tables .
 * @param patternInfoTable [store Pattern and its infomation]
 * @param seqInfoTable     [store pattern in seqences]
 */
void preProcessing(map<string, PatternInfo> &patternInfoTable,SeqInfo *seqInfoTable)
{
	ifstream fs("q3.data");
    
    string line ;
    int controlSeqLength ;
    string subPattern ;
    PatternInfo patternInfo;
    int seq = 0 ;

    while(getline(fs, line))
    {
        controlSeqLength = line.length() ;

        for(int i = 0 ; i < controlSeqLength - 15; i ++)
        {
        	subPattern = line.substr(i,15);

        	PositionInfo newPositionInfo;
        	newPositionInfo.seq = seq;
        	newPositionInfo.index = i;
        				
			patternInfoTable[subPattern].positionInfo.push_back(newPositionInfo) ;

			(seqInfoTable[seq].patterns).push_back(subPattern);
       	}

       	seq ++ ;

    }
    
    fs.close();

}

/**
 * 	Initialize buckets, put Seq1's pattern in as initial state .
 */

void initBucket(vector<Bucket> &buckets,map<string, PatternInfo> &patternInfoTable,SeqInfo *seqInfoTable)
{
	SeqInfo seqOne = seqInfoTable[0];
	vector<string>::iterator it = seqOne.patterns.begin();
	Bucket tempBucket;
	

	while(it != seqOne.patterns.end())
	{
		// cout << *it << endl;
		struct patternNode *newNode = new patternNode;
		newNode->pattern = (*it);
		newNode->depth = 0;
		newNode->parentNode = 0;

		tempBucket.leaves.push_back(newNode);
		tempBucket.updated = false;
		tempBucket.rootPattern = (*it);

		buckets.push_back(tempBucket);
		tempBucket.leaves.clear();
		it ++;
	}

}


/**
 * Compare distance between Two Seq .
 */
int patternDistance(string patternOne, string patternTwo)
{
	int patternLen = patternOne.length();
	int distance = 0 ;
	
	for(int i = 0 ; i < patternLen ; i ++)
	{
		if(patternOne[i] != patternTwo[i])
			distance ++ ;
	}

	return distance;
}

/**
 * Check bucket's updated property, if false, remove it !
 */
void removeUnupdateBucket(vector<Bucket> &buckets)
{
	vector<Bucket>::iterator it = buckets.begin();

	while(it != buckets.end())
	{
		if(!(*it).updated)
		{
			(*it).leaves.clear();
			buckets.erase(it);
		}
		else
		{
			(*it).updated = false;
			it ++;
		}
	}
}

/**
 * 走訪整個leaf到root的path, 依序檢查pattern是否與所有經過的node之distane小於threshold .
 */
bool validNode(struct patternNode *leaf,string pattern)
{
	struct patternNode *currentNode = leaf;
	int toleranceCount = 0 ;
	int distance ;
	
	while(currentNode != NULL)
	{
		distance = patternDistance(pattern, currentNode->pattern);
		if(distance > 7)
		{
			if(distance == 8 && toleranceCount < 0)
				toleranceCount ++ ;
			else
				return false;
		}
		currentNode = currentNode->parentNode;
	}

	return true;
}

/**
 * Delete掉沒有擴展的leaf .
 */
void updateBucketsLeaves(vector<Bucket> &buckets,int currentDepth)
{
	vector<Bucket>::iterator it = buckets.begin();
	while(it != buckets.end())
	{
		vector<struct patternNode*>::iterator n_it = (*it).leaves.begin();
		while(n_it != (*it).leaves.end())
		{
			if( (*n_it)->depth != currentDepth)
				(*it).leaves.erase(n_it);
			else
				n_it ++ ;
		}


		it ++;
	}
}

/**
 * Testing , 計算各bucket的leaf數 .
 */
void countBranch(vector<Bucket> &buckets)
{
	vector<Bucket>::iterator it = buckets.begin();
	int i = 0;
	while(it != buckets.end())
	{
		cout << "Bucket " << i << " : "<< (*it).leaves.size() << endl;
		i ++;
		it ++;
	}
}


vector<string> reducedSeqPatterns(SeqInfo currentSeq,string rootPattern)
{
	vector<string> result;

	for(int i = 0 ; i < currentSeq.patterns.size() ; i ++)
	{
		if(patternDistance(currentSeq.patterns.at(i), rootPattern) < 11)
			result.push_back(currentSeq.patterns.at(i));
	}

	return result;

}

/**
 * Insert (SeqNum)'th seqence's patterns to suitable bucket .
 */
void insertToBuckets(int seqNum,vector<Bucket> &buckets,SeqInfo *seqInfoTable)
{
	SeqInfo currentSeq = seqInfoTable[seqNum];

	vector<Bucket>::iterator b_it = buckets.begin();
	while(b_it != buckets.end())
	{
		vector<string> reducedCurrentSeqPatterns = reducedSeqPatterns(currentSeq, (*b_it).rootPattern);

		vector<string>::iterator p_it = reducedCurrentSeqPatterns.begin();
		while(p_it != reducedCurrentSeqPatterns.end())
		{
			int leavesAmount = (*b_it).leaves.size();
			
			for(int i = 0 ; i < leavesAmount ; i ++)
			{

				if(((*b_it).leaves.at(i))->depth == seqNum - 1)
				{
					if(validNode((*b_it).leaves.at(i), (*p_it)))
					{

						struct patternNode *temp = new patternNode ;

						temp->pattern = (*p_it);
						temp->depth = seqNum;
						temp->parentNode = ((*b_it).leaves.at(i));

						(*b_it).leaves.push_back(temp);

						(*b_it).updated = true;
						
					}

				}
			} 

			p_it ++;
		}

		b_it ++;
	}
}

void vectorAppend(vector<Bucket> &result, vector<Bucket> &appender)
{
	vector<Bucket>::iterator it = appender.begin();
	while(it != appender.end())
	{
		result.push_back(*it);
		it ++;
	}
}

string generateRootPattern(vector<Bucket> &buckets)
{
	int pointingArray[4][15]  = { {0}, {0}, {0}, {0}};

	vector<Bucket>::iterator b_it = buckets.begin();
	while(b_it != buckets.end())
	{
		vector<struct patternNode*>::iterator n_it = (*b_it).leaves.begin();
		while(n_it != (*b_it).leaves.end())
		{
		    
			struct patternNode *currentNode = *n_it;
			while(currentNode != NULL)
			{
				string currentPattern = currentNode->pattern;
				for(int i = 0 ; i < currentPattern.length() ; i ++)
				{
					if(currentPattern[i] == 'A')
						pointingArray[0][i] ++;
					else if(currentPattern[i] == 'T')
						pointingArray[1][i] ++;
					else if(currentPattern[i] == 'C')
						pointingArray[2][i] ++;
					else if(currentPattern[i] == 'G')
						pointingArray[3][i] ++;
				}

				currentNode = currentNode->parentNode;
			}

			n_it ++ ;
		}

		b_it ++ ;
	}


	for(int i = 0 ; i < 4 ; i ++)
	{
		for(int j = 0 ; j < 15 ; j ++)
		{
			cout << pointingArray[i][j] << " ";
		}

		cout << endl;
	}

	string target = "";

	for(int i = 0 ; i < 15 ; i ++)
	{
		int max = 0 ;
		for(int j = 1 ; j < 4 ; j ++)
		{
			max = pointingArray[j][i] > pointingArray[max][i] ? j:max;
		}

		if(max == 0)
			target += "A";
		else if(max == 1)
			target += "T";
		else if(max == 2)
			target += "C";
		else if(max == 3)
			target += "G";
	}

	cout << target << endl;

	return target;

}

void showCurrentBucket(Bucket &bucket)
{

	vector<struct patternNode*>::iterator n_it = bucket.leaves.begin();
	while(n_it != bucket.leaves.end())
	{
		tracePath((*n_it));
		n_it ++ ;
	}
}

void DFS_updateCurrentBucketLeaves(Bucket &bucket, int currentDepth)
{
	vector<struct patternNode*>::iterator n_it = bucket.leaves.begin();
	while(n_it != bucket.leaves.end())
	{
		if( (*n_it)->depth != currentDepth)
			bucket.leaves.erase(n_it);
		else
			n_it ++ ;
	}
}

void DFS_insertToBuckets(vector<Bucket> &buckets,SeqInfo *seqInfoTable)
{
	vector<Bucket>::iterator b_it = buckets.begin();
	int count = 1;

	while(b_it != buckets.end())
	{
		cout << "Bucket " << count << endl;

		for(int j = 1 ; j < SEQ_NUM ; j ++)
		{
			SeqInfo currentSeq = seqInfoTable[j];
			vector<string> reducedCurrentSeqPatterns = reducedSeqPatterns(currentSeq, (*b_it).rootPattern);

			vector<string>::iterator p_it = reducedCurrentSeqPatterns.begin();
			while(p_it != reducedCurrentSeqPatterns.end())
			{
				int leavesAmount = (*b_it).leaves.size();
				
				for(int i = 0 ; i < leavesAmount ; i ++)
				{
					

					if(((*b_it).leaves.at(i))->depth == j - 1)
					{
						if(validNode((*b_it).leaves.at(i), (*p_it)))
						{

							struct patternNode *temp = new patternNode ;

							temp->pattern = (*p_it);
							temp->depth = j;
							temp->parentNode = ((*b_it).leaves.at(i));

							(*b_it).leaves.push_back(temp);
							// cout << temp.parentNode->pattern << "," << temp.parentNode->depth << endl;

							(*b_it).updated = true;

							// tracePath(temp);
						}

					}
					
					// showTree((*l_it));
				} 

				p_it ++;
			}

			DFS_updateCurrentBucketLeaves(*b_it,j);
			// showCurrentBucket(*b_it);
			cout << "Current Bucket "<< count << "Seq :" << j <<" ,branch # :" << (*b_it).leaves.size() << endl;

			if((*b_it).leaves.size() == 0)
				break;

		}
		count ++ ;
		b_it ++;
	}
}

string generateRootPatternByCandidates(vector<string> &candidatePatterns)
{
	int pointingArray[4][15]  = { {0}, {0}, {0}, {0}};

	vector<string>::iterator it = candidatePatterns.begin();

	while( it != candidatePatterns.end())
	{
		string currentPattern = (*it);

		for(int i = 0 ; i < currentPattern.length() ; i ++)
		{
			if(currentPattern[i] == 'A')
				pointingArray[0][i] ++;
			else if(currentPattern[i] == 'T')
				pointingArray[1][i] ++;
			else if(currentPattern[i] == 'C')
				pointingArray[2][i] ++;
			else if(currentPattern[i] == 'G')
				pointingArray[3][i] ++;
		}

		it ++;
	}

	string target = "";

	for(int i = 0 ; i < 15 ; i ++)
	{
		int max = 0 ;
		for(int j = 1 ; j < 4 ; j ++)
		{
			max = pointingArray[j][i] > pointingArray[max][i] ? j:max;
		}

		if(max == 0)
			target += "A";
		else if(max == 1)
			target += "T";
		else if(max == 2)
			target += "C";
		else if(max == 3)
			target += "G";
	}

	cout << target << endl;

	return target;


}

void Testing(SeqInfo *seqInfoTable,string s)
{
	vector<string>::iterator it ;
	int pos[50];
	int posCount ;
	
	for(int i = 0 ; i < SEQ_NUM ; i ++)
	{
		it = seqInfoTable[i].patterns.begin();
		posCount = 0 ;

		while(it != seqInfoTable[i].patterns.end())
		{
			if( patternDistance(s,(*it)) <= 7)
			{
				pos[i] = posCount;
				cout << "Seq " << i << " : "<< posCount << ", pattern = " << *it <<endl;
				break;
			}

			posCount ++ ;
			it ++ ;
		}

		if(posCount == 986)
		{
			cout << "Not the target!" << endl;
			return;
		}

	}
}


int main(int argc, const char * argv[]) 
{

    // insert code here...
    map<string, PatternInfo> patternInfoTable ;
    SeqInfo seqInfoTable[SEQ_NUM];
    
	preProcessing(patternInfoTable, seqInfoTable);
	vector<Bucket> buckets;
	initBucket(buckets, patternInfoTable, seqInfoTable);

    // DFS_insertToBuckets(buckets,seqInfoTable);

    vector<Bucket> cloneBuckets ; // 用來收集最後剩下的Buckets .
	
	int searchSeqNum =  49;
	for(int i = 1 ; i <= searchSeqNum ; i ++)
	{
		insertToBuckets(i,buckets,seqInfoTable);
			
		// cout << i << "before - bucket # : " << buckets.size() << endl ;
		cloneBuckets.assign(buckets.begin(), buckets.end());
		removeUnupdateBucket(buckets);
		// cout << i << "after - bucket # : " << buckets.size() << endl ;
				
		updateBucketsLeaves(buckets,i);

		if(buckets.size() == 0)
			break;
	
	}

	cout << "-------" << endl;
	

	string target = generateRootPattern(cloneBuckets);
	Testing(seqInfoTable, target);

    return 0;
}



