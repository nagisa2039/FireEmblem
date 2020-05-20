#include "Charactor.h"
#include "MapCtrl.h"
#include "Camera.h"
#include "../Utility/DxLibUtility.h"
#include "Animator.h"
#include "../Scene/SceneController.h"
#include "../Scene/BattleScene.h"
#include "Effect.h"
#include <DxLib.h>
#include "Application.h"
#include "FlyText.h"
#include <algorithm>
#include "FileSystem.h"
#include "CorsorTarget.h"

using namespace std;

void Charactor::NormalUpdate(const Input& input)
{
	if (_moveStandby)
	{
		if (--_rigid > 0)return;

		_moveStandby = false; 
		SearchAndMove();
	}

	Move();

	_animator->ChangeAnim(_dirTable[_dir].animName);
	_animator->Update();
}

void Charactor::BattaleStartUpdate(const Input& input)
{
	if (!_battleStartEffect->GetDelete())return;

	auto charactor = _mapCtrl.GetMapPosChar((*_moveDirList.begin()).mapPos);
	BattaleStart(charactor);

	_updater = &Charactor::NormalUpdate;
}

void Charactor::BattaleStart(Charactor* charactor)
{
	_mapCtrl.SetGroupActive(_groupNum, true);
	_mapCtrl.SetGroupActive(charactor->GetGroupNum(), true);

	// 戦闘
	_controller.PushScene(make_shared<BattleScene>
		(GetBattleC(), charactor->GetBattleC(), _controller, _camera.GetCameraOffset()));
}

void Charactor::DyingUpdate(const Input& input)
{
	_dyingAnimAlphaTL->Update();
	if (_dyingAnimAlphaTL->GetEnd())
	{
		_delete = true;
	}
}

void Charactor::NormalDraw()
{
	auto offset = _camera.GetCameraOffset();
	auto chipSize = _mapCtrl.GetChipSize().ToVector2Int();

	if (!_canMove)
	{
		SetDrawBright(128, 128, 128);
	}
	Vector2Int drawPos = offset + _pos.ToVector2Int();
	_animator->Draw(drawPos, _mapCtrl.GetChipSize());

	SetDrawBright(255, 255, 255);

	// アイコンの描画
	int handle = Application::Instance().GetFileSystem()->GetImageHandle(_iconPath.c_str());
	DrawGraph(_camera.GetCameraOffset() + _pos.ToVector2Int(), handle, false);

	// HPBerの描画
	Size hpberSize = (chipSize * Vector2(0.8f, 0.1f)).ToSize();
	Vector2Int hpLeftup		= drawPos + chipSize * Vector2(0.1f, 0.8f);
	Vector2Int hpRightdown	= hpLeftup + hpberSize;
	DrawBox(hpLeftup + Vector2Int::One() * -2, hpRightdown + Vector2Int::One() * +2, 0xffffff);
	DrawBox(hpLeftup + Vector2Int::One() * -1, hpRightdown + Vector2Int::One() * +1, 0xaaaaaa);
	float berLength = max(min(static_cast<float>(_status.health) / static_cast<float>(_startStatus.health), 1.0f), 0.0f);
	DrawBox(hpLeftup, hpLeftup + hpberSize.ToVector2Int() * Vector2(berLength, 1.0f), GetTeamColor());

	// レベルの描画
	DrawFormatString(drawPos.x, drawPos.y, 0x000000, "Level.%d", _status.level);
}

void Charactor::DyingDraw()
{
	auto offset = _camera.GetCameraOffset();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * _dyingAnimAlphaTL->GetValue());
	_animator->Draw(offset + _pos.ToVector2Int(), _mapCtrl.GetChipSize());
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void Charactor::Move()
{
	if ((!_isMoveAnim || _moveDirList.size() == 0))
	{
		return;
	}

	auto it = _moveDirList.begin();
	_dir = it->dir;
	if (it->attack)
	{
		// そのマスに敵がいたら戦闘
		auto charactor = _mapCtrl.GetMapPosChar(it->mapPos);
		if (charactor != nullptr && _team != charactor->GetTeam())
		{
			if (_team == Team::player)
			{
				BattaleStart(charactor);
				return;
			}
			_battleStartEffect->SetPos(charactor->GetCenterPos());
			_battleStartEffect->Reset();
			_effects.emplace_back(_battleStartEffect);
			_updater = &Charactor::BattaleStartUpdate;
			return;
		}
		else
		{
			MoveEnd(_status.move > 0);
		}
		return;
	}

	_pos += (_dirTable[it->dir].moveVec * _moveSpeed).ToVector2();
	if (_pos.ToVector2Int() % _mapCtrl.GetChipSize().ToVector2Int() == Vector2Int(0, 0))
	{
		_moveDirList.pop_front();
		if (_moveDirList.size() <= 0)
		{
			MoveEnd(_status.move > 0);
		}
	}
}

unsigned int Charactor::GetTeamColor() const
{
	switch (_team)
	{
	case Team::player:
		return 0x0000ff;
	case Team::enemy:
		return 0xff0000;
	default:
		return 0xffffff;
	}
}

void Charactor::DrawMovableMass(const uint8_t alpha) const
{
	auto offset = _camera.GetCameraOffset();
	auto chipSize = _mapCtrl.GetChipSize();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	int currentX = 0;
	list<int> drawYList;
	for (const auto& movePos : _resutlPosList)
	{
		if (currentX != movePos.mapPos.x)
		{
			currentX = movePos.mapPos.x;
			drawYList.clear();
		}
		bool skip = false;
		for (const auto drawY : drawYList)
		{
			if (drawY == movePos.mapPos.y)
			{
				skip = true;
				break;
			}
		}

		if (skip) { continue; }
		drawYList.emplace_back(movePos.mapPos.y);

		Rect box(offset + (movePos.mapPos * chipSize.ToVector2Int() + chipSize*0.5) +-1, chipSize);
		unsigned int color = movePos.attack ? 0xff0000 : 0x0000ff;
		box.Draw(color);
		box.Draw(color, false);

		Vector2Int leftup = offset + movePos.mapPos * chipSize.ToVector2Int();
		DrawFormatString(leftup.x, leftup.y, 0x000000, "%d", movePos.moveCnt);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}

bool Charactor::GetIsSelect() const
{
	return _isSelect;
}

bool Charactor::GetCanMove() const
{
	return _canMove;
}

bool Charactor::GetMoveActive() const
{
	return _moveActive;
}

bool Charactor::GetMoveStandby() const
{
	return _moveStandby;
}

Charactor::Status Charactor::GetStartStatus() const
{
	return _startStatus;
}

Charactor::Status Charactor::GetStatus() const
{
	Status status = _status;

	MapCtrl::MapChipData mapchipData = _mapCtrl.GetMapChipData(GetMapPos());
	status.avoidanceCorrection = mapchipData.avoidance;
	status.defenseCorrection = mapchipData.defense;

	return status;
}

bool Charactor::GetIsDying() const
{
	return _isDying;
}

Dir Charactor::GetDir() const
{
	return _dir;
}

Vector2 Charactor::GetCenterPos() const
{
	return Vector2(_pos + (_mapCtrl.GetChipSize() * 0.5f).ToVector2());
}

BattleCharactor& Charactor::GetBattleC() const
{
	return *_battleC;
}

bool Charactor::GetIsMoveAnim() const
{
	return _isMoveAnim;
}

Range Charactor::GetAttackRange() const
{
	return _attackRange;
}

const std::string& Charactor::GetName() const
{
	return _name;
}

const Astar::ResultPos* Charactor::GetResutlPos(const Vector2Int& mapPos)
{
	for (const auto& resutlPos : _resutlPosList)
	{
		if (resutlPos.mapPos == mapPos)
		{
			return &resutlPos;
		}
	}
	return nullptr;
}

unsigned int Charactor::GetGroupNum() const
{
	return _groupNum;
}

void Charactor::SetIsSelect(const bool select)
{
	_isSelect = select;
}

void Charactor::SetIsDying()
{
	_isDying = true;

	_updater = &Charactor::DyingUpdate;
	_drawer	 = &Charactor::DyingDraw;
}

void Charactor::SetDir(const Dir dir)
{
	_dir = dir;
}

void Charactor::SetStatus(const Status& status)
{
	_status = status;
}

void Charactor::SetMoveActive(const bool active)
{
	_moveActive = active;
}

void Charactor::SetMoveStandby(const int time)
{
	_moveStandby = true;
	_rigid = time;
}

void Charactor::MoveEnd(const bool canMove)
{
	if (!canMove)
	{
		_status.move = _startStatus.move;
	}
	_canMove = canMove;
	_isMoveAnim = false;
	_camera.PopTargetActor();

	_mapCtrl.AllCharactorRouteSearch();
}

void Charactor::RouteSearch()
{
	_mapCtrl.RouteSearch(*this);
}

void Charactor::TurnReset()
{
	_canMove = true;
}

void Charactor::SearchAndMove()
{
	MoveMapPos(_mapCtrl.SearchMovePos(*this));
}

std::list<Astar::ResultPos>& Charactor::GetResutlPosList()
{
	return _resutlPosList;
}

void Charactor::AddDamage(const int damage)
{
	_status.health = max(_status.health - damage, 0);
	if (_status.health <= 0)
	{
		SetIsDying();
	}
	SetStatus(_status);
}

void Charactor::DrawCharactorIcon(const Rect& drawRect)const
{
	DrawRectExtendGraph(drawRect.Left(), drawRect.Top(), drawRect.Right(), drawRect.Botton(), 32, 0, 32, 32, _animator->GetImageH(), true);
}

void Charactor::DrawRoute(const Vector2Int& targetPos)
{
	if (targetPos == GetMapPos()) return;

	auto itr = _resutlPosList.begin();
	bool locate = false;
	for (; itr != _resutlPosList.end(); itr++)
	{
		if (itr->mapPos == targetPos)
		{
			locate = true;
			break;
		}
	}

	if (!locate)return;

	list<Astar::ResultPos*> routeList;
	bool begin = true;
	for (auto resutlPos = &*itr; resutlPos != nullptr; resutlPos = resutlPos->parent)
	{
		if (resutlPos->attack)continue;
		routeList.emplace_back(resutlPos);
	}

	for (const auto& route : routeList)
	{
		auto offset = _camera.GetCameraOffset();
		auto chipSize = _mapCtrl.GetChipSize();
		size_t dir_idx = static_cast<size_t>(route->dir);

		if (0 > dir_idx || dir_idx >= _dirTable.size())return;

		Vector2Int endPos = offset + (route->parent == nullptr ? GetMapPos() : route->parent->mapPos) * chipSize + chipSize * 0.5f;
		Vector2Int startPos = offset + route->mapPos * chipSize + chipSize * 0.5f;
		DrawLine(startPos, endPos, 0xffff00, 10);
		if (begin)
		{
			auto arrowH = Application::Instance().GetFileSystem()->GetImageHandle("Resource/Image/UI/arrow.png");
			Size arrowSize;
			GetGraphSize(arrowH, arrowSize);
			DrawRotaGraph(startPos, chipSize.w / static_cast<float>(arrowSize.w), _dirTable[dir_idx].angle, arrowH, true);
			begin = false;
		}
	}

}

Charactor::Charactor(const uint8_t level, const Vector2Int& mapPos, const Team team, const unsigned int groupNum, MapCtrl& mapCtrl, SceneController& ctrl,
	std::vector<std::shared_ptr<Effect>>& effects, Camera& camera)
	: _team(team), _groupNum(groupNum), _mapCtrl(mapCtrl), _controller(ctrl), _effects(effects), Actor(camera)
{
	_pos = (mapPos * _mapCtrl.GetChipSize().ToVector2Int()).ToVector2();
	_isMoveAnim = false;

	_moveSpeed = 4;
	_isSelect = false;
	_canMove = true;
	_moveActive = false;
	_isDying = false;

	_animator = make_shared<Animator>();

	_status = Status();

	_dyingAnimAlphaTL = make_unique<Track<float>>();
	_dyingAnimAlphaTL->AddKey(0, 1.0f);
	_dyingAnimAlphaTL->AddKey(60, 0.0f);

	_updater = &Charactor::NormalUpdate;
	_drawer = &Charactor::NormalDraw;

	_attackRange = Range(2,2);

	_battleStartEffect = make_shared<CorsorTarget>(Vector2Int(), _camera, true, _mapCtrl.GetChipSize());

	_rigid = 0;
	_moveStandby = false;
}

Charactor::~Charactor()
{
}

void Charactor::Update(const Input& input)
{
	(this->*_updater)(input);
}

void Charactor::Draw()
{
	(this->*_drawer)();
}

void Charactor::InitAnim()
{
	const Size divSize = Size(32, 32);

	int cnt = 0;
	auto nextRectCenterOffset = [&](std::vector<Rect>& animRectVec, int cnt)
	{
		for (auto& rect : animRectVec)
		{
			rect.center.y += divSize.h;
		}
	};

	std::vector<Rect> animRectVec;
	animRectVec.emplace_back(Rect(Vector2Int(16, 16), divSize));
	animRectVec.emplace_back(Rect(Vector2Int(16 + divSize.w * 2, 16), divSize));

	_animator->AddAnim("DownWalk", animRectVec, 30, true);
	nextRectCenterOffset(animRectVec, ++cnt);
	_animator->AddAnim("LeftWalk", animRectVec, 30, true);
	nextRectCenterOffset(animRectVec, ++cnt);
	_animator->AddAnim("RightWalk", animRectVec, 30, true);
	nextRectCenterOffset(animRectVec, ++cnt);
	_animator->AddAnim("UpWalk", animRectVec, 30, true);

	_animator->ChangeAnim("LeftWalk");

	_dirTable[Dir::left]	= DirInf(Vector2Int(-1, 0), "LeftWalk", 270.0f * DX_PI / 180.0f);
	_dirTable[Dir::right]	= DirInf(Vector2Int(1, 0), "RightWalk", 90.0f * DX_PI / 180.0f);
	_dirTable[Dir::up]		= DirInf(Vector2Int(0, -1), "UpWalk",	0.0f * DX_PI / 180.0f);
	_dirTable[Dir::down]	= DirInf(Vector2Int(0, 1), "DownWalk",	180.0f *  DX_PI / 180.0f);

	_dir = Dir::down;
}

void Charactor::AnimRestart()
{
	_animator->AnimRestart();
}

Vector2Int Charactor::GetMapPos() const
{
	return _pos.ToVector2Int() / _mapCtrl.GetChipSize().ToVector2Int();
}

Team Charactor::GetTeam() const
{
	return _team;
}

bool Charactor::MoveMapPos(const Vector2Int& mapPos)
{
	if (mapPos == GetMapPos() || _isMoveAnim)
	{
		return false;
	}

	_moveDirList.clear();

	std::list<Astar::ResultPos> targetPosList;
	for (const auto& resultPos : _resutlPosList)
	{
		if (mapPos == resultPos.mapPos)
		{
			targetPosList.emplace_back(resultPos);
		}
	}

	if (targetPosList.size() <= 0)
	{
		return false;
	}

	Astar::ResultPos startResultPos = *targetPosList.begin();
	auto targetCharactor = _mapCtrl.GetMapPosChar(mapPos);
	if (targetCharactor != nullptr)
	{
		auto targetRange = targetCharactor->GetAttackRange();
		Range criticalRange = _attackRange.GetCriticalRange(targetRange);
		if (criticalRange != Range(0, 0))
		{
			for (const auto& targetPos : targetPosList)
			{
				Vector2Int startPos = targetPos.parent == nullptr ? GetMapPos() : targetPos.parent->mapPos;
				Vector2Int disVec = targetPos.mapPos - startPos;
				unsigned int distance = abs(disVec.x) + abs(disVec.y);
				if (criticalRange.Hit(distance))
				{
					startResultPos = targetPos;
					break;
				}
			}
		}
	}

	// _resultPosListからmoveDirListを作るために
	// 終わりから始まりへと辿ったリストを作る
	std::list<Astar::ResultPos> oneLineResutlList;
	oneLineResutlList.clear();
	oneLineResutlList.emplace_front(startResultPos);
	Astar::ResultPos* rp = startResultPos.parent;

	// attackマスが何マス続くかの数(攻撃範囲内かの確認のため)
	int attackMassCnt = 0;
	for (; rp->parent != nullptr;)
	{
		oneLineResutlList.emplace_back(*rp);
		if (rp->attack)
		{
			attackMassCnt++;
		}
		rp = rp->parent;
	}


	// 攻撃範囲外のマスを削除
	for (int i = _attackRange.max; i < attackMassCnt; i++)
	{
		oneLineResutlList.pop_front();
	}

	list<Astar::ResultPos> addResultPosList;
	bool coverCheck = true;
	for (auto itr = oneLineResutlList.begin(); itr != oneLineResutlList.end(); itr++)
	{
		if (itr->moveCnt > _status.move) continue;

		if (itr->attack)
		{
			_moveDirList.emplace_front(MoveInf(itr->dir, itr->attack, itr->mapPos));
			continue;
		}


		if (!coverCheck)
		{
			_moveDirList.emplace_front(MoveInf(itr->dir, itr->attack, itr->mapPos));
			continue;
		}

		if (_mapCtrl.GetMapPosChar(itr->mapPos) == nullptr)
		{
			coverCheck = false;
			_moveDirList.emplace_front(MoveInf(itr->dir, itr->attack, itr->mapPos));
		}
		else
		{
			addResultPosList.clear();
			if (_mapCtrl.MoveRouteSearch(itr->mapPos, max(0, _status.move - itr->moveCnt - 1), addResultPosList, _team))
			{
				coverCheck = false;
				_moveDirList.emplace_front(MoveInf(itr->dir, itr->attack, itr->mapPos));
				for (const auto& addResultPos : addResultPosList)
				{
					_moveDirList.emplace_back(MoveInf(addResultPos.dir, addResultPos.attack, addResultPos.mapPos));
				}
				for (auto resutlPos = itr->parent; resutlPos->parent != nullptr; resutlPos = resutlPos->parent)
				{
					_moveDirList.emplace_front(MoveInf(resutlPos->dir, resutlPos->attack, resutlPos->mapPos));
				}
				break;
			}
		}
	}

	_isMoveAnim = true;

	_status.move = max(_status.move - startResultPos.moveCnt, 0);

	return false;
}

int Charactor::Status::GetDamage(const Status& target)const
{
	return max(power - target.defense - target.defenseCorrection, 0);
}

int Charactor::Status::GetHitRate() const
{
	return 100 + skill * 2;
}

int Charactor::Status::GetHit(const Status& target) const
{
	return min(100, max(0, GetHitRate() - target.GetAvoidance()));
}

int Charactor::Status::GetCriticalRate() const
{
	return min(100, max(0, skill / 5));
}

int Charactor::Status::GetCritical(const Status& target) const
{
	return min(100, max(0, skill / 5));
}

int Charactor::Status::GetDifense() const
{
	return this->defense + this->defenseCorrection;
}

int Charactor::Status::GetMagicDifense() const
{
	return this->magic_defense + this->defenseCorrection;
}

int Charactor::Status::GetAvoidance() const
{
	return GetAttackSpeed() * 2 + this->avoidanceCorrection;
}

bool Charactor::Status::CheckPursuit(const Status& target) const
{
	return GetAttackSpeed() - target.GetAttackSpeed() >= 4;
}

int Charactor::Status::GetAttackSpeed()const
{
	return this->speed;
}
