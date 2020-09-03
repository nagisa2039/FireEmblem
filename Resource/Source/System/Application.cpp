 #include <DxLib.h>
#include <time.h>
#include "../Utility/Input.h"
#include "../Scene/SceneController.h"
#include "../Scene/PlayScene.h"
#include "Application.h"
#include "../Utility/Geometry.h"
#include "../System/FileSystem.h"
#include "../Scene/PlayScene.h"
#include "../Scene/MapEditScene.h"
#include "../Scene/PlayScene.h"
#include "../Scene/MapSelectScene.h"
#include "DataBase.h"
#include "SaveData.h"
#include "AnkerCalculation.h"

using namespace std;

constexpr auto default_window_size_wide		= 1280;
constexpr auto default_window_size_hight	= 720;

Application::Application()
{
}


Application::~Application()
{
}

Application::Configure & Application::GetConfigure(void)
{
	return *_configure;
}

const Size & Application::GetWindowSize(void)
{
	return _configure->GetWindowSize();
}

const DataBase& Application::GetDataBase() const
{
	return *_dataBase;
}

SaveData& Application::GetSaveData()
{
	return *_saveData;
}

FileSystem& Application::GetFileSystem()
{
	return *_fileSystem;
}

const AnkerCalculation& Application::GetAnkerCalculation() const
{
	return *_ankerCalculation;
}

SceneController& Application::GetSceneController() const
{
	assert(_sceneController);
	return *_sceneController;
}

double Application::GetDeltaTime() const
{
	return static_cast<double>(_end.QuadPart - _start.QuadPart) / static_cast<double>(_freq.QuadPart);
}

unsigned int Application::GetFPS()const
{
	auto deltaTime = GetDeltaTime();
	return static_cast<unsigned int>(1.0 / deltaTime);
}

bool Application::Initialize()
{
	_configure = make_unique<Configure>();
	_dataBase = make_unique<DataBase>();
	_saveData = make_unique<SaveData>();
	_ankerCalculation = make_unique<AnkerCalculation>();

	// Dxlibの初期化
	ChangeWindowMode(TRUE);
	auto wSize = GetWindowSize();
	SetGraphMode(wSize.w, wSize.h, 32);
	if (DxLib_Init() != 0) { return false; }
	srand(static_cast<unsigned int>(time(NULL)));
	SetDrawScreen(DX_SCREEN_BACK);
	SetWindowText("Commander");
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	_fileSystem = make_unique<FileSystem>();

	// inputの初期化
	_input = make_unique<Input>();

	// コマンドの登録
	KeySetUp();

	// フォント読込
	FontSetUp();

	_sceneController = make_unique<SceneController>();
	//_sceneController->ChangeScene(make_unique<PlayScene>(*_sceneController));
	//_sceneController->ChangeScene(make_unique<MapEditScene>(*_sceneController));
	_sceneController->ChangeScene(make_unique<MapSelectScene>(*_sceneController));


	_freq = {};
	_start = {};
	_end = {};
	QueryPerformanceFrequency(&_freq);
	QueryPerformanceCounter(&_end);

	return true;
}

void Application::FontSetUp()
{
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin20", 20, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin30", 30, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin40", 40, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin50", 50, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin60", 60, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin70", 70, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin80", 80, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin100", 100, 1, false, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin200", 200, 1, false, false);

	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin20edge", 20, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin30edge", 30, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin40edge", 40, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin50edge", 50, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin60edge", 60, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin70edge", 70, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin80edge", 80, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin100edge", 100, 1, true, false);
	_fileSystem->FontInit("Resource/Font/Choplin.ttf", "Choplin", "choplin200edge", 200, 1, true, false);
}

void Application::KeySetUp()
{
	const auto keybord = Input::PeripheralType::keybord;
	_input->AddCommand("down", keybord, KEY_INPUT_DOWN);
	_input->AddCommand("up", keybord, KEY_INPUT_UP);
	_input->AddCommand("right", keybord, KEY_INPUT_RIGHT);
	_input->AddCommand("left", keybord, KEY_INPUT_LEFT);
	_input->AddCommand("pause", keybord, KEY_INPUT_P);
	_input->AddCommand("ctrl", keybord, KEY_INPUT_LCONTROL);
	_input->AddCommand("debug", keybord, KEY_INPUT_D);
	_input->AddCommand("ok", keybord, KEY_INPUT_SPACE);
	_input->AddCommand("change", keybord, KEY_INPUT_C);
	_input->AddCommand("team", keybord, KEY_INPUT_T);
	_input->AddCommand("back", keybord, KEY_INPUT_BACK);
	_input->AddCommand("status", keybord, KEY_INPUT_X);

	// マウスの入力
	const auto mouse = Input::PeripheralType::mouse;
	_input->AddCommand("ok", mouse, MOUSE_INPUT_LEFT);
	_input->AddCommand("back", mouse, MOUSE_INPUT_RIGHT);
	_input->AddCommand("status", mouse, MOUSE_INPUT_MIDDLE);

	// プレイヤーごとのパッド入力
	const auto pad = Input::PeripheralType::gamepad;
	_input->AddCommand("down", pad, PAD_INPUT_DOWN);
	_input->AddCommand("up", pad, PAD_INPUT_UP);
	_input->AddCommand("right", pad, PAD_INPUT_RIGHT);
	_input->AddCommand("left", pad, PAD_INPUT_LEFT);
	_input->AddCommand("pause", pad, PAD_INPUT_START);
	_input->AddCommand("ok", pad, PAD_INPUT_1);		// Aボタン
	_input->AddCommand("back", pad, PAD_INPUT_2);		// Bボタン
	_input->AddCommand("status", pad, PAD_INPUT_4);	// Yボタン
}

void Application::Run()
{
	while (ProcessMessage() == 0 && !CheckHitKey(KEY_INPUT_ESCAPE))
	{
		_start = _end;
		QueryPerformanceCounter(&_end);

		_input->Update();

		ClsDrawScreen();

		_sceneController->SceneUpdate(*_input);

		// DrawFormatString(0,0, 0xffffff, "FPS : %d", GetFPS());

		ScreenFlip();
	}
	DxLib_End();
}

void Application::Terminate()
{
}

Application::Configure::Configure()
{
	_winSize = { default_window_size_wide, default_window_size_hight };
}