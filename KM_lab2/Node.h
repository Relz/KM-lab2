#pragma once
#include "Point.h"
#include "Matrix.h"
#include "Algorithm.h"
#include "Constant.h"

using namespace std;

template<class CONSTANT>
class CNode
{
public:
	CNode() = default;
	CNode(const Matrix & matrix)
		: matrix(matrix)
	{
	}

	CNode(Matrix && matrix)
	{
		matrix = move(matrix);
	}

	CNode *GetFather() const
	{
		return m_father;
	}

	void SetFather(CNode *father)
	{
		m_father = father;
		m_depth = father->GetDepth() + 1;
	}

	size_t GetDepth() const
	{
		return m_depth;
	}

	size_t GetHash() const
	{
		return m_hash;
	}

	void SetHash(size_t value)
	{
		m_hash = value;
	}

	Point GetZeroPos() const
	{
		return m_zeroPos;
	}

	void SetZeroPos(const Point & value)
	{
		m_zeroPos = value;
	}

	void SetZeroPos(Point && value)
	{
		m_zeroPos = move(value);
	}

	static size_t CalculateMatrixHash(const Matrix & matrix)
	{
		size_t seed = 0;
		for (size_t i = 0; i < matrix.size(); ++i)
		{
			seed ^= boost::hash_range(matrix[i].begin(), matrix[i].end()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}

	static void PrintWay(CNode *currentNode, const Matrix & matrix, size_t & wayLength)
	{
		if (currentNode->GetFather() != nullptr)
		{
			PrintWay(currentNode->GetFather(), currentNode->GetFather()->matrix, wayLength);
		}
		++wayLength;
		for (size_t y = 0; y < matrix.size(); ++y)
		{
			for (size_t x = 0; x < matrix[y].size(); ++x)
			{
				cout << matrix[y][x] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	static void PrintSteps(CNode *currentNode, const Matrix & matrix, const Point & fromZeroPos, size_t & wayLength)
	{
		if (currentNode->GetFather() != nullptr)
		{
			PrintSteps(currentNode->GetFather(), currentNode->GetFather()->matrix, currentNode->GetZeroPos(), wayLength);
		}
		if (fromZeroPos.x == SIZE_MAX && fromZeroPos.y == SIZE_MAX)
		{
			return;
		}
		++wayLength;
		if (currentNode->GetZeroPos().x > fromZeroPos.x)
		{
			cout << "'left', ";
		}
		else if (currentNode->GetZeroPos().x < fromZeroPos.x)
		{
			cout << "'right', ";
		}
		else if (currentNode->GetZeroPos().y > fromZeroPos.y)
		{
			cout << "'up', ";
		}
		else if (currentNode->GetZeroPos().y < fromZeroPos.y)
		{
			cout << "'down', ";
		}
	}

	static size_t CalculateManhattanDistance(const Matrix & matrix)
	{
		size_t result = 0;
		for (size_t y = 0; y < matrix.size(); ++y)
		{
			for (size_t x = 0; x < matrix[y].size(); ++x)
			{
				if (CONSTANT::WIN_POSITIONS.at(matrix[y][x]).first > x)
				{
					result += CONSTANT::WIN_POSITIONS.at(matrix[y][x]).first - x;
				}
				else
				{
					result += x - CONSTANT::WIN_POSITIONS.at(matrix[y][x]).first;
				}
				if (CONSTANT::WIN_POSITIONS.at(matrix[y][x]).second > y)
				{
					result += CONSTANT::WIN_POSITIONS.at(matrix[y][x]).second - y;
				}
				else
				{
					result += y - CONSTANT::WIN_POSITIONS.at(matrix[y][x]).second;
				}
			}
		}
		return result;
	}

	static bool CheckDepthLimit(CNode *currentNode, Algorithm algorithm, const map<Algorithm, size_t> & maxDepth)
	{
		bool result = false;
		if (maxDepth.find(algorithm) != maxDepth.end())
		{
			if (currentNode->GetDepth() == maxDepth.at(algorithm))
			{
				result = true;
			}
		}
		return result;
	}

	static void CalculateZeroPos(const Matrix & matrix, Point & zeroPos)
	{
		for (size_t y = 0; y < matrix.size(); ++y)
		{
			for (size_t x = 0; x < matrix[y].size(); ++x)
			{
				if (matrix[y][x] == 0)
				{
					zeroPos = Point(x, y);
					return;
				}
			}
		}
	}

	static CNode *CreateNode(CNode *currentNode, int directionX, int directionY)
	{
		Point & currentNodeZeroPos = currentNode->GetZeroPos();
		// bad_alloc chance
		CNode *newNode = new CNode;
		newNode->SetZeroPos(Point(currentNodeZeroPos.x + directionX, currentNodeZeroPos.y + directionY));
		newNode->SetFather(currentNode);
		newNode->matrix = currentNode->matrix;
		swap(newNode->matrix[newNode->GetZeroPos().y][newNode->GetZeroPos().x], newNode->matrix[currentNodeZeroPos.y][currentNodeZeroPos.x]);
		newNode->SetHash(CNode::CalculateMatrixHash(newNode->matrix));
		return newNode;
	}

	Matrix matrix;

private:

	Point m_zeroPos = { 0, 0 };
	size_t m_hash = 0;
	CNode *m_father = nullptr;
	size_t m_depth = 0;
};

