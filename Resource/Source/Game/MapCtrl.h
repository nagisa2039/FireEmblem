#pragma once
#include <vector>
#include <string>
#include <array>
#include "../Utility/Geometry.h"
#include "MapChip.h"
#include <list>
#include <memory>
#include "Astar.h"

class Camera;
class Charactor;

struct Astar::ResultPos;

class MapCtrl
{
public:
	struct DrawData
	{
		Vector2Int leftup;
		Size size;
		std::string path;

		DrawData() {};

		DrawData(Vector2Int leftup, Size size, std::string path) :
			leftup(leftup), size(size), path(path) {};

	};

	struct MapChipData
	{
		DrawData drawData;
		int moveCost;

		MapChipData() {};
		MapChipData(const DrawData& dd, const int mc) :
			drawData(dd), moveCost(mc) {};
	};

private:
	std::vector<std::vector<Map_Chip>> _mapData;			// マップデータ
	std::array<MapChipData, Map_Chip_Max> _mapChipData;	// マップチップのデータ
	std::shared_ptr<Astar> _astar;

	std::vector<std::shared_ptr<Charactor>>& _charactors;

	int _mapFloorH;
	int _mapChipH;
	const std::string imageFolderPath;

	void DrawToMapFloorScreen();
	void DrawToMapChipScreen();

public:
	MapCtrl(std::vector<std::shared_ptr<Charactor>> charactors);
	~MapCtrl();

	void Draw(const Camera& camera, const bool edit = false);

	Size GetChipSize()const;
	Size GetMapSize()const;

	bool SetMapChip(const Vector2Int& mapPos, const Map_Chip mapChip);
	bool DrawMapChip(const Vector2Int& mapPos, const Map_Chip mapChip, const Vector2Int& offset = Vector2Int(0,0));

	bool SaveMap(const std::string fileName);
	bool LoadMap(const std::string fileName);

	std::list<Astar::ResultPos> RouteSearch(const Vector2Int& startMapPos, const int move);
};

