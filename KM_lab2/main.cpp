#include "stdafx.h"
#include "Algorithm.h"
#include "Constant.h"
#include "Node.h"

using namespace std;

using CONSTANT = Constant::FIFTEEN_GAME;
using Node = CNode<CONSTANT>;

static const map<Algorithm, size_t> MAX_DEPTH = {
	{ Algorithm::LENGTH, 50 }
};

Algorithm ALGORITHM = Algorithm::WIDTH;

bool IsHashProcessed(size_t hash, set<size_t> & processedHashes)
{
	return (processedHashes.find(hash) != processedHashes.end());
}

Node *GetFirstNode(vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, Algorithm algorithm)
{
	Node *result = nullptr;
	if (algorithm == Algorithm::ASTAR)
	{
		result = nodesPriorityQueue.begin()->second[nodesPriorityQueue.begin()->second.size() - 1];
		if (nodesPriorityQueue.begin()->second.size() == 1)
		{
			nodesPriorityQueue.erase(nodesPriorityQueue.begin());
		}
		else
		{
			nodesPriorityQueue.begin()->second.pop_back();
		}
	}
	else if (algorithm == Algorithm::WIDTH || algorithm == Algorithm::LENGTH)
	{
		result = nodesQueue.front();
		nodesQueue.erase(nodesQueue.begin());
	}
	return result;
}

bool InsertNewNodeToQueueAndCheckIsItWin(Node *curentNode, const Point & direction, vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, size_t & totalNodeCount, Algorithm algorithm)
{
	bool result = true;
	++totalNodeCount;
	if (algorithm == Algorithm::WIDTH)
	{
		nodesQueue.push_back(Node::CreateNode(curentNode, direction.x, direction.y));
		if (nodesQueue.back()->GetHash() == CONSTANT::WIN_MATRIX_HASH)
		{
			result = false;
		}
	}
	else if (algorithm == Algorithm::LENGTH)
	{
		nodesQueue.insert(nodesQueue.begin(), Node::CreateNode(curentNode, direction.x, direction.y));
		if (nodesQueue.front()->GetHash() == CONSTANT::WIN_MATRIX_HASH)
		{
			result = false;
		}
	}
	else if (algorithm == Algorithm::ASTAR)
	{
		Node *newNode = Node::CreateNode(curentNode, direction.x, direction.y);
		nodesPriorityQueue[Node::CalculateManhattanDistance(newNode->matrix)].push_back(newNode);
		if (newNode->GetHash() == CONSTANT::WIN_MATRIX_HASH)
		{
			result = false;
		}
	}
	return result;
}

bool IsEmptyQueue(const vector<Node*> & nodesQueue, const map<size_t, vector<Node*>> & nodesPriorityQueue, Algorithm algorithm)
{
	bool result = false;
	if (nodesQueue.empty() && (algorithm == Algorithm::WIDTH || algorithm == Algorithm::LENGTH))
	{
		result = true;
	}
	else if (nodesPriorityQueue.empty() && algorithm == Algorithm::ASTAR)
	{
		result = true;
	}
	return result;
}

bool ProcessQueue(vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, set<size_t> & processedHashes, size_t & totalNodeCount, Algorithm algorithm)
{
	Node *firstNode = GetFirstNode(nodesQueue, nodesPriorityQueue, algorithm);
	if (Node::CheckDepthLimit(firstNode, algorithm, MAX_DEPTH))
	{
		return true;
	}
	if (!IsHashProcessed(firstNode->GetHash(), processedHashes))
	{
		Point & firstNodeZeroPos = firstNode->GetZeroPos();
		if (firstNodeZeroPos.x < firstNode->matrix.size() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(1, 0), nodesQueue, nodesPriorityQueue, totalNodeCount, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.y < firstNode->matrix.size() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, 1), nodesQueue, nodesPriorityQueue, totalNodeCount, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.y > 0)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, -1), nodesQueue, nodesPriorityQueue, totalNodeCount, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.x > 0)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(-1, 0), nodesQueue, nodesPriorityQueue, totalNodeCount, algorithm))
			{
				return false;
			}
		}
		processedHashes.insert(firstNode->GetHash());
	}
	if (IsEmptyQueue(nodesQueue, nodesPriorityQueue, algorithm))
	{
		return false;
	}
	return true;
}

void SkipWhitespaces(ifstream & inputFile)
{
	while (inputFile.peek() == ' ')
	{
		inputFile.get();
	}
}

bool ReadStartMatrix(const string & inputFileName, Matrix & matrix)
{
	ifstream inputFile(inputFileName);
	bool result = false;
	SkipWhitespaces(inputFile);
	size_t elem;
	size_t counter = 0;
	matrix = Matrix(4, vector<size_t>());
	while (inputFile.peek() != '\n' && !inputFile.eof() && inputFile >> elem)
	{
		matrix[counter].push_back(elem);
		result = true;
		SkipWhitespaces(inputFile);
		if (matrix[counter].size() % 4 == 0)
		{
			++counter;
		}
	}
	return result;
}

void PrintWay(const vector<Node*> & nodesQueue, const map<size_t, vector<Node*>> & nodesPriorityQueue, size_t & wayLength)
{
	wayLength = 0;
	if (ALGORITHM == Algorithm::WIDTH)
	{
		Node::PrintWay(nodesQueue[nodesQueue.size() - 1], nodesQueue[nodesQueue.size() - 1]->matrix, wayLength);
	}
	else if (ALGORITHM == Algorithm::LENGTH)
	{
		Node::PrintWay(nodesQueue[0], nodesQueue[0]->matrix, wayLength);
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		Node::PrintWay(nodesPriorityQueue.begin()->second[0], nodesPriorityQueue.begin()->second[0]->matrix, wayLength);
	}
	--wayLength;
}

void PrintSteps(const vector<Node*> & nodesQueue, const map<size_t, vector<Node*>> & nodesPriorityQueue, size_t & wayLength)
{
	wayLength = 0;
	cout << "[";
	if (ALGORITHM == Algorithm::WIDTH)
	{
		Node::PrintSteps(nodesQueue[nodesQueue.size() - 1], nodesQueue[nodesQueue.size() - 1]->matrix, Point(SIZE_MAX, SIZE_MAX), wayLength);
	}
	else if (ALGORITHM == Algorithm::LENGTH)
	{
		Node::PrintSteps(nodesQueue[0], nodesQueue[0]->matrix, Point(SIZE_MAX, SIZE_MAX), wayLength);
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		Node::PrintSteps(nodesPriorityQueue.begin()->second[0], nodesPriorityQueue.begin()->second[0]->matrix, Point(SIZE_MAX, SIZE_MAX), wayLength);
	}
	cout << "]\n";
}

void PrintMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	cout << pmc.WorkingSetSize << "\n";
}

int main()
{
	Matrix startMatrix;
	ReadStartMatrix("input.txt", startMatrix);
	Node *firstNode = new Node(startMatrix);
	firstNode->SetHash(Node::CalculateMatrixHash(firstNode->matrix));
	Point zeroPos(0, 0);
	Node::CalculateZeroPos(firstNode->matrix, zeroPos);
	firstNode->SetZeroPos(move(zeroPos));

	set<size_t> processedHashes;
	vector<Node*> nodesQueue;
	map<size_t, vector<Node*>> nodesPriorityQueue;

	size_t totalNodeCount = 0;

	if (ALGORITHM == Algorithm::WIDTH || ALGORITHM == Algorithm::LENGTH)
	{
		nodesQueue.push_back(firstNode);
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		nodesPriorityQueue[Node::CalculateManhattanDistance(firstNode->matrix)].push_back(firstNode);
	}

	boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();
	while (ProcessQueue(nodesQueue, nodesPriorityQueue, processedHashes, totalNodeCount, ALGORITHM)) {}
	boost::chrono::high_resolution_clock::time_point end = boost::chrono::high_resolution_clock::now();
	boost::chrono::milliseconds duration = boost::chrono::duration_cast<boost::chrono::milliseconds>(end - start);
	cout << fixed << duration.count() << "\n";
	//cout << totalNodeCount << "\n";
	//cout << processedHashes.size() << "\n";

	size_t wayLength = 0;
	//PrintWay(nodesQueue, nodesPriorityQueue, wayLength);
	//PrintSteps(nodesQueue, nodesPriorityQueue, wayLength);
	//cout << wayLength << "\n";
	
	//PrintMemoryUsage();
	
	return 0;
}