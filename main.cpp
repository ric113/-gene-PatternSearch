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
	ifstream fs("ex3_5_mutates.data");
    
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

	// cout << patternOne << " " << patternTwo << " " << distance << endl;

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

bool validNode(struct patternNode *leaf,string pattern)
{
	struct patternNode *currentNode = leaf;
	// cout << pattern << endl;
	
	while(currentNode != NULL)
	{
		if(patternDistance(pattern, currentNode->pattern) > 8)
			return false;
		currentNode = currentNode->parentNode;
	}

	return true;
}

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
		if(patternDistance(currentSeq.patterns.at(i), rootPattern) < 9)
			result.push_back(currentSeq.patterns.at(i));
	}

	return result;

}

/**
 * Insert (SeqNum)'th seqence's patterns to suitable bucket .
 */
void insertToBuckets(int seqNum,vector<Bucket> &buckets,map<string, PatternInfo> &patternInfoTable,SeqInfo *seqInfoTable)
{
	SeqInfo currentSeq = seqInfoTable[seqNum];

	vector<Bucket>::iterator b_it = buckets.begin();
	while(b_it != buckets.end())
	{
		vector<string> reducedCurrentSeqPatterns = reducedSeqPatterns(currentSeq, (*b_it).rootPattern);

		// cout << "After reduced size :" << reducedCurrentSeqPatterns.size() << endl;

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
						// cout << temp.parentNode->pattern << "," << temp.parentNode->depth << endl;

						(*b_it).updated = true;

						// tracePath(temp);
					}

				}
				
				// showTree((*l_it));
			} 

			p_it ++;
		}

		b_it ++;
	}



	/*
	vector<string>::iterator p_it = currentSeq.patterns.begin();
	while(p_it != currentSeq.patterns.end())
	{
		vector<Bucket>::iterator b_it = buckets.begin();
		while(b_it != buckets.end())
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
						// cout << temp.parentNode->pattern << "," << temp.parentNode->depth << endl;

						(*b_it).updated = true;

						// tracePath(temp);
					}

				}
				
				// showTree((*l_it));
			} 

			b_it ++;
		}
		p_it ++;
	}

	*/
}



int main(int argc, const char * argv[]) 
{

    // insert code here...
    map<string, PatternInfo> patternInfoTable ;
    SeqInfo seqInfoTable[SEQ_NUM];
    
	preProcessing(patternInfoTable, seqInfoTable);

	
	vector<Bucket> buckets;
    initBucket(buckets, patternInfoTable, seqInfoTable);

    // showBuckets(buckets);

	int searchSeqNum =  10;
	for(int i = 1 ; i <= searchSeqNum ; i ++)
	{
		 
		insertToBuckets(i,buckets,patternInfoTable,seqInfoTable);
		cout << i << "before - bucket # : " << buckets.size() << endl ;
		removeUnupdateBucket(buckets);
		cout << i << "after - bucket # : " << buckets.size() << endl ;
		
		//countBranch(buckets);
     	updateBucketsLeaves(buckets,i);
		//countBranch(buckets);
		// showBuckets(buckets);
		
	}
	showBuckets(buckets);
	// showCurrentBucketState(buckets);
	// showPatternAmoutInfo(patternInfoTable);
    // showSeqInfo(seqInfoTable);
    return 0;
}



