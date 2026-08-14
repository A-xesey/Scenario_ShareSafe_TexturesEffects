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

member_detour(
	PaletteCategoryUI_Load,
	PaletteCategoryUI,
	void(PaletteCategory*, IWindow*, PaletteInfo*)
)
{
	void detoured(PaletteCategory* pCategory, IWindow* pWindow, PaletteInfo* pInfo)
	{
		
		original_function(this, pCategory, pWindow, pInfo);

		IWindowPtr pPaletteCategoryWin = mpLayout->FindWindowByID(CONTROL_ID_PALETTE);
		IWindowPtr pPanelWin = pPaletteCategoryWin
			? pPaletteCategoryWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL)
			: nullptr;
		IWindowPtr pPanelItemsWin = pPanelWin
			? pPanelWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS)
			: nullptr;
		if (!pPanelItemsWin)
			return;

		Math::Rectangle panelItemsWinArea = pPanelItemsWin->GetArea();
		pPanelWin->RemoveWindow(pPanelItemsWin.get());
		IWindowPtr pScrollFrameVerticalWin = ScrollFrameVertical::Create(
			ScrollFrameVertical::GENERIC_LAYOUT,
			pPanelItemsWin
		);
		pPanelWin->AddWindow(pScrollFrameVerticalWin.get());
		pScrollFrameVerticalWin->SetFillColor(Math::Color(0));
		pScrollFrameVerticalWin->AddWinProc(
			new SimpleLayout(kAnchorBottom | kAnchorLeft | kAnchorRight | kAnchorTop)
		);
		pScrollFrameVerticalWin->SetEnabled(true);
		pScrollFrameVerticalWin->SetFlag(kWinFlagIgnoreMouse, true);
		pScrollFrameVerticalWin->SetFlag(kWinFlagClip, true);
		pScrollFrameVerticalWin->SetArea(panelItemsWinArea);
		ScrollFrameVertical::Update(pScrollFrameVerticalWin.get());

		g_pWinProc = new ScenarioCustomization(
			pPaletteCategoryWin,
			pScrollFrameVerticalWin,
			pPanelItemsWin
		);

		//TODO: add other buttons here
		
		if (IWindowPtr pPaletteCategoryTextureBtn = mpLayout->FindWindowByID(
			CONTROL_ID_PALETTE_BTN_TEXTURE
		))
			pPaletteCategoryTextureBtn->AddWinProc(g_pWinProc.get());

		if (IWindowPtr pPanelCloseBtn = pPanelWin->FindWindowByID(
			CONTROL_ID_CUSTOMIZATION_PANEL_CLOSE
		))
			pPanelCloseBtn->AddWinProc(g_pWinProc.get());

		if (IWindowPtr pPanelSearchbox = pPanelWin->FindWindowByID(
			CONTROL_ID_CUSTOMIZATION_PANEL_SEARCHBOX
		))
			pPanelSearchbox->AddWinProc(g_pWinProc.get());

		if (IWindowPtr pTexturePropertySelect = mpLayout->FindWindowByID(
			CONTROL_ID_PALETTE_PROPERTIES_TEXTURE
		))
			for (IWindow* pChildWin : pTexturePropertySelect->children())
				pTexturePropertySelect->AddWinProc(g_pWinProc.get());

		pPanelWin->AddWinProc(g_pWinProc.get());
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

