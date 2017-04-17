#include "stdafx.h"
#include "Constant.h"
#include "Node.h"

using namespace std;

using CONSTANT = Constant::FIFTEEN_GAME;
using Node = CNode<CONSTANT>;

enum Algorithm
{
	WIDTH,
	LENGTH,
	ASTAR
};

static const map<Algorithm, size_t> MAX_DEPTH = {
	{ Algorithm::LENGTH, 50 }
};

Algorithm ALGORITHM = Algorithm::ASTAR;

Node *CreateNode(Node *currentNode, int directionX, int directionY)
{
	Point & currentNodeZeroPos = currentNode->GetZeroPos();
	Point newNodeZeroPos = Point(currentNodeZeroPos.x + directionX, currentNodeZeroPos.y + directionY);
	// Возможность появления исключения bad_alloc
	Node *newNode = new Node;
	newNode->SetZeroPos(newNodeZeroPos);
	newNode->SetFather(currentNode);
	Matrix tmpMatrix = currentNode->GetMatrix();
	swap(tmpMatrix[newNodeZeroPos.y][newNodeZeroPos.x], tmpMatrix[currentNodeZeroPos.y][currentNodeZeroPos.x]);
	newNode->SetMatrix(move(tmpMatrix), false);
	return newNode;
}

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

bool CheckDepthLimit(Node *currentNode, Algorithm algorithm)
{
	bool result = false;
	if (MAX_DEPTH.find(algorithm) != MAX_DEPTH.end())
	{
		if (currentNode->GetDepth() == MAX_DEPTH.at(algorithm))
		{
			result = true;
		}
	}
	return result;
}

bool InsertNewNodeToQueueAndCheckIsItWin(Node *curentNode, const Point & direction, vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, Algorithm algorithm)
{
	bool result = true;
	if (algorithm == Algorithm::WIDTH)
	{
		nodesQueue.push_back(CreateNode(curentNode, direction.x, direction.y));
		if (nodesQueue.back()->GetHash() == CONSTANT::WIN_MATRIX_HASH)
		{
			result = false;
		}
	}
	else if (algorithm == Algorithm::LENGTH)
	{
		nodesQueue.insert(nodesQueue.begin(), CreateNode(curentNode, direction.x, direction.y));
		if (nodesQueue.front()->GetHash() == CONSTANT::WIN_MATRIX_HASH)
		{
			result = false;
		}
	}
	else if (algorithm == Algorithm::ASTAR)
	{
		Node *newNode = CreateNode(curentNode, direction.x, direction.y);
		nodesPriorityQueue[newNode->CalculateManhattanDistance()].push_back(newNode);
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
	if (CheckDepthLimit(firstNode, algorithm))
	{
		return true;
	}
	if (!IsHashProcessed(firstNode->GetHash(), processedHashes))
	{
		if (firstNode->GetZeroPos().x < firstNode->GetMatrixSize() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(1, 0), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNode->GetZeroPos().y < firstNode->GetMatrixSize() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, 1), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNode->GetZeroPos().y > 0)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, -1), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNode->GetZeroPos().x > 0)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(-1, 0), nodesQueue, nodesPriorityQueue, algorithm))
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

int main()
{
	Node *firstNode = new Node(CONSTANT::START_MATRIX, true);

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
		nodesPriorityQueue[firstNode->CalculateManhattanDistance()].push_back(firstNode);
	}

	boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
	while (ProcessQueue(nodesQueue, nodesPriorityQueue, processedHashes, totalNodeCount, ALGORITHM)) {}
	boost::chrono::duration<double> duration = boost::chrono::system_clock::now() - start;
	cout << fixed << duration << endl;

	if (ALGORITHM == Algorithm::WIDTH)
	{
		nodesQueue[nodesQueue.size() - 1]->PrintWay();
	}
	else if (ALGORITHM == Algorithm::LENGTH)
	{
		nodesQueue[0]->PrintWay();
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		nodesPriorityQueue.begin()->second[0]->PrintWay();
	}
	
	return 0;
}