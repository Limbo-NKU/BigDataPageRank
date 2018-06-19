#include<iostream>
#include<fstream>
#include<list>
#include<algorithm>
using namespace std;

//data structures
struct tempStruct
{
	int node;
	double rank;
};

struct matrixLine
{
	int fromNode;
	int degrees;
	int toNode;
};
//const variables
//counts maximun node number
int maxNode = 0;
//marks which node is valid
bool *valid;
//counts the number of valid node
int validNum = 0;
//counts the number of outnodes per node 
int *numOut;
//control the size of a block
int blockSize = 1000;
int blockNum = 0;
//pageRank algorithm parameters
double beta = 0.85;
double sigma = 0.0000000000001;
//data of now using block 
double* r_new;
double* r_old;
//list<matrixLine> matrix;
//一遍读取原数据，统计总节点数
void preLoad()
{
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		if (fromNode>maxNode)
		{
			maxNode = fromNode;
		}
		if (toNode>maxNode)
		{
			maxNode = toNode;
		}

	}
	cout << "preload complete" << endl;
	cout << "total nodes = " << maxNode << endl;
	cout << endl;
	//system("pause");
}
//第二遍读取原文件，统计每个节点的出度和是否有效 
void numOutCount()
{
	r_old = new double[maxNode];
	r_new = new double[maxNode];
	numOut = new int[maxNode];
	valid = new bool[maxNode];
	//初始化 出度数组和有效位数组 
	for (int i = 0; i<maxNode; i++)
	{
		numOut[i] = 0;
		valid[i] = false;
	}
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	//第二遍读取原文件，检测节点的有效性和出度 
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		valid[toNode - 1] = true;
		valid[fromNode - 1] = true;
		numOut[fromNode - 1]++;
	}
	//统计有效节点数量 
	for (int i = 0; i<maxNode; i++)
	{
		if (valid[i])
		{
			validNum++;
		}
	}
	//初始化rold，rnew数组 
	for (int i = 0; i<maxNode; i++)
	{
		if (valid[i])
		{
			r_old[i] = 1 / validNum;
		}
		else
		{
			r_old[i] = 0;
		}
		r_new[i] = 0;
	}
	cout << "count outnode number complete" << endl;
	cout << endl;
	//system("pause");
}
//将原文件分块保存
void blockSaving()
{
	blockNum = maxNode / blockSize + 1;
	ofstream* out = new ofstream[blockNum];
	char buf[50];
	for (int i = 0; i<blockNum; i++)
	{
		sprintf_s(buf, "matrixBlock%d.txt", i);
		out[i].open(buf);
	}
	ifstream in("Wikidata.txt");
	int fromNode, toNode;
	/*
	简单形式：
	while(!in.eof())
	{
	in>>fromNode>>toNode;
	out[fromNode/blockSize]<<fromNode<<" "<<numOut[fromNode]<<" "<<toNode<<endl;
	}
	*/
	int nowNode = -1,nowCount=0;
	list<int> toList;
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		if (nowNode != fromNode)
			//读取当前节点完成，先将原节点写入磁盘 
		{
			int tmp;
			while (!toList.empty())
			{
				tmp = toList.front();
				out[nowNode / blockSize] << nowNode << " " << numOut[nowNode - 1] << " " << tmp << endl;
				toList.pop_front();
			}
			nowNode = fromNode;
			nowCount = 0;
		}
		//节点数据写入队列 
		nowCount++;
		toList.push_back(toNode);
	}
	cout << "split blocks complete" << endl;
	cout << endl;
	//system("pause");
}
//读取块中数据
void readBlock(int n)
{
	//matrix.clear();
	char buf[50];
	sprintf_s(buf, "matrixBlock%d.txt", n);
	ifstream in(buf);
	int fromNode, degree, toNode;
	while (!in.eof())
	{
		in >> fromNode >> degree >> toNode;
		r_new[toNode - 1] += r_old[fromNode - 1] / degree * beta;
	}
}
bool finalProcess()
{
	double totalNew = 0.0, totalOld = 0.0;
	bool rtn = false;
	for (int i = 0; i<maxNode; i++)
	{
		totalOld += r_new[i];
	}
	//cout << "previous total rnew :" << totalOld << endl;
	for (int i = 0; i<maxNode; i++)
	{
		if (!valid[i])
		{
			r_new[i] = 0;
			continue;
		}
		else
		{
			r_new[i] += (1 - totalOld) / validNum;
			if (r_old[i] - r_new[i]>sigma || r_new[i] - r_old[i]>sigma)
			{
				rtn = true;
			}
			r_old[i] = r_new[i];
			r_new[i] = 0;
			totalNew += r_old[i];
		}
	}
	//cout << "next total rnew :" << totalNew << endl;
	return rtn;
}
bool Iterator()
{
	for (int i = 0; i<blockNum; i++)
	{
		readBlock(i);
	}
	return finalProcess();
}
//top100 ranking sort data structure and comp func
struct rankList
{
	int nodeId;
	double nodeRank;
};
bool rankComp(rankList r1, rankList r2)
{
	return r1.nodeRank > r2.nodeRank;
}
//ranking data saving array
rankList* p;
void topRankCount()
{
	p = new rankList[maxNode];
	for (int i = 0; i < maxNode; i++)
	{
		p[i].nodeId = i ;
		p[i].nodeRank = r_old[i];
	}
	sort(p, p + maxNode, rankComp);
	cout << "count top 100 complete" << endl;
	cout << endl;
	//system("pause");
}
void writeBack()
{
	cout << "result preview:" << endl;
	ofstream out("result.txt");
	for (int i = 0; i < 100; i++)
	{
		if (i < 10)
		{
			cout << p[i].nodeId + 1 << " " << p[i].nodeRank << endl;
		}
		out << p[i].nodeId + 1 << " " << p[i].nodeRank << endl;
	}
	//system("pause");
}

int main()
{
	preLoad();
	numOutCount();
	blockSaving();
	bool rtn = true;
	int nowIter = 0;
	while (rtn)
	{
		nowIter++;
		//cout << "Iterator " << nowIter << " start" << endl;
		rtn = Iterator();
		//cout << "Iterator " << nowIter << " complete" << endl;
		//system("pause");
	}
	cout << "after " << nowIter << " iterator complete" << endl;
	cout << endl;
	topRankCount();
	writeBack();
	system("pause");
	return 0;
}