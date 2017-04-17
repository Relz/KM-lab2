﻿#include "stdafx.h"
#include "Algorithm.h"
#include "Constant.h"
#include "Node.h"

using namespace std;

using CONSTANT = Constant::FIFTEEN_GAME;
using Node = CNode<CONSTANT>;

static const map<Algorithm, size_t> MAX_DEPTH = {
	{ Algorithm::LENGTH, 50 }
};

Algorithm ALGORITHM = Algorithm::ASTAR;

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

bool InsertNewNodeToQueueAndCheckIsItWin(Node *curentNode, const Point & direction, vector<Node*> & nodesQueue, map<size_t, vector<Node*>> & nodesPriorityQueue, Algorithm algorithm)
{
	bool result = true;
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

int main()
{
	Node *firstNode = new Node(CONSTANT::START_MATRIX);
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

	boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
	while (ProcessQueue(nodesQueue, nodesPriorityQueue, processedHashes, totalNodeCount, ALGORITHM)) {}
	boost::chrono::duration<double> duration = boost::chrono::system_clock::now() - start;
	cout << fixed << duration << endl;

	if (ALGORITHM == Algorithm::WIDTH)
	{
		Node::PrintWay(nodesQueue[nodesQueue.size() - 1], nodesQueue[nodesQueue.size() - 1]->matrix);
	}
	else if (ALGORITHM == Algorithm::LENGTH)
	{
		Node::PrintWay(nodesQueue[0], nodesQueue[0]->matrix);
	}
	else if (ALGORITHM == Algorithm::ASTAR)
	{
		Node::PrintWay(nodesPriorityQueue.begin()->second[0], nodesPriorityQueue.begin()->second[0]->matrix);
	}
	
	return 0;
}