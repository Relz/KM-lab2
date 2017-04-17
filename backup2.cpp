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

size_t CalculateManhattanDistance(const Matrix & matrix)
{
	size_t result = 0;
	for (size_t row = 0; row < matrix.size(); ++row)
	{
		for (size_t col = 0; col < matrix.size(); ++col)
		{
			if (CONSTANT::WIN_POSITIONS.at(matrix[row][col]).first > col)
			{
				result += CONSTANT::WIN_POSITIONS.at(matrix[row][col]).first - col;
			}
			else
			{
				result += col - CONSTANT::WIN_POSITIONS.at(matrix[row][col]).first;
			}
			if (CONSTANT::WIN_POSITIONS.at(matrix[row][col]).second > row)
			{
				result += CONSTANT::WIN_POSITIONS.at(matrix[row][col]).second - row;
			}
			else
			{
				result += row - CONSTANT::WIN_POSITIONS.at(matrix[row][col]).second;
			}
		}
	}
	return result;
}

size_t CalculateMatrixHash(const Matrix & matrix)
{
	size_t seed = 0;
	for (size_t i = 0; i < matrix.size(); ++i)
	{
		seed ^= boost::hash_range(matrix[i].begin(), matrix[i].end()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

void CalculateZeroPos(const Matrix & matrix, Point & zeroPos)
{
	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix.size(); ++j)
		{
			if (matrix[i][j] == 0)
			{
				zeroPos = Point(j, i);
				return;
			}
		}
	}
}

Node *CreateNode(Node *currentNode, int directionX, int directionY)
{
	Point & currentNodeZeroPos = currentNode->GetZeroPos();
	// Возможность появления исключения bad_alloc
	Node *newNode = new Node;
	newNode->SetZeroPos(Point(currentNodeZeroPos.x + directionX, currentNodeZeroPos.y + directionY));
	newNode->SetFather(currentNode);
	newNode->matrix = currentNode->matrix;
	swap(newNode->matrix[newNode->GetZeroPos().y][newNode->GetZeroPos().x], newNode->matrix[currentNodeZeroPos.y][currentNodeZeroPos.x]);
	newNode->SetHash(CalculateMatrixHash(newNode->matrix));
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
		nodesPriorityQueue[CalculateManhattanDistance(newNode->matrix)].push_back(newNode);
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
		Point & firstNodeZeroPos = firstNode->GetZeroPos();
		if (firstNodeZeroPos.x < firstNode->matrix.size() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(1, 0), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.y < firstNode->matrix.size() - 1)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, 1), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.y > 0)
		{
			if (!InsertNewNodeToQueueAndCheckIsItWin(firstNode, Point(0, -1), nodesQueue, nodesPriorityQueue, algorithm))
			{
				return false;
			}
		}
		if (firstNodeZeroPos.x > 0)
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

template<class CONSTANT>
void PrintWay(CNode<CONSTANT> *currentNode, const Matrix & matrix)
{
	if (currentNode->GetFather() != nullptr)
	{
		PrintWay(currentNode->GetFather(), matrix);
	}
	for (size_t y = 0; y < matrix.size(); ++y)
	{
		for (size_t x = 0; x < matrix.size(); ++x)
		{
			cout << matrix[y][x] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

int main()
{
	Node *firstNode = new Node(CONSTANT::START_MATRIX);
	firstNode->SetHash(CalculateMatrixHash(firstNode->matrix));
	CalculateZeroPos(firstNode->matrix, firstNode->GetZeroPos());

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
		nodesPriorityQueue[CalculateManhattanDistance(firstNode->matrix)].push_back(firstNode);
	}

	boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
	while (ProcessQueue(nodesQueue, nodesPriorityQueue, processedHashes, totalNodeCount, ALGORITHM)) {}
	boost::chrono::duration<double> duration = boost::chrono::system_clock::now() - start;
	cout << fixed << duration << endl;

	if (ALGORITHM == Algorithm::WIDTH)
	{
		PrintWay(nodesQueue[nodesQueue.size() - 1], nodesQueue[nodesQueue.size() - 1]->matrix);
	}
	else if (ALGORITHM == Algorithm::LENGTH)
	{
		PrintWay(nodesQueue[0], nodesQueue[0]->matrix);
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		PrintWay(nodesPriorityQueue.begin()->second[0], nodesPriorityQueue.begin()->second[0]->matrix);
	}
	
	return 0;
}