#include "stdafx.h"

using namespace std;

/*static const vector<vector<size_t>> START_MATRIX =
		{
				{8, 7, 6},
				{5, 4, 3},
				{2, 1, 0}
		};

static const vector<vector<size_t>> WIN_MATRIX =
		{
				{0, 1, 2},
				{3, 4, 5},
				{6, 7, 8}
		};*/

static const vector<vector<size_t>> START_MATRIX =
		{
				{15, 14, 13, 12},
				{11, 10, 9, 8},
				{7, 6, 5, 4},
				{3, 2, 1, 0}
		};

static const vector<vector<size_t>> WIN_MATRIX =
		{
				{0, 1, 2, 3},
				{4, 5, 6, 7},
				{8, 9, 10, 11},
				{12, 13, 14, 15}
		};

enum Algorithm
{
	WIDTH,
	LENGTH
};

struct Position
{
	size_t x = 0;
	size_t y = 0;

	Position(size_t x, size_t y)
		: x(x)
		, y(y)
	{
	}
};

struct Node
{
	size_t hash = 0;
	vector<vector<size_t>> matrix;
	Position zeroPos = {0, 0};
	Node *father = nullptr;
};

void CalculateZeroPos(Node *&node)
{
	for (size_t i = 0; i < node->matrix.size(); ++i)
	{
		for (size_t j = 0; j < node->matrix[i].size(); ++j)
		{
			if (node->matrix[i][j] == 0)
			{
				node->zeroPos = Position(j, i);
				break;
			}
		}
	}
}

size_t GetVectorHash(const vector<vector<size_t> > & vect, size_t seed)
{
	for (size_t i = 0; i < vect.size(); ++i)
	{
		seed ^= boost::hash_range(vect[i].begin(), vect[i].end()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

static const size_t WIN_MATRIX_HASH = GetVectorHash(WIN_MATRIX, 0);

Node *CreateNode(Node *currentNode, int directionX, int directionY)
{
	Node *newNode = new Node;
	newNode->zeroPos = Position(currentNode->zeroPos.x + directionX, currentNode->zeroPos.y + directionY);
//	newNode->father = currentNode;
	newNode->matrix = currentNode->matrix;
	swap(newNode->matrix[currentNode->zeroPos.y + directionY][currentNode->zeroPos.x + directionX], newNode->matrix[currentNode->zeroPos.y][currentNode->zeroPos.x]);
	newNode->hash = GetVectorHash(newNode->matrix, 0);
	return newNode;
}

bool IsHashProcessed(size_t hash, set<size_t> & processedHashes)
{
	return (processedHashes.find(hash) != processedHashes.end());
}

Node *savedNode = nullptr;

bool ProcessQueue(vector<Node*> & nodesQueue, set<size_t> & processedHashes, Algorithm algorithm)
{
	Node *firstNode = nodesQueue.front();
	nodesQueue.erase(nodesQueue.begin());
	if (!IsHashProcessed(firstNode->hash, processedHashes))
	{
		if (firstNode->zeroPos.y > 0)
		{
			if (algorithm == Algorithm::WIDTH)
			{
				nodesQueue.push_back(CreateNode(firstNode, 0, -1));
				if (nodesQueue.back()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
			else if (algorithm == Algorithm::LENGTH)
			{
				nodesQueue.insert(nodesQueue.begin(), CreateNode(firstNode, 0, -1));
				if (nodesQueue.front()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
		if (firstNode->zeroPos.x < firstNode->matrix.size() - 1)
		{
			if (algorithm == Algorithm::WIDTH)
			{
				nodesQueue.push_back(CreateNode(firstNode, 1, 0));
				if (nodesQueue.back()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
			else if (algorithm == Algorithm::LENGTH)
			{
				nodesQueue.insert(nodesQueue.begin(), CreateNode(firstNode, 1, 0));
				if (nodesQueue.front()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
		if (firstNode->zeroPos.y < firstNode->matrix.size() - 1)
		{
			if (algorithm == Algorithm::WIDTH)
			{
				nodesQueue.push_back(CreateNode(firstNode, 0, 1));
				if (nodesQueue.back()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
			else if (algorithm == Algorithm::LENGTH)
			{
				nodesQueue.insert(nodesQueue.begin(), CreateNode(firstNode, 0, 1));
				if (nodesQueue.front()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
		if (firstNode->zeroPos.x > 0)
		{
			if (algorithm == Algorithm::WIDTH)
			{
				nodesQueue.push_back(CreateNode(firstNode, -1, 0));
				if (nodesQueue.back()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
			else if (algorithm == Algorithm::LENGTH)
			{
				nodesQueue.insert(nodesQueue.begin(), CreateNode(firstNode, -1, 0));
				if (nodesQueue.front()->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
		processedHashes.insert(firstNode->hash);
		if (savedNode == nullptr)
		{
			if (nodesQueue.size() > 0)
			{
				savedNode = nodesQueue[nodesQueue.size() - 1];
			}
		}
	}
	return true;
}

int main()
{
	Node *firstNode = new Node;
	firstNode->matrix = START_MATRIX;
	CalculateZeroPos(firstNode);
	firstNode->hash = GetVectorHash(firstNode->matrix, 0);

	set<size_t> processedHashes;
	vector<Node*> nodesQueue;

	nodesQueue.push_back(firstNode);

	while (ProcessQueue(nodesQueue, processedHashes, Algorithm::WIDTH)) {}

	return 0;
}