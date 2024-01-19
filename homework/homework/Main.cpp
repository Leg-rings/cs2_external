// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm")
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <map>
#include <list>
#include <offsets.hpp>
#include <client.dll.hpp>
#include <vector.hpp>
#include <TlHelp32.h>
#include <iostream>
#include <dwmapi.h>

struct RCS
{
	ptrdiff_t A;
	ptrdiff_t B;
};

//进程量
const wchar_t* ProcessName = L"cs2.exe";
const wchar_t* ModuleName = L"client.dll";
ptrdiff_t ModuleAddress;
HANDLE hProcess;
DWORD ProcessID;
HWND Hwnd;

//工作区
float screenx, screeny;
//总命中数
int _totalHitsOnServer;
//绘制玩家碰撞旗帜
bool draw_Collision_flag = false;
//绘制武器碰撞旗帜
bool draw_weapon_Collision_flag = false;
//自动扳机旗帜
bool trigger_flag = false;

//武器图标字典
const std::map<std::string, const char*> icon =
{
	{"weapon_knife_ct", "]"},
	{"weapon_knife_t", "["},
	{"weapon_deagle", "A"},
	{"weapon_elite", "B"},
	{"weapon_fiveseven", "C"},
	{"weapon_glock", "D"},
	{"weapon_hkp2000", "E"},
	{"weapon_p250", "F"},
	{"weapon_usp_silencer", "G"},
	{"weapon_tec9", "H"},
	{"weapon_cz75a", "I"},
	{"weapon_revolver", "J"},
	{"weapon_mac10", "K"},
	{"weapon_ump45", "L"},
	{"weapon_bizon", "M"},
	{"weapon_mp7", "N"},
	{"weapon_mp9", "O"},
	{"weapon_p90", "P"},
	{"weapon_galilar", "Q"},
	{"weapon_famas", "R"},
	{"weapon_m4a1", "S"},
	{"weapon_m4a1_silencer", "T"},
	{"weapon_aug", "U"},
	{"weapon_sg556", "V"},
	{"weapon_ak47", "W"},
	{"weapon_g3sg1", "X"},
	{"weapon_scar20", "Y"},
	{"weapon_awp", "Z"},
	{"weapon_ssg08", "a"},
	{"weapon_xm1014", "b"},
	{"weapon_sawedoff", "c"},
	{"weapon_mag7", "d"},
	{"weapon_nova", "e"},
	{"weapon_negev", "f"},
	{"weapon_m249", "g"},
	{"weapon_taser", "h"},
	{"weapon_flashbang", "i"},
	{"flashbang_projectile", "i"},
	{"weapon_hegrenade", "j"},
	{"hegrenade_projectile", "j"},
	{"weapon_smokegrenade", "k"},
	{"smokegrenade_projectile", "k"},
	{"weapon_molotov", "l"},
	{"molotov_projectile", "l"},
	{"weapon_decoy", "m"},
	{"decoy_projectile", "m"},
	{"weapon_incgrenade", "n"},
	{"incgrenade_projectile", "n"},
	{"weapon_c4", "o"},
};

//骨骼ID
const std::list<int> A = { 0, 4, 5, 6 };
const std::list<int> B = { 5, 8, 9, 10 };
const std::list<int> C = { 5, 13,14, 15 };
const std::list<int> D = { 0, 22, 23, 24 };
const std::list<int> E = { 0, 25, 26, 27 };
const std::list<std::list<int>> List = { A, B, C, D, E };

//读内存
template<typename Value>
Value ReadMemory(ptrdiff_t Address)
{
	Value value{};
	ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &value, sizeof(value), nullptr);
	return value;
}

//写内存
template<typename Value>
void WriteMemory(ptrdiff_t Address, Value value)
{
	WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address), &value, sizeof(value), nullptr);
}

ImVec4 draw_Collision(ptrdiff_t BaseEntity, ptrdiff_t GameSceneNode, Matrix ViewMatrix, bool draw_flag, ImColor color, float thickness)
{
	ptrdiff_t Collision = ReadMemory<ptrdiff_t>(BaseEntity + C_BaseEntity::m_pCollision);
	vector vecMins = ReadMemory<vector>(Collision + CCollisionProperty::m_vecMins);
	vector vecMaxs = ReadMemory<vector>(Collision + CCollisionProperty::m_vecMaxs);
	vector t1 = vector{ vecMins.x,vecMins.y,vecMaxs.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector t2 = vector{ vecMins.x,vecMaxs.y,vecMaxs.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector t3 = vector{ vecMaxs.x,vecMaxs.y,vecMaxs.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector t4 = vector{ vecMaxs.x,vecMins.y,vecMaxs.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector b1 = vector{ vecMins.x,vecMins.y,vecMins.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector b2 = vector{ vecMins.x,vecMaxs.y,vecMins.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector b3 = vector{ vecMaxs.x,vecMaxs.y,vecMins.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	vector b4 = vector{ vecMaxs.x,vecMins.y,vecMins.z }.Transform(ReadMemory<CTransform>(GameSceneNode + CGameSceneNode::m_nodeToWorld).ToMatrix()).WorldToScreen(ViewMatrix, screenx, screeny);
	ImVec4 collision;
	collision.x = min(min(min(t1.x, t2.x), min(t3.x, t4.x)), min(min(b1.x, b2.x), min(b3.x, b4.x)));
	collision.y = max(max(max(t1.x, t2.x), max(t3.x, t4.x)), max(max(b1.x, b2.x), max(b3.x, b4.x)));
	collision.z = min(min(min(t1.y, t2.y), min(t3.y, t4.y)), min(min(b1.y, b2.y), min(b3.y, b4.y)));
	collision.w = max(max(max(t1.y, t2.y), max(t3.y, t4.y)), max(max(b1.y, b2.y), max(b3.y, b4.y)));
	if (draw_flag)
	{
		ImGui::GetBackgroundDrawList()->AddLine({ t1.x, t1.y }, { t2.x, t2.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ t2.x, t2.y }, { t3.x, t3.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ t3.x, t3.y }, { t4.x, t4.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ t4.x, t4.y }, { t1.x, t1.y }, color, thickness);

		ImGui::GetBackgroundDrawList()->AddLine({ b1.x, b1.y }, { b2.x, b2.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b2.x, b2.y }, { b3.x, b3.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b3.x, b3.y }, { b4.x, b4.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b4.x, b4.y }, { b1.x, b1.y }, color, thickness);

		ImGui::GetBackgroundDrawList()->AddLine({ b1.x, b1.y }, { t1.x, t1.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b2.x, b2.y }, { t2.x, t2.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b3.x, b3.y }, { t3.x, t3.y }, color, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ b4.x, b4.y }, { t4.x, t4.y }, color, thickness);
	}
	return collision;
}

void draw_Collision_2D(ImVec4 collision, ImColor color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.z }, { collision.y,collision.z }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.z }, { collision.y,collision.w }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.w }, { collision.x,collision.w }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.w }, { collision.x,collision.z }, color, thickness);
}

void draw_Collision_boundingbox(ImVec4 collision, ImColor color, float thickness)
{
	float spacing = (collision.y - collision.x) * 0.2f;
	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.z }, { collision.x,collision.z + spacing }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.z }, { collision.x + spacing,collision.z }, color, thickness);

	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.z }, { collision.y - spacing,collision.z }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.z }, { collision.y,collision.z + spacing }, color, thickness);

	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.w }, { collision.y,collision.w - spacing }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.y,collision.w }, { collision.y - spacing,collision.w }, color, thickness);

	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.w }, { collision.x + spacing,collision.w }, color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine({ collision.x,collision.w }, { collision.x,collision.w - spacing }, color, thickness);
}

void draw_name(ImFont* font, const char* name, ImVec4 collision, ImColor color)
{
	ImGui::PushFont(font);
	ImVec2 namesize = ImGui::CalcTextSize(name);
	ImGui::GetBackgroundDrawList()->AddText({ (collision.x + collision.y - namesize.x) / 2,collision.z - namesize.y }, color, name);
	ImGui::PopFont();
}

void draw_weaponicon(std::string weaponname, ImVec4 collision, ImColor color)
{
	auto weaponicon = icon.find(weaponname)->second;
	if (weaponicon)
	{
		ImVec2 iconsize = ImGui::CalcTextSize(weaponicon);
		ImGui::GetBackgroundDrawList()->AddText({ (collision.x + collision.y - iconsize.x) / 2,collision.w }, color, weaponicon);
	}
}

void draw_projectileicon(std::string projectilename, vector Origin, ImColor color)
{
	auto projectileicon = icon.find(projectilename)->second;
	if (projectileicon)
	{
		ImVec2 iconsize = ImGui::CalcTextSize(projectileicon);
		ImGui::GetBackgroundDrawList()->AddText({ Origin.x - iconsize.x / 2,Origin.y - iconsize.y / 2 }, color, projectileicon);
	}
}

void draw_timer(vector Origin, float radius, float ratio, float thickness)
{
	ImGui::GetForegroundDrawList()->PathArcTo({ Origin.x,Origin.y }, radius, 4.71f - 6.28f * ratio, 4.71f);
	ImGui::GetForegroundDrawList()->PathStroke(ImColor(1.f, ratio, ratio), 0, thickness);
}

//Cheats
void Cheats(ImFont* font)
{
	ptrdiff_t EntityList = ReadMemory<ptrdiff_t>(ModuleAddress + client_dll::dwEntityList);
	if (!EntityList)return;

	ptrdiff_t LocalPlayerController = ReadMemory<ptrdiff_t>(ModuleAddress + client_dll::dwLocalPlayerController);
	if (!LocalPlayerController)return;

	ptrdiff_t LocalPlayerPawn = ReadMemory<ptrdiff_t>(ModuleAddress + client_dll::dwLocalPlayerPawn);
	if (!LocalPlayerPawn)return;

	Matrix ViewMatrix = ReadMemory<Matrix>(ModuleAddress + client_dll::dwViewMatrix);

	int LocalTeamNum = ReadMemory<int>(LocalPlayerPawn + C_BaseEntity::m_iTeamNum);

	for (int i = 64; i < 1024; i++)
	{
		ptrdiff_t A = ReadMemory<ptrdiff_t>(EntityList + 8LL * ((i & 0x7FFF) >> 9) + 16);
		if (!A)continue;

		ptrdiff_t B = ReadMemory<ptrdiff_t>(A + 120LL * (i & 0x1FF));
		if (!B)continue;

		ptrdiff_t GameSceneNode = ReadMemory<ptrdiff_t>(B + C_BaseEntity::m_pGameSceneNode);

		vector vecAbsOrigin = ReadMemory<vector>(GameSceneNode + CGameSceneNode::m_vecAbsOrigin).WorldToScreen(ViewMatrix, screenx, screeny);

		if (!(vecAbsOrigin.z < 0.01f))
		{
			char entityname[128]{};
			ptrdiff_t Entity = ReadMemory<ptrdiff_t>(B + CEntityInstance::m_pEntity);
			ptrdiff_t designerName = ReadMemory<ptrdiff_t>(Entity + CEntityIdentity::m_designerName);
			ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(designerName), entityname, sizeof(entityname), nullptr);
			if (strstr(entityname, "weapon_"))
			{
				if (ReadMemory<int>(B + C_CSWeaponBase::m_iState) == 0)
				{
					ptrdiff_t C = ReadMemory<ptrdiff_t>(B + 0x360);
					ptrdiff_t D = ReadMemory<ptrdiff_t>(C + CCSWeaponBaseVData::m_szName);
					ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(D), entityname, sizeof(entityname), nullptr);
					ImVec4 collision = draw_Collision(B, GameSceneNode, ViewMatrix, draw_weapon_Collision_flag, ImColor(255, 255, 255), 1);
					draw_Collision_2D(collision, ImColor(255, 255, 255), 1);
					draw_Collision_boundingbox(collision, ImColor(255, 255, 255), 2);
					draw_name(font, entityname, collision, ImColor(255, 255, 255));
					draw_weaponicon(entityname, collision, ImColor(255, 255, 255));
				}
			}
			else if (strstr(entityname, "_projectile"))
			{
				draw_projectileicon(entityname, vecAbsOrigin, ImColor(255, 0, 0));
				if (strstr(entityname, "smokegrenade_projectile"))
				{
					int SmokeEffectTickBegin = ReadMemory<int>(B + C_SmokeGrenadeProjectile::m_nSmokeEffectTickBegin);
					if (SmokeEffectTickBegin)
					{
						ptrdiff_t GlobalVars = ReadMemory<ptrdiff_t>(ModuleAddress + client_dll::dwGlobalVars);
						float ratio = (SmokeEffectTickBegin * 0.015625f + 22 - ReadMemory<float>(GlobalVars + 0x2C)) / 22;
						draw_timer(vecAbsOrigin, 10, ratio, 2);
					}
				}
			}
		}
	}

	float _aim_distance = 5201314;
	vector _aim_target;

	for (int i = 1; i < 64; i++)
	{
		ptrdiff_t A = ReadMemory<ptrdiff_t>(EntityList + 8LL * ((i & 0x7FFF) >> 9) + 16);
		if (!A)continue;

		ptrdiff_t B = ReadMemory<ptrdiff_t>(A + 120LL * (i & 0x1FF));
		if (!B)continue;

		ptrdiff_t C = ReadMemory<ptrdiff_t>(B + CCSPlayerController::m_hPlayerPawn);
		if (!C)continue;

		ptrdiff_t D = ReadMemory<ptrdiff_t>(EntityList + 8LL * ((C & 0x7FFF) >> 9) + 16);

		ptrdiff_t E = ReadMemory<ptrdiff_t>(D + 120LL * (C & 0x1FF));

		bool IsLocalPlayerController = ReadMemory<bool>(B + CBasePlayerController::m_bIsLocalPlayerController);

		bool PawnIsAlive = ReadMemory<bool>(B + CCSPlayerController::m_bPawnIsAlive);

		bool IsLocalTeamNum = ReadMemory<int>(E + C_BaseEntity::m_iTeamNum) == LocalTeamNum;

		vector OldOrigin = ReadMemory<vector>(E + C_BasePlayerPawn::m_vOldOrigin).WorldToScreen(ViewMatrix, screenx, screeny);

		if (!(OldOrigin.z < 0.01f))
		{
			if (!IsLocalTeamNum)
			{
				if (PawnIsAlive)
				{
					if (!IsLocalPlayerController)
					{
						int MaxHealth = ReadMemory<int>(E + C_BaseEntity::m_iMaxHealth);

						int Health = ReadMemory<int>(E + C_BaseEntity::m_iHealth);

						ptrdiff_t GameSceneNode = ReadMemory<ptrdiff_t>(E + C_BaseEntity::m_pGameSceneNode);

						ImVec4 collision = draw_Collision(E, GameSceneNode, ViewMatrix, draw_Collision_flag, ImColor(255, 0, 0), 1);

						draw_Collision_boundingbox(collision, ImColor(255, 0, 0), 1);

						char playername[128]{};
						ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(B + CBasePlayerController::m_iszPlayerName), playername, sizeof(playername), nullptr);
						draw_name(font, playername, collision, ImColor(255, 255, 255));

						char weaponname[128]{};
						ptrdiff_t ClippingWeapon = ReadMemory<ptrdiff_t>(E + C_CSPlayerPawnBase::m_pClippingWeapon);
						ptrdiff_t F = ReadMemory<ptrdiff_t>(ClippingWeapon + 0x360);
						ptrdiff_t G = ReadMemory<ptrdiff_t>(F + CCSWeaponBaseVData::m_szName);
						ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(G), weaponname, sizeof(weaponname), nullptr);
						draw_weaponicon(weaponname, collision, ImColor(255, 255, 255));

						ptrdiff_t Bone = ReadMemory<ptrdiff_t>(GameSceneNode + 0x1E0);

						for (const std::list<int> list : List)
						{
							vector cache;
							for (const LONGLONG index : list)
							{
								vector bone = ReadMemory<vector>(Bone + index * 32).WorldToScreen(ViewMatrix, screenx, screeny);
								if (cache.x)ImGui::GetBackgroundDrawList()->AddLine({ cache.x,cache.y }, { bone.x,bone.y }, ImColor(255, 0, 0), 1);
								cache = bone;
							}
						}

						vector aim_target = ReadMemory<vector>(Bone + 6LL * 32);
						vector aim_target_ = aim_target.WorldToScreen(ViewMatrix, screenx, screeny);
						float aim_distance = hypot(screenx / 2 - aim_target_.x, screeny / 2 - aim_target_.y);
						if (aim_distance < _aim_distance)
						{
							_aim_distance = aim_distance;
							_aim_target = aim_target;
						}
					}
				}
			}
		}
	}
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (_aim_distance < 5201314)
		{
			vector vecLastClipCameraPos = ReadMemory<vector>(LocalPlayerPawn + C_CSPlayerPawnBase::m_vecLastClipCameraPos);
			vector angEyeAngles = ReadMemory<vector>(LocalPlayerPawn + C_CSPlayerPawnBase::m_angEyeAngles);
			vector Unknown = { _aim_target.x - vecLastClipCameraPos.x,_aim_target.y - vecLastClipCameraPos.y,_aim_target.z - vecLastClipCameraPos.z };
			float pitch = atan2(-Unknown.z, hypot(Unknown.x, Unknown.y)) * 180 / 3.14f - angEyeAngles.x;
			float yaw = atan2(Unknown.y, Unknown.x) * 180 / 3.14f - angEyeAngles.y;
			if (hypot(pitch, yaw) < 20)
			{
				if (ReadMemory<int>(LocalPlayerPawn + C_CSPlayerPawnBase::m_iShotsFired) > 1)
				{
					RCS aimPunchCache = ReadMemory<RCS>(LocalPlayerPawn + C_CSPlayerPawn::m_aimPunchCache);
					vector aimPunch = ReadMemory<vector>(aimPunchCache.B + (aimPunchCache.A - 1) * sizeof(vector));
					WriteMemory<vector>(ModuleAddress + client_dll::dwViewAngles, { pitch + angEyeAngles.x - aimPunch.x * 2.f,yaw + angEyeAngles.y - aimPunch.y * 2.f });
				}
			}
		}
	}
	if (trigger_flag)
	{
		int IDEntIndex = ReadMemory<int>(LocalPlayerPawn + C_CSPlayerPawnBase::m_iIDEntIndex);
		if (IDEntIndex != -1)
		{
			ptrdiff_t A = ReadMemory<ptrdiff_t>(EntityList + 8LL * ((IDEntIndex & 0x7FFF) >> 9) + 16);
			ptrdiff_t B = ReadMemory<ptrdiff_t>(A + 120LL * (IDEntIndex & 0x1FF));
			if (ReadMemory<int>(B + C_BaseEntity::m_iHealth))
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
		}
	}
	if (ReadMemory<float>(LocalPlayerPawn + C_CSPlayerPawnBase::m_flFlashDuration))
	{
		WriteMemory<float>(LocalPlayerPawn + C_CSPlayerPawnBase::m_flFlashDuration, 0);
	}
	ptrdiff_t BulletServices = ReadMemory<ptrdiff_t>(LocalPlayerPawn + C_CSPlayerPawn::m_pBulletServices);
	int totalHitsOnServer = ReadMemory<int>(BulletServices + CCSPlayer_BulletServices::m_totalHitsOnServer);
	if (totalHitsOnServer != _totalHitsOnServer)
	{
		PlaySoundW(L"hit.wav", 0, SND_ASYNC);
		_totalHitsOnServer = totalHitsOnServer;
	}
	if (GetAsyncKeyState(VK_NUMPAD1))draw_Collision_flag = true;
	if (GetAsyncKeyState(VK_NUMPAD2))draw_Collision_flag = false;
	if (GetAsyncKeyState(VK_NUMPAD3))draw_weapon_Collision_flag = true;
	if (GetAsyncKeyState(VK_NUMPAD4))draw_weapon_Collision_flag = false;
	if (GetAsyncKeyState(VK_NUMPAD5))trigger_flag = true;
	if (GetAsyncKeyState(VK_NUMPAD6))trigger_flag = false;
}

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
	PROCESSENTRY32 A{};
	A.dwSize = sizeof(PROCESSENTRY32);
	HANDLE B = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(B, &A))
	{
		do
		{
			if (!wcscmp(ProcessName, A.szExeFile))
			{
				ProcessID = A.th32ProcessID;
			}
		} while (Process32Next(B, &A));
		CloseHandle(B);
	}
	MODULEENTRY32 C{};
	C.dwSize = sizeof(MODULEENTRY32);
	HANDLE D = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
	if (Module32First(D, &C))
	{
		do
		{
			if (!wcscmp(ModuleName, C.szModule))
			{
				ModuleAddress = reinterpret_cast<ptrdiff_t>(C.hModule);
			}
		} while (Module32Next(D, &C));
		CloseHandle(D);
	}
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessID);
	Hwnd = FindWindowA(0, "Counter-Strike 2");
	std::cout << "[PID] ---> " << ProcessID << std::endl;
	std::cout << "[MODULE] ---> " << ModuleAddress << std::endl;
	std::cout << "[HPROCESS] ---> " << hProcess << std::endl;
	std::cout << "[HWND] ---> " << Hwnd << std::endl;

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT, wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("font.ttf", 15.f);
	ImFont* font2 = io.Fonts->AddFontFromFileTTF("font.otf", 10.f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != nullptr);

	// Our state

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		{
			POINT point{};
			RECT rect{};
			ClientToScreen(Hwnd, &point);
			GetClientRect(Hwnd, &rect);
			screenx = rect.right;
			screeny = rect.bottom;
			SetWindowPos(hwnd, 0, point.x, point.y, screenx, screeny, 0);
		}

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//hack
		Cheats(font2);

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = {  };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}