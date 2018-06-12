
/*
稀疏矩阵优化：
p->q
原方法统计的最大值分配空间法很可能会造成大量浪费
即出现许多入度为0，同时出度也为0的节点（即无关节点）
将这些节点删去不仅可以减少占用的空间
还可以增加rnew的精准度

由于复杂度问题，本例中将采用有效位形式标记无关节点
无法起到减少空间的作用
但仍可以增加rnew的精准度
*/


#include<iostream>
#include<fstream>
#include<list>
#include<algorithm>
using namespace std;

struct matEle
{
	int nodeId;
	double rank;
};

list<matEle>* listMatrix;
int *numOut;
bool *valid;
int validNum;

double beta = 0.85;
double sigma = 0.00000001;

double **Matrix;
double *rold;
double *rnew;

int maxNode = 0;
int nowIter = 0;
int blockSize = 100;
//一遍读取原数据，统计总节点数
void preLoad()
{
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	int maxReport = 0;
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
		maxReport++;
	}
	cout << "total nodes = " << maxNode << endl;
	cout << "total reports = " << maxReport << endl;
}
//根据统计的节点数分配空间
void makeSpace()
{
	if (maxNode > 0)
	{
		rold = new double[maxNode];
		rnew = new double[maxNode];
		Matrix = new double*[maxNode];
		numOut = new int[maxNode];
		valid = new bool[maxNode];
		listMatrix = new list<matEle>[maxNode];
		for (int i = 0; i < maxNode; i++)
		{
			numOut[i] = 0;
			rold[i] = 1.0 / maxNode;
			rnew[i] = 1;
			valid[i] = false;
		}
	}
	cout << "making space complete." << endl;
}
//计算节点出度并形成邻接链表
void calcNumOut()
{
	int  now = 0;
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		//由于使用节点作为下标，下标范围要从1 - maxNode
		fromNode--;
		toNode--;
		valid[fromNode] = true;
		valid[toNode] = true;
		numOut[fromNode]++;
		matEle tmp;
		tmp.nodeId = fromNode;
		listMatrix[toNode].push_back(tmp);
	}
	for (int i = 0; i < maxNode; i++)
	{
		if (valid[i])
			validNum++;
		list<matEle>::iterator p = listMatrix[i].begin();
		while (p != listMatrix[i].end())
		{
			p->rank = 1.0 / numOut[p->nodeId];
			p++;
		}
	}
	cout << "calcing out degrees complete." << endl;
	cout << "total valid nodes : " << validNum << endl;
	cout << "out degrees preview:" << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << i + 1 << " " << numOut[i] << endl;
	}
}
//迭代计算pageRank
bool Iterator()
{
	nowIter++;
	double totalNew = 0.0;
	bool rtn = false;//when false this func ends.
	for (int i = 0; i < maxNode; i++)
	{
		list<matEle>::iterator p = listMatrix[i].begin();
		rnew[i] = 0;
		if (listMatrix[i].empty())
		{
			//cout << i << " deadends discovered." << endl;
			continue;
		}
		while (p != listMatrix[i].end())
		{
			rnew[i] += rold[p->nodeId] * p->rank*beta;
			p++;
		}
		totalNew += rnew[i];
	}
	double totalOld = 0.0;
	for (int i = 0; i < maxNode; i++)
	{
		if (!valid[i])
		{
			rnew[i] = 0;
			continue;
		}
		rnew[i] += (1 - totalNew) / validNum;
		if (rold[i] - rnew[i] > sigma || rnew[i] - rold[i] > sigma)
			rtn = true;
		rold[i] = rnew[i];
		totalOld += rold[i];
	}
	cout << nowIter << " iterator result preview:" << endl;
	cout << "rold total: " << totalOld << endl;
	cout << "rnew total: " << totalNew << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << i + 1 << " " << rnew[i] << endl;
	}
	cout << endl;
	//cout << nowIter << "iterator complete" << endl;
	system("pause");
	return rtn;
}

struct rankList
{
	int nodeId;
	double nodeRank;
};
bool rankComp(rankList r1, rankList r2)
{
	return r1.nodeRank > r2.nodeRank;
}

rankList *p;

//计算前100的节点
void rank100()
{
	p = new rankList[maxNode];
	for (int i = 0; i < maxNode; i++)
	{
		p[i].nodeId = i;
		p[i].nodeRank = rold[i];
	}
	sort(p, p + maxNode, rankComp);
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
}

int main()
{
	preLoad();
	makeSpace();
	calcNumOut();
	bool iteRtn = true;
	while (iteRtn)
		//for(int i=0;i<100;i++)
	{
		iteRtn = Iterator();
	}
	cout << nowIter << "iterator complete" << endl;
	rank100();
	writeBack();
	system("pause");
	return 0;
}