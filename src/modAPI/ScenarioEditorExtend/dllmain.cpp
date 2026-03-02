// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "TerrainTest.h"
#include "ScenarioCustomization.h"
#include "Spore\UTFWin\WinGrid.h"

using namespace Terrain;
using namespace App;
using namespace Palettes;

ScenarioCustomizationPtr winProc = nullptr;

void Initialize()
{
	CheatManager.AddCheat("terst", new TerrainTest());
}

void Dispose()
{
	// This method is called when the game is closing
}

member_detour(PaletteCategoryUI_Load, PaletteCategoryUI, void(PaletteCategory*, UTFWin::IWindow*, PaletteInfo*))
{
	void detoured(PaletteCategory* pCategory, UTFWin::IWindow* pWindow, PaletteInfo* pInfo)
	{
		original_function(this, pCategory, pWindow, pInfo);
		IWindowPtr items = this->mpLayout->FindWindowByID(id("PlanetCustomizationItems"));
		if (items == nullptr) return; // fix for a copy on (0,0)
		WinGridPtr winTest = new UTFWin::WinGrid();
		if (winTest == nullptr) return; // fix for a copy on (0,0)
		if (winTest != nullptr)
		{
			winTest->SetFillColor(items->GetFillColor());
			winTest->SetArea(items->GetArea());
			winTest->SetControlID(id("PlanetCustomizationItems"));
			winTest->AddWinProc(new UTFWin::SimpleLayout(UTFWin::kAnchorBottom | UTFWin::kAnchorLeft | UTFWin::kAnchorRight | UTFWin::kAnchorTop));
			winTest->SetEnabled(true);
			winTest->SetFlag(UTFWin::kWinFlagIgnoreMouse, true);
			winTest->SetFlag(UTFWin::kWinFlagClip, true);
			winTest->SetDefaultRowHeight(120);
			winTest->SetDefaultColumnWidth(120);
			winTest->SetCellColors(0,0,0, items->GetFillColor());
		}
		IWindowPtr button = this->mpLayout->FindWindowByID(id("PlanetCustomizationTextureButton"));
		IWindowPtr panel = this->mpLayout->FindWindowByID(id("PlanetCustomizationPanel"));
		panel->RemoveWindow(items.get());
		panel->AddWindow(winTest.get());
		if (winProc == nullptr) winProc = new ScenarioCustomization();
		//winProc->InitItems(items.get());
		IScrollbarDrawablePtr scrollbar = object_cast<IScrollbarDrawable>(this->mpLayout->FindWindowByID(id("PlanetCustomizationScrollbar")));
		winTest->SetScrollBarDrawableVertical(scrollbar.get());
		winProc->InitItems(winTest.get());
		if (button != nullptr) button->AddWinProc(winProc.get());
		if (panel != nullptr) panel->AddWinProc(winProc.get());
		//if (items != nullptr) items->AddWinProc(winProc.get());
		if (winTest != nullptr) winTest->AddWinProc(winProc.get());
	}
};

//member_detour(cTerrainStateMgr_UpdateFromDefinition, cTerrainStateMgr, void(PropertyList*))
//{
//	void detoured(PropertyList* propList)
//	{
//		if (propList != nullptr)
//		{
//			ResourceKey terrainThemeWaterFoam;
//			if (Property::GetKey(propList, id("TerrainThemeWaterFoam"), terrainThemeWaterFoam))
//				this->mTextures.mpWaterFoamCutMap = TextureManager.GetTexture(terrainThemeWaterFoam);
//		}
//		return original_function(this, propList);
//	}
//};

void AttachDetours()
{
	PaletteCategoryUI_Load::attach(GetAddress(PaletteCategoryUI, Load));
	//cTerrainStateMgr_UpdateFromDefinition::attach(Address(0xfbc100));
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

