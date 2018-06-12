
/*
ϡ������Ż���
p->q
ԭ����ͳ�Ƶ����ֵ����ռ䷨�ܿ��ܻ���ɴ����˷�
������������Ϊ0��ͬʱ����ҲΪ0�Ľڵ㣨���޹ؽڵ㣩
����Щ�ڵ�ɾȥ�������Լ���ռ�õĿռ�
����������rnew�ľ�׼��

���ڸ��Ӷ����⣬�����н�������Чλ��ʽ����޹ؽڵ�
�޷��𵽼��ٿռ������
���Կ�������rnew�ľ�׼��
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
//����ڵ���Ȳ��γ��ڽ�����
void calcNumOut()
{
	int  now = 0;
	ifstream in("WikiData.txt");
	int fromNode, toNode;
	while (!in.eof())
	{
		in >> fromNode >> toNode;
		//����ʹ�ýڵ���Ϊ�±꣬�±귶ΧҪ��1 - maxNode
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
//��������pageRank
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

//����ǰ100�Ľڵ�
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