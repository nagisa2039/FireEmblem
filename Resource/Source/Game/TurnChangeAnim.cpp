#include "TurnChangeAnim.h"
#include "Input.h"
#include "Camera.h"
#include "Application.h"
#include "FileSystem.h"
#include "DxLibUtility.h"
#include <DxLib.h>

using namespace std;

TurnChangeAnim::TurnChangeAnim()
{
	teamInfs[static_cast<size_t>(Team::player)] = TeamInf("PLAYER");
	teamInfs[static_cast<size_t>(Team::enemy)]	= TeamInf("ENEMY");

	_currentTeam = Team::player;
	_animEnd = false;

	_exrateTL = make_unique<TimeLine<float>>();
	_exrateTL->AddKey(0,	2);
	_exrateTL->AddKey(15,	1);
	_exrateTL->AddKey(105, 1);
	_exrateTL->AddKey(120, 2);

	_alphaTL = make_unique<TimeLine<float>>();
	_alphaTL->AddKey(0, 0);
	_alphaTL->AddKey(15, 255);
	_alphaTL->AddKey(105, 255);
	_alphaTL->AddKey(120, 0);

	_angleTL = make_unique<TimeLine<float>>(true);
	_angleTL->AddKey(0, 0);
	_angleTL->AddKey(59, 59/60.0f * DX_PI*2);

	_alphaTL->Reset();
	_exrateTL->Reset();
	_angleTL->Reset();
}

TurnChangeAnim::~TurnChangeAnim()
{
}

void TurnChangeAnim::Update(const Input& input)
{
	if (_animEnd) return;

	_alphaTL->Update();
	_exrateTL->Update();
	_angleTL->Update();
}

void TurnChangeAnim::Draw(const Camera& camera)
{
	auto alpha = _alphaTL->GetValue();
	auto exrate = _exrateTL->GetValue();
	_animEnd = _alphaTL->GetEnd();

	if (_animEnd) return;

	auto wsize = Application::Instance().GetWindowSize();
	auto gHangle = Application::Instance().GetFileSystem()->GetImageHandle("Resource/Image/Battle/magicCircle.png");
	auto blurHandle = Application::Instance().GetFileSystem()->GetImageHandle("Resource/Image/Battle/magicCircleBlur.png");

	auto teamInf = teamInfs[static_cast<size_t>(_currentTeam)];

	// 魔法陣の描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	// ぼかし
	auto angle = _angleTL->GetValue();
	auto teamColor = GetTeamColor(_currentTeam);
	SetDrawBright((teamColor << 8) >> 24, (teamColor << 16) >> 24, (teamColor << 24) >> 24);
	DrawRotaGraph(wsize.w / 2, wsize.h / 2, exrate, _angleTL->GetValue(), blurHandle, true);
	SetDrawBright(255,255,255);

	// オリジナル
	DrawRotaGraph(wsize.w / 2, wsize.h / 2, exrate, _angleTL->GetValue(), gHangle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	// 名前表示
	char str[20];
	sprintf_s(str, 20, "%s%s", teamInf.name, " TURN");
	Size strSize;
	int lineCnt;
	auto fontHandle = Application::Instance().GetFileSystem()->GetFontHandle("choplin100");
	GetDrawFormatStringSizeToHandle(&strSize.w, &strSize.h, &lineCnt, fontHandle, str);
	auto nameDrawPos = GetDrawPos(wsize.ToVector2Int() * 0.5f, strSize, Anker::center);
	DrawFormatStringToHandle(nameDrawPos.x, nameDrawPos.y, teamColor, fontHandle, str);
}

void TurnChangeAnim::TurnStart(const Team team)
{
	_animEnd = false;
	_currentTeam = team;

	_alphaTL->Reset();
	_exrateTL->Reset();
	_angleTL->Reset();
}

bool TurnChangeAnim::GetAnimEnd() const
{
	return _animEnd;
}
