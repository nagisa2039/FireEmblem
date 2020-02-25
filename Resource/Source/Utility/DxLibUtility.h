#pragma once
#include <array>
#include <functional>
#include "Geometry.h"

void DrawBox(const Vector2Int& leftup, const Vector2Int& rightdown, const unsigned int color, const bool fillFlag = true);

void DrawLine(const Vector2Int& start, const Vector2Int& end, const unsigned int color, const int thickness = 1);

void DrawGraph(const Vector2Int& leftup, const int handle, const bool transFlag = true);

void DrawCircle(const Vector2Int& center, const int radius, const unsigned int color, const bool fillFlag = true);

void DrawFormatStringToHandle(const Vector2Int& leftup, unsigned int Color, const int FontHandle, const char* FormatString);

class CalDrawPos
{
private:
	std::array<std::function<Vector2Int(const Vector2Int&, const Size&)>, static_cast<int>(Anker::max)> _calFunArray;

public:
	CalDrawPos();
	~CalDrawPos();

	Vector2Int GetDrawPos(const Vector2Int& drawPos, const Size& size, const Anker anker);
};