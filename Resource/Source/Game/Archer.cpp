#include "Archer.h"
#include "MapCtrl.h"
#include "../Utility/DxLibUtility.h"
#include "Camera.h"
#include "Animator.h"
#include "../Scene/SceneController.h"
#include "Effect//BattleEffect/ArrowEffect.h"
#include "Application.h"
#include <DxLib.h>
#include "FileSystem.h"
#include "DataBase.h"

using namespace std;

Archer::Archer(const uint8_t level, const Vector2Int& mapPos, const Team team, const unsigned int groupNum, MapCtrl& mapCtrl, SceneController& ctrl,
	std::vector<std::shared_ptr<Effect>>& effects, Camera& camera)
	:Charactor(level, mapPos, team, groupNum, mapCtrl, ctrl, effects, camera)
{
	CharactorDataInit(CharactorType::archer, level);
	_battleC = make_shared<ArcherBC>(*this, _animator->GetImageH(), _camera);
}

Archer::~Archer()
{
}

ArcherBC::ArcherBC(Charactor& charactor, const int imageHandle, Camera& camera) : BattleCharactor(charactor, imageHandle, camera)
{
	const Size divSize = Size(32, 32);
	_animator->SetImageHandle(imageHandle);

	int cnt = 0;
	auto nextRectCenterOffset = [&](std::vector<Rect>& animRectVec, int cnt)
	{
		for (auto& rect : animRectVec)
		{
			rect.center.y += divSize.h;
		}
	};

	std::vector<Rect> animRectVec;
	animRectVec.clear();

	animRectVec.emplace_back(Rect(Vector2Int(16, 16 + 32 * 1), divSize));
	animRectVec.emplace_back(Rect(Vector2Int(16 + divSize.w * 2, 16 + 32 * 1), divSize));
	_animator->AddAnim("LeftWalk", animRectVec, 30, true);
	animRectVec.clear();

	animRectVec.emplace_back(Rect(Vector2Int(16, 16 + 32 * 2), divSize));
	animRectVec.emplace_back(Rect(Vector2Int(16 + divSize.w * 2, 16 + 32 * 2), divSize));
	_animator->AddAnim("RightWalk", animRectVec, 30, true);
}

ArcherBC::~ArcherBC()
{
}

std::shared_ptr<Effect> ArcherBC::CreateAttackEffect(std::vector<std::shared_ptr<Effect>>& effects, bool critical)
{
	return make_shared<ArrowEffect>(*this, *_targetChar, effects, critical, _camera);
}

