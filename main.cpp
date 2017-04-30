#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define SEQ_NUM 50
#define PATTERN_LEN 15

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

/**
 *	讀入基因序列, 以Hash來記錄一些序列資訊; 並且用一個vector紀錄各Sequence內有的基因序列 .
 */
void preProcessing(map<string, PatternInfo> &patternInfoTable,SeqInfo *seqInfoTable,string fileName)
{
	ifstream fs(fileName);
    
    string line ;
    int controlSeqLength ;
    string subPattern ;
    PatternInfo patternInfo;
    int seq = 0 ;

    while(getline(fs, line))
    {
        controlSeqLength = line.length() ;

        for(int i = 0 ; i < controlSeqLength - PATTERN_LEN; i ++)
        {
        	subPattern = line.substr(i,PATTERN_LEN);

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
	int distance ;
	
	while(currentNode != NULL)
	{
		distance = patternDistance(pattern, currentNode->pattern);
		if(distance > 7)	// 7是極限, 設8的話花的時間瞬間增長, 但準確率高 .
			return false;
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


vector<string> reducedSeqPatterns(SeqInfo currentSeq,string rootPattern,int mutation)
{
	vector<string> result;
	int tolerance = (mutation * 2) + 1;

	for(int i = 0 ; i < currentSeq.patterns.size() ; i ++)
	{
		if(patternDistance(currentSeq.patterns.at(i), rootPattern) < tolerance)
			result.push_back(currentSeq.patterns.at(i));
	}

	return result;

}

/**
 * Insert (SeqNum)'th seqence's patterns to suitable bucket .
 */
void insertToBuckets(int seqNum,vector<Bucket> &buckets,SeqInfo *seqInfoTable,int mutation)
{
	SeqInfo currentSeq = seqInfoTable[seqNum];

	vector<Bucket>::iterator b_it = buckets.begin();
	while(b_it != buckets.end())
	{
		vector<string> reducedCurrentSeqPatterns = reducedSeqPatterns(currentSeq, (*b_it).rootPattern, mutation);

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

string generateRootPattern(vector<Bucket> &buckets)
{
	int pointingArray[4][PATTERN_LEN]  = { {0}, {0}, {0}, {0}};

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

	string target = "";

	for(int i = 0 ; i < PATTERN_LEN ; i ++)
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

	cout << "Target Pattern : " << target << endl;

	return target;

}

void showResult(SeqInfo *seqInfoTable,string s, int mutation)
{
	vector<string>::iterator it ;
	int pos[SEQ_NUM];
	int posCount ;
	
	for(int i = 0 ; i < SEQ_NUM ; i ++)
	{
		it = seqInfoTable[i].patterns.begin();
		posCount = 0 ;

		cout << "Seq " << i << " :{ ";

		while(it != seqInfoTable[i].patterns.end())
		{
			
			if( patternDistance(s,(*it)) <= mutation )
			{
				pos[i] = posCount;
				cout << "\"" << *it << "\" ,"<< posCount << "; "; 
				// break;
			}

			posCount ++ ;
			it ++ ;
		}

		cout << "}" << endl;
	}
}

void coreProcess(string fileName, int mutation)
{
	map<string, PatternInfo> patternInfoTable ;
    SeqInfo seqInfoTable[SEQ_NUM];
    
	preProcessing(patternInfoTable, seqInfoTable, fileName);
	vector<Bucket> buckets;
	initBucket(buckets, patternInfoTable, seqInfoTable);

    vector<Bucket> cloneBuckets ; // 用來收集最後剩下的Buckets .
	
	for(int i = 1 ; i < SEQ_NUM ; i ++)
	{
		insertToBuckets(i,buckets,seqInfoTable,mutation);
			
		// cout << i << "before - bucket # : " << buckets.size() << endl ;
		cloneBuckets.assign(buckets.begin(), buckets.end());
		removeUnupdateBucket(buckets);
		// cout << i << "after - bucket # : " << buckets.size() << endl ;
				
		updateBucketsLeaves(buckets,i);

		if(buckets.size() == 0)
			break;
	
	}

	string target = generateRootPattern(cloneBuckets);
	showResult(seqInfoTable, target, mutation);
}

void problemOneProcess()
{
	cout << "Question 1: " << endl;
	coreProcess("q1.data", 0);
	cout << "-------" << endl;
}

void problemTwoProcess()
{
	cout << "Question 2: " << endl;
	coreProcess("q2.data", 5);
	cout << "-------" << endl;
}

void problemThreeProcess()
{
	cout << "Question 3: " << endl;
	coreProcess("q3.data", 7);
	cout << "-------" << endl;
}

int main(int argc, const char * argv[]) 
{
    problemOneProcess();
    problemTwoProcess();
    problemThreeProcess();

  	return 0;
}

