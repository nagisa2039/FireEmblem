#pragma once
#include "Team.h"
#include "Geometry.h"

using CharactorType = unsigned int;

struct CharactorChipInf
{
	Vector2Int mapPos;
	unsigned int level;
	unsigned int groupNum;
	CharactorType type;
	Team team;
	bool active = false;

	uint8_t weaponId;	// 武器

	uint8_t stuffing[7];	// 8バイト区切りにするための詰め物
};
