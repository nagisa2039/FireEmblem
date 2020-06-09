#pragma once
#include <list>
#include <vector>
#include <array>
#include <map>
#include "../Utility/Geometry.h"
#include "../Utility/Dir.h"
#include "Team.h"

class Astar
{
public:
	struct ResultPos
	{
		bool attack;
		Vector2Int mapPos;
		ResultPos* prev;
		Dir dir;
		int moveCnt;	// 消費する移動量

		ResultPos() :attack(false), mapPos(Vector2Int()), prev(nullptr), dir(Dir::left), moveCnt(0){};
		ResultPos(const bool atc, const Vector2Int& mapP, ResultPos* parent, const Dir d, const unsigned int mc)
			:attack(atc), mapPos(mapP), prev(parent), dir(d), moveCnt(mc) {};
	};

	struct MapData
	{
		int moveCost;
		Team team;
		bool isHurt;

		MapData():moveCost(1), team(Team::max), isHurt(false){};
		MapData(const int mc, const Team t, const bool hurt) :moveCost(mc), team(t), isHurt(hurt) {};
	};

private:

	enum class SearchState
	{
		non,	// 未探索
		search,	// 探索
		attack	// 攻撃
	};

	struct SearchPos
	{
		Vector2Int mapPos;	//マップ上の座標
		Vector2Int parentPos;	// 親のマップ上の座標
		SearchState state;	// Search状況
		int moveCost;	// 消費する移動量
		SearchPos();
		SearchPos(const Vector2Int& mapPos, const Vector2Int& parent, const SearchState state, const int moveCnt);
	};

	std::vector<std::vector<SearchPos>> _searchPosVec2Move;
	std::vector<std::vector<SearchPos>> _searchPosVec2Attack;
	std::array<Vector2Int, static_cast<size_t>(Dir::max)> _dirTable;

	void ResetSerchPosVec2D(const std::vector<std::vector<MapData>>& mapData);

	bool CheckSearchPosVec2Range(const Vector2Int& checkPos)const;

	// 全ての移動可能範囲を検索
	void AllMoveRouteSerch(const Vector2Int& startMapPos, const int move, 
		const std::vector<std::vector<MapData>>& mapData, std::vector<std::vector<std::list<Astar::ResultPos>>>& resutlPosListVec2, const Team team, const bool addThrough, const bool heal, const bool searchEnemy = false);

public:
	Astar();
	~Astar();

	// 開始位置から終端位置までのマスをリストに格納して返す。見つからなかった場合はリストを空にして返す
	void RouteSearch(const Vector2Int& startMapPos, const int move, const Range& attackRange, 
		const std::vector<std::vector<MapData>>& mapData, std::vector<std::vector<std::list<Astar::ResultPos>>>& resutlPosListVec2, const Team team, const bool heal, const bool searchEnemy = false);

	// 一つのルートを見つけるようの
	bool MoveRouteSerch(const Vector2Int& startMapPos, const int move, const std::vector<std::vector<MapData>>& mapData, std::list<Astar::ResultPos>& resutlPosList, const Team team, const std::list<Astar::ResultPos>& excludeList);
};

