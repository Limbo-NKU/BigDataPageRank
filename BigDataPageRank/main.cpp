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

double beta = 0.8;
double sigma = 0.0001;

double **Matrix;
double *rold;
double *rnew;

int maxNode = 0;
int nowIter = 0;
//һ���ȡԭ���ݣ�ͳ���ܽڵ���
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
//����ͳ�ƵĽڵ�������ռ�
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
			rold[i] = 1 / maxNode;
			rnew[i] = 1;
		}
	}
	cout << "making space complete." << endl;
}
//����ڵ���Ȳ��γ��ڽ�����
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
	cout << "calcing out degrees complete." << endl;
}
//��ʼ��M��rold��rnew
void preSet()
{
	
}
//��������pageRank
bool Iterator()
{
	nowIter++;
	int totalNew = 0;
	bool rtn = false;//when false this func ends.
	for (int i = 0; i < maxNode; i++)
	{
		list<matEle>::iterator p = listMatrix[i].begin();
		rnew[i] = 0;
		if (listMatrix[i].empty())
		{
			continue;
			cout << nowIter << "deadends discovered." << endl;
		}
		while (p != listMatrix[i].end())
		{
			rnew[i] += rold[i] * p->rank*beta;
		}
		totalNew += rnew[i];
	}
	for (int i = 0; i < maxNode; i++)
	{
		rnew[i] += (1 - totalNew) / maxNode;
		if (rold[i] - rnew[i] > sigma || rnew[i] - rold[i] > sigma)
			rtn = true;
		rold[i] = rnew[i];
	}
	cout << nowIter << "iterator complete" << endl;
	return rtn;
}

void writeBack()
{
	cout << "result preview:" << endl;
	ofstream out("result.txt");
	for (int i = 0; i < maxNode; i++)
	{
		if (i < 10)
		{
			cout << i << " " << rold[i] << endl;
		}
		out << i << " " << rold[i] << endl;
	}
}

int main()
{
	preLoad();
	makeSpace();
	calcNumOut();
	bool iteRtn = true;
	while (iteRtn)
	{
		iteRtn = Iterator();
	}
	writeBack();
	system("pause");
	return 0;
}