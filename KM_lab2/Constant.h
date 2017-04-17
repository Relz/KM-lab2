#pragma once
#include <vector>
#include <map>
#include "Matrix.h"

class Constant
{
public:
	class EIGHT_GAME
	{
	public:
		static const std::map<size_t, std::pair<size_t, size_t>> WIN_POSITIONS;
		static const Matrix START_MATRIX;
		static const Matrix WIN_MATRIX;
		static const size_t WIN_MATRIX_HASH;
	};

	class FIFTEEN_GAME
	{
	public:
		static const std::map<size_t, std::pair<size_t, size_t>> WIN_POSITIONS;
		static const Matrix START_MATRIX;
		static const Matrix WIN_MATRIX;
		static const size_t WIN_MATRIX_HASH;
	};
};
