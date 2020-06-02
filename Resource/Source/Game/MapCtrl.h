#pragma once
#include <vector>
#include <string>
#include <array>
#include "MapChip.h"
#include <memory>
#include "Astar.h"
#include "CharactorType.h"
#include <functional>
#include "DataBase.h"
#include <windows.h>

class Camera;
class Charactor;

class SceneController;
class Effect;

class MapCtrl
{
private:
	struct SaveData
	{
		CharactorType charType;
		Status status;
		SaveData():charType(CharactorType::max), status(Status()) {};
		SaveData(const CharactorType ct, const Status& st):charType(ct), status(st) {};
	};
	std::vector<SaveData> _charactorSaveDataVec;

	struct MapData
	{
		Map_Chip mapChip;
		CharactorChipInf charactorChip;
	};

	std::vector<std::vector<MapData>> _mapDataVec2;			// マップデータ
	std::shared_ptr<Astar> _astar;

	std::vector<std::shared_ptr<Charactor>>& _charactors;

	// CharactorTypeに対応したCharactorクラスを_charactorsに追加する
	std::array<std::function<void(const CharactorChipInf&, SceneController&, std::vector<std::shared_ptr<Effect>>&, Camera& camera)>,
		static_cast<size_t>(CharactorType::max)> _charactorCreateFuncs;

	int _mapFloorH;
	int _mapChipH;
	int _warSituationH;

	const std::string imageFolderPath;

	void DrawToMapFloorScreen();
	void DrawToMapChipScreen();

	bool FileSave(const HWND hWnd, const char* ext, const char* filter, const char* title, std::string& saveFilePath);
	bool FileLoad(const HWND hWnd, const char* ext, const char* filter, const char* title, std::string& loadFilePath);
	void SaveMapData(const std::string& saveFilePath);
	bool LoadMapData(const std::string& filePath);

	// 指定したマスがマップの変更可能な範囲化を調べる
	bool CheckMapPosPutRange(const Vector2Int& mapPos);

public:
	MapCtrl(std::vector<std::shared_ptr<Charactor>>& charactors);
	~MapCtrl();

	void Draw(const Camera& camera, const bool edit = false);

	bool DrawSortieMass(const Vector2Int& offset, const CharactorChipInf& charactorChipInf, const unsigned int color = 0x00ffff, const unsigned int frameColor = 0x0000ff);

	Size GetChipSize()const;
	Size GetMapSize()const;

	// そのマスにいるキャラクターを返す
	Charactor* GetMapPosChar(const Vector2Int mapPos)const;

	bool SetMapChip(const Vector2Int& mapPos, const Map_Chip mapChip);
	bool DrawMapChip(const Vector2Int& mapPos, const Map_Chip mapChip, const Vector2Int& offset = Vector2Int(0,0));

	const std::vector<std::vector<MapData>>& GetMapData()const;
	CharactorChipInf GetCharactorChipInf(const Vector2Int& mapPos)const;
	bool SetCharactorChip(const CharactorChipInf& charactorChipInf);
	bool DrawCharactorChip(const CharactorChipInf& charactorChipInf, const Vector2Int& offset = Vector2Int(0,0));

	void CreateCharactor(SceneController& ctrl, std::vector<std::shared_ptr<Effect>>& effects, Camera& camera);

	bool SaveMap();
	bool LoadMap(const std::string fileName);
	bool LoadMap();

	// 移動可能なマスを探す
	void RouteSearch(Charactor& charactor);

	// 指定した移動力で行ける場所を探す
	bool MoveRouteSearch(const Vector2Int& startPos, const unsigned int move, std::vector<std::vector<std::list<Astar::ResultPos>>>& resultPosListVec2, const Team team);

	// 指定キャラから視野範囲にいるキャラクターを返す
	Vector2Int SearchMovePos(Charactor& charactor);

	void CreateMapVec(std::vector<std::vector<Astar::MapData>>& mapVec2, const Team team);

	// 指定した座標のMapChipDataを取得する
	DataBase::MapChipData GetMapChipData(const Vector2Int& mapPos)const;

	// 戦況確認用の画像を生成する(更新する)
	void CreateWarSituation()const;

	// 指定した個所を中心にWarSituationを描画する
	void DrawWarSituatin(const Vector2Int center)const;

	// 自軍と敵軍の数を返す(x : 自軍, y : 敵軍)
	Vector2Int GetCharactorCnt()const;

	// 未行動の自軍数を返す
	unsigned int GetCanMoveCnt()const;

	// 全キャラクターの行動範囲計算を行う
	void AllCharactorRouteSearch()const;

	// 指定した集団のmoveActiveを変更
	void SetGroupActive(const unsigned int groupNum, const bool active);

	// 指定したマスがマップの範囲内かを返す
	bool CheckMapDataRange(const Vector2Int& mapPos);

	bool CreateCharactorData();
	bool SaveCharactorData();
	bool LoadCharactorData();

	Status GetLevelInitStatus(const uint8_t level, const CharactorType charType);

	// キャラクターの配列を取得
	const std::vector<std::shared_ptr<Charactor>>& GetCharacots()const;
};