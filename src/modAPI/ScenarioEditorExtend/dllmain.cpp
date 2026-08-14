// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Global.h"
#include "TerrainTest.h"
#include "ScenarioCustomization.h"
#include <Spore\UI\ScrollFrameVertical.h>

//using namespace Terrain;
using namespace App;
using namespace Palettes;
using namespace UI;
using namespace UTFWin;

ScenarioCustomizationPtr g_pWinProc = nullptr;

void Initialize()
{
	// do we still need this?
	//CheatManager.AddCheat("terst", new TerrainTest());
}

void Dispose()
{
	g_pWinProc = nullptr;
}

class ScenarioEditorUI {};

member_detour(
	PaletteCategoryUI_Load,
	PaletteCategoryUI,
	void(PaletteCategory*, IWindow*, PaletteInfo*)
)
{
	void detoured(PaletteCategory* pCategory, IWindow* pWindow, PaletteInfo* pInfo)
	{
		original_function(this, pCategory, pWindow, pInfo);

		IWindow* pPaletteCategoryWin = mpLayout->FindWindowByID(CONTROL_ID_PALETTE);
		IWindow* pPanelWin = pPaletteCategoryWin
			? pPaletteCategoryWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL)
			: nullptr;
		IWindowPtr pPanelItemsWin = pPanelWin
			? pPanelWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS)
			: nullptr;
		if (!pPanelItemsWin)
			return;

		Math::Rectangle panelItemsWinArea = pPanelItemsWin->GetArea();
		pPanelWin->RemoveWindow(pPanelItemsWin.get());
		IWindow* pScrollFrameVerticalWin = ScrollFrameVertical::Create(
			ScrollFrameVertical::GENERIC_LAYOUT,
			pPanelItemsWin
		);
		pPanelWin->AddWindow(pScrollFrameVerticalWin);
		pScrollFrameVerticalWin->SetFillColor(Math::Color(0));
		pScrollFrameVerticalWin->AddWinProc(
			new SimpleLayout(kAnchorBottom | kAnchorLeft | kAnchorRight | kAnchorTop)
		);
		pScrollFrameVerticalWin->SetEnabled(true);
		pScrollFrameVerticalWin->SetFlag(kWinFlagIgnoreMouse, true);
		pScrollFrameVerticalWin->SetFlag(kWinFlagClip, true);
		pScrollFrameVerticalWin->SetArea(panelItemsWinArea);
		ScrollFrameVertical::Update(pScrollFrameVerticalWin);

		g_pWinProc = new ScenarioCustomization(
			pPaletteCategoryWin,
			pScrollFrameVerticalWin,
			pPanelItemsWin.get()
		);

		//TODO: add other buttons here
		
		if (IWindow* pPaletteCategoryTextureBtn = mpLayout->FindWindowByID(
			CONTROL_ID_PALETTE_BTN_TEXTURE
		))
			pPaletteCategoryTextureBtn->AddWinProc(g_pWinProc.get());

		if (IWindow* pPanelCloseBtn = pPanelWin->FindWindowByID(
			CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE
		))
			pPanelCloseBtn->AddWinProc(g_pWinProc.get());

		if (IWindow* pPanelSearchbox = pPanelWin->FindWindowByID(
			CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX
		))
			pPanelSearchbox->AddWinProc(g_pWinProc.get());

		if (IWindow* pTexturePropertySelect = mpLayout->FindWindowByID(
			CONTROL_ID_PALETTE_PROPERTIES_TEXTURE
		))
			for (IWindow* pChildWin : pTexturePropertySelect->children())
				pTexturePropertySelect->AddWinProc(g_pWinProc.get());

		pPanelWin->AddWinProc(g_pWinProc.get());
	}
};

static inline void SwitchPaletteCategory()
{
	if (g_pWinProc)
		g_pWinProc->SwitchPaletteCategory();
}

member_detour(PaletteUI_SetActiveCategory, PaletteUI, void(int))
{
	void detoured(int categoryIndex)
	{
		SwitchPaletteCategory();
		original_function(this, categoryIndex);
	}
};

member_detour(ScenarioEditorUI_SetMode, ScenarioEditorUI, void(int))
{
	void detoured(int mode)
	{
		SwitchPaletteCategory();
		original_function(this, mode);
	}
};

// TODO: add detours for undo/redo history entriy

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
	PaletteUI_SetActiveCategory::attach(GetAddress(PaletteUI, SetActiveCategory));
	ScenarioEditorUI_SetMode::attach(GetAddress(SSSTE::ScenarioEditorUI, SetMode));
	//cTerrainStateMgr_UpdateFromDefinition::attach(Address(0xfbc100));
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
