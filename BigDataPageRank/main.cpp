#include<iostream>
#include<fstream>
#include<list>
using namespace std;

struct matEle
{
	int nodeId;
	double rank;
};

list<matEle>* listMatrix;
int *numOut;

double **Matrix;
double *rold;
double *rnew;

int maxNode = 0;

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

void makeSpace()
{
	if (maxNode > 0)
	{
		rold = new double[maxNode];
		rnew = new double[maxNode];
		Matrix = new double*[maxNode];
		numOut = new int[maxNode];
		listMatrix = new list<matEle>[maxNode];
		for (int i = 0; i < maxNode; i++)
		{
			numOut[i] = 0;
		}
	}
}

void calcNumOut()
{
	int  now = 0;
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		numOut[fromNode]++;
		matEle tmp;
		tmp.nodeId = fromNode;
		listMatrix[toNode].push_back(tmp);
	}
	for (int i = 0; i < maxNode; i++)
	{
		list<matEle>::iterator p = listMatrix[i].begin();
		while (p != listMatrix[i].end())
		{
			p->rank = numOut[p->nodeId];
		}
	}
}

void preSet()
{

}

void Iterator()
{

}