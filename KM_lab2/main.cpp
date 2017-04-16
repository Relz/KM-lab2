#include "stdafx.h"

using namespace std;

static const size_t MAX_DEPTH = 50;

//const map<size_t, pair<size_t, size_t>> POSITIONS = {
//	{ 0, { 0, 0 } },
//	{ 1, { 1, 0 } },
//	{ 2, { 2, 0 } },
//	{ 3, { 0, 1 } },
//	{ 4, { 1, 1 } },
//	{ 5, { 2, 1 } },
//	{ 6, { 0, 2 } },
//	{ 7, { 1, 2 } },
//	{ 8, { 2, 2 } }
//};
//
//static const vector<vector<size_t>> START_MATRIX =
//{
//	{8, 7, 6},
//	{5, 4, 3},
//	{2, 1, 0}
//};
//
//static const vector<vector<size_t>> WIN_MATRIX =
//{
//	{0, 1, 2},
//	{3, 4, 5},
//	{6, 7, 8}
//};

// Тяжелый тест, проходится только с помощью метода A* (bad_alloc)
static const vector<vector<size_t>> START_MATRIX =
{
	{ 15, 14, 13, 12 },
	{ 11, 10, 9, 8 },
	{ 7, 6, 5, 4 },
	{ 3, 2, 1, 0 }
};

const map<size_t, pair<size_t, size_t>> POSITIONS = {
	{ 0, { 0, 0 } },
	{ 1, { 1, 0 } },
	{ 2, { 2, 0 } },
	{ 3, { 3, 0 } },
	{ 4, { 0, 1 } },
	{ 5, { 1, 1 } },
	{ 6, { 2, 1 } },
	{ 7, { 3, 1 } },
	{ 8, { 0, 2 } },
	{ 9, { 1, 2 } },
	{ 10, { 2, 2 } },
	{ 11, { 3, 2 } },
	{ 12, { 0, 3 } },
	{ 13, { 1, 3 } },
	{ 14, { 2, 3 } },
	{ 15, { 3, 3 } }
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
	LENGTH,
	ASTAR
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
	size_t currentDepth = 0;
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

size_t GetMatrixHash(const vector<vector<size_t> > & vect, size_t seed)
{
	for (size_t i = 0; i < vect.size(); ++i)
	{
		seed ^= boost::hash_range(vect[i].begin(), vect[i].end()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

size_t GetManhattanDistance(const vector<vector<size_t>> & matrix)
{
	size_t result = 0;
	for (size_t row = 0; row < matrix.size(); ++row)
	{
		for (size_t col = 0; col < matrix.size(); ++col)
		{
			if (POSITIONS.at(matrix[row][col]).first > col)
			{
				result += POSITIONS.at(matrix[row][col]).first - col;
			}
			else
			{
				result += col - POSITIONS.at(matrix[row][col]).first;
			}
			if (POSITIONS.at(matrix[row][col]).second > row)
			{
				result += POSITIONS.at(matrix[row][col]).second - row;
			}
			else
			{
				result += row - POSITIONS.at(matrix[row][col]).second;
			}
		}
	}
	return result;
}

static const size_t WIN_MATRIX_HASH = GetMatrixHash(WIN_MATRIX, 0);

Node *CreateNode(Node *currentNode, int directionX, int directionY)
{
	// Возможность появления исключения bad_alloc
	Node *newNode = new Node;
	newNode->zeroPos = Position(currentNode->zeroPos.x + directionX, currentNode->zeroPos.y + directionY);
	newNode->father = currentNode;
	newNode->matrix = currentNode->matrix;
	swap(newNode->matrix[currentNode->zeroPos.y + directionY][currentNode->zeroPos.x + directionX], newNode->matrix[currentNode->zeroPos.y][currentNode->zeroPos.x]);
	newNode->hash = GetMatrixHash(newNode->matrix, 0);
	newNode->currentDepth = currentNode->currentDepth + 1;
	return newNode;
}

bool IsHashProcessed(size_t hash, set<size_t> & processedHashes)
{
	return (processedHashes.find(hash) != processedHashes.end());
}

bool ProcessQueue(vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, set<size_t> & processedHashes, Algorithm algorithm)
{
	Node *firstNode = nullptr;
	if (algorithm == Algorithm::ASTAR)
	{
		firstNode = nodesPriorityQueue.begin()->second[nodesPriorityQueue.begin()->second.size() - 1];
		if (nodesPriorityQueue.begin()->second.size() == 1)
		{
			nodesPriorityQueue.erase(nodesPriorityQueue.begin());
		}
		else
		{
			nodesPriorityQueue.begin()->second.pop_back();
		}
	}
	else
	{
		firstNode = nodesQueue.front();
		nodesQueue.erase(nodesQueue.begin());
	}
	if (algorithm == Algorithm::LENGTH && firstNode->currentDepth == MAX_DEPTH)
	{
		return true;
	}
	if (!IsHashProcessed(firstNode->hash, processedHashes))
	{
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
			else if (algorithm == Algorithm::ASTAR)
			{
				Node *newNode = CreateNode(firstNode, 1, 0);
				nodesPriorityQueue[GetManhattanDistance(newNode->matrix)].push_back(newNode);
				if (newNode->hash == WIN_MATRIX_HASH)
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
			else if (algorithm == Algorithm::ASTAR)
			{
				Node *newNode = CreateNode(firstNode, 0, 1);
				nodesPriorityQueue[GetManhattanDistance(newNode->matrix)].push_back(newNode);
				if (newNode->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
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
			else if (algorithm == Algorithm::ASTAR)
			{
				Node *newNode = CreateNode(firstNode, 0, -1);
				nodesPriorityQueue[GetManhattanDistance(newNode->matrix)].push_back(newNode);
				if (newNode->hash == WIN_MATRIX_HASH)
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
			else if (algorithm == Algorithm::ASTAR)
			{
				Node *newNode = CreateNode(firstNode, -1, 0);
				nodesPriorityQueue[GetManhattanDistance(newNode->matrix)].push_back(newNode);
				if (newNode->hash == WIN_MATRIX_HASH)
				{
					return false;
				}
			}
		}
		processedHashes.insert(firstNode->hash);
	}
	if (nodesQueue.empty() && (algorithm == Algorithm::WIDTH || algorithm == Algorithm::LENGTH))
	{
		return false;
	}
	else if (nodesPriorityQueue.empty() && algorithm == Algorithm::ASTAR)
	{
		return false;
	}
	return true;
}

// Надеемся, что стека хватит, иначе организовать через цикл
void PrintWayToWin(Node *winNode)
{
	if (winNode->father != nullptr)
	{
		PrintWayToWin(winNode->father);
	}
	for (size_t y = 0; y < winNode->matrix.size(); ++y)
	{
		for (size_t x = 0; x < winNode->matrix[y].size(); ++x)
		{
			cout << winNode->matrix[y][x] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

int main()
{
	Node *firstNode = new Node;
	firstNode->matrix = START_MATRIX;
	CalculateZeroPos(firstNode);
	firstNode->hash = GetMatrixHash(firstNode->matrix, 0);
	set<size_t> processedHashes;
	vector<Node*> nodesQueue;
	map<size_t, vector<Node*>> nodesPriorityQueue;

	Algorithm algorithm = Algorithm::ASTAR;
	if (algorithm == Algorithm::WIDTH || algorithm == Algorithm::LENGTH)
	{
		nodesQueue.push_back(firstNode);
	}
	else if (algorithm == Algorithm::ASTAR)
	{
		nodesPriorityQueue[GetManhattanDistance(firstNode->matrix)].push_back(firstNode);
	}

	boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
	while (ProcessQueue(nodesQueue, nodesPriorityQueue, processedHashes, algorithm)) {}
	boost::chrono::duration<double> duration = boost::chrono::system_clock::now() - start;
	cout << fixed << duration << endl;

	if (algorithm == Algorithm::WIDTH)
	{
		PrintWayToWin(nodesQueue[nodesQueue.size() - 1]);
	}
	else if (algorithm == Algorithm::LENGTH)
	{
		PrintWayToWin(nodesQueue[0]);
	}
	else if (algorithm == Algorithm::ASTAR)
	{
		PrintWayToWin(nodesPriorityQueue.begin()->second[0]);
	}

	return 0;
}