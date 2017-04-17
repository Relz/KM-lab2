#include "Constant.h"

const std::map<size_t, std::pair<size_t, size_t>> Constant::EIGHT_GAME::WIN_POSITIONS =
{
	{ 0,{ 0, 0 } },
	{ 1,{ 1, 0 } },
	{ 2,{ 2, 0 } },
	{ 3,{ 0, 1 } },
	{ 4,{ 1, 1 } },
	{ 5,{ 2, 1 } },
	{ 6,{ 0, 2 } },
	{ 7,{ 1, 2 } },
	{ 8,{ 2, 2 } }
};

const Matrix Constant::EIGHT_GAME::START_MATRIX =
{
	{ 8, 7, 6 },
	{ 5, 4, 3 },
	{ 2, 1, 0 }
};

const Matrix Constant::EIGHT_GAME::WIN_MATRIX =
{
	{ 0, 1, 2 },
	{ 3, 4, 5 },
	{ 6, 7, 8 }
};

const size_t Constant::EIGHT_GAME::WIN_MATRIX_HASH = 3376529474;

const std::map<size_t, std::pair<size_t, size_t>> Constant::FIFTEEN_GAME::WIN_POSITIONS =
{
	{ 0,{ 0, 0 } },
	{ 1,{ 1, 0 } },
	{ 2,{ 2, 0 } },
	{ 3,{ 3, 0 } },
	{ 4,{ 0, 1 } },
	{ 5,{ 1, 1 } },
	{ 6,{ 2, 1 } },
	{ 7,{ 3, 1 } },
	{ 8,{ 0, 2 } },
	{ 9,{ 1, 2 } },
	{ 10,{ 2, 2 } },
	{ 11,{ 3, 2 } },
	{ 12,{ 0, 3 } },
	{ 13,{ 1, 3 } },
	{ 14,{ 2, 3 } },
	{ 15,{ 3, 3 } }
};

// Тяжелый тест, проходится только с помощью метода A* (bad_alloc)
const Matrix Constant::FIFTEEN_GAME::START_MATRIX =
{
	{ 15, 14, 13, 12 },
	{ 11, 10, 9, 8 },
	{ 7, 6, 5, 4 },
	{ 3, 2, 1, 0 }
};

const Matrix Constant::FIFTEEN_GAME::WIN_MATRIX =
{
	{ 0, 1, 2, 3 },
	{ 4, 5, 6, 7 },
	{ 8, 9, 10, 11 },
	{ 12, 13, 14, 15 }
};

const size_t Constant::FIFTEEN_GAME::WIN_MATRIX_HASH = 3350190892;
