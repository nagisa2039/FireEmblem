#pragma once
#include "Actor.h"
#include <list>
#include "Astar.h"
#include <array>
#include "../Utility/Dir.h"
#include "Team.h"

class Animator;
class MapCtrl;

class Charactor :
	public Actor
{
public:
	struct MoveInf
	{
		Dir dir;
		bool attack;

		MoveInf() : dir(Dir::left), attack(false) {};
		MoveInf(const Dir d, const bool at) : dir(d), attack(at) {};
	};

	struct Status
	{
		uint8_t level;
		uint8_t health;
		uint8_t power;
		uint8_t move;

		Status(): level(1), health(1), power(1), move(1) {};
		Status(const uint8_t lv, const uint8_t he, const uint8_t pw, const uint8_t mv)
			: level(lv), health(he), power(pw), move(mv) {};
	};

protected:
	struct DirInf
	{
		Vector2Int moveVec;
		std::string animName;

		DirInf() : moveVec(Vector2Int()), animName("") {};
		DirInf(const Vector2Int& vec2Int, const std::string& animN)
			: moveVec(vec2Int), animName(animN) {};
	};

	Team _team;
	MapCtrl& _mapCtrl;
	std::list<Astar::ResultPos> _resutlPosList;
	std::shared_ptr<Animator> _animator;

	bool isMoveAnim;	// 移動アニメーション中
	std::list<MoveInf> _moveDirList;
	std::array<DirInf, Dir::max> _dirTable;

	bool _canMove;

	int _moveSpeed;
	bool _isSelect;
	Dir _dir;

	Status _status; 
	Status _startStatus;

	void Move();

	unsigned int GetTeamColor()const;
	void DrawMovableMass(const Camera& camera)const;

public:
	Charactor(const Vector2Int& mapPos, const Team team, MapCtrl& mapCtrl);
	~Charactor();

	virtual void Update(const Input& input)override = 0;
	virtual void Draw(const Camera& camera)override = 0;
	Vector2Int GetMapPos()const;
	Team GetTeam()const;

	bool MoveMapPos(const Vector2Int& mapPos);

	bool GetIsSelect()const;
	bool GetCanMove()const;
	Status GetStatus()const;

	void SetIsSelect(const bool select);
	void MoveEnd();

	void RouteSearch();
	void TurnReset();
};

