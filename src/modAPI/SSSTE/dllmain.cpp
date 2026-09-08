// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Global.h"
#include "ScenarioCustomization.h"
#include "ScenarioFloraGroundCoverLock.h"
#include <Spore\UI\ScrollFrameVertical.h>

using namespace App;
using namespace ArgScript;
using namespace Palettes;
using namespace Simulator;
using namespace UI;
using namespace UTFWin;

int g_nScenarioCustomizationCategoryIndex = -1;
ScenarioCustomizationPtr g_pWinProc = nullptr;
ScenarioFloraGroundCoverLockPtr g_pFloraGroundCoverLock = nullptr;


#ifdef _DEBUG
class VisualEffectTestCheat
	: public ICommand
{
public:
	VisualEffectTestCheat() {}
	~VisualEffectTestCheat() {}

	uint32_t ParseUniversal(const char* szArgument)
	{
		bool bIsInteger = true;
		if (szArgument[0] == '0' && szArgument[1] == 'x')
			return mpFormatParser->ParseUInt(szArgument);
		for (const char* ch = szArgument; *ch != '\0'; ++ch)
			if (*ch < '0' || *ch > '9')
			{
				bIsInteger = false;
				break;
			}
		return bIsInteger
			? mpFormatParser->ParseInt(szArgument)
			: id(szArgument);
	}

	void ParseLine(const Line& line) override
	{
		if (!IsScenarioMode() ||
			ScenarioMode.GetMode() != cScenarioMode::Mode::EditMode ||
			line.GetArgumentsCount() - 1 < 1
		)
			return;
		ResourceKey visualEffectId = instance_id(ParseUniversal(line.GetArguments(1)[0]));
		CALL(
			GetAddress(SSSTE::cScenarioTerraformMode, SetVisualStyle),
			void,
			Args(cScenarioTerraformMode*, ResourceKey*),
			Args(ScenarioMode.GetTerraformMode(), &visualEffectId)
		);
	}

	const char* GetDescription(DescriptionMode mode) const override { return "meow"; }
};

void Initialize()
{
	CheatManager.AddCheat("sssteSetVisualStyle", new VisualEffectTestCheat());
}
#else
void Initialize() {}
#endif

void Dispose()
{
	g_pWinProc = nullptr;
	g_pFloraGroundCoverLock = nullptr;
}

#pragma region Detours
member_detour(
	PaletteCategoryUI_Load,
	PaletteCategoryUI,
	void(PaletteCategory*, IWindow*, PaletteInfo*)
)
{
	void detoured(PaletteCategory* pCategory, IWindow* pWindow, PaletteInfo* pInfo)
	{
		original_function(this, pCategory, pWindow, pInfo);

		if (!IsScenarioMode())
			return;

		IWindow* pPaletteCategoryWin = mpMainFrame->FindWindowByID(CONTROL_ID_PALETTE);
		IWindow* pPanelWin = pPaletteCategoryWin
			? pPaletteCategoryWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL)
			: nullptr;
		IWindowPtr pPanelItemsWin = pPanelWin
			? pPanelWin->FindWindowByID(CONTROL_ID_CUSTOMIZATION_PANEL_ITEMS)
			: nullptr;
		if (!pPanelItemsWin)
			return;

		if (IWindow* pModAPIHideWin = pPaletteCategoryWin->FindWindowByID(CONTROL_ID_MODAPI_HIDE))
			pModAPIHideWin->SetVisible(false);
		if (IWindow* pModAPIShowWin = pPaletteCategoryWin->FindWindowByID(CONTROL_ID_MODAPI_SHOW))
			pModAPIShowWin->SetVisible(true);

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
		//pScrollFrameVerticalWin->SetFlag(kWinFlagClip, true);
		pScrollFrameVerticalWin->SetArea(panelItemsWinArea);
		ScrollFrameVertical::Update(pScrollFrameVerticalWin);

		if (!g_pFloraGroundCoverLock)
			g_pFloraGroundCoverLock = new ScenarioFloraGroundCoverLock(nullptr);

		g_pWinProc = new ScenarioCustomization(
			pPaletteCategoryWin,
			pScrollFrameVerticalWin,
			pPanelItemsWin.get()
		);
	}
};

member_detour(
	PaletteCategory_ReadProp,
	PaletteCategory,
	bool(const ResourceKey&, uint32_t)
)
{
	// This one is just for a fancy disabled button to be shown if the .dll of the mod isn't loaded.
	bool detoured(const ResourceKey& name, uint32_t defaultLayoutID)
	{
		if (original_function(this, name, defaultLayoutID))
		{
			PropertyListPtr pCategoryPropList;
			if (PropManager.GetPropertyList(name.instanceID, name.groupID, pCategoryPropList))
			{
				ResourceKey* pCategoryIconsModAPI;
				size_t nCategoryIconsModAPICount;
				if (Property::GetArrayKey(
						pCategoryPropList.get(),
						PROPERTY_ID_PALETTE_CATEGORY_ICON_LIST_MODAPI,
						nCategoryIconsModAPICount,
						pCategoryIconsModAPI
					) && nCategoryIconsModAPICount == 9
				)
					mCategoryIconList = pCategoryIconsModAPI;
			}
			return true;
		}
		return false;
	}
};

#pragma region Switch Palette Category
static inline void SwitchPaletteCategory()
{
	if (g_pWinProc)
		g_pWinProc->SwitchPaletteCategory();
}

member_detour(PaletteUI_SetActiveCategory, PaletteUI, void(int))
{
	void detoured(int nCategoryIndex)
	{
		if (g_pWinProc && g_nScenarioCustomizationCategoryIndex == -1)
			for (int nCategoryIndexCheck = 0;
				nCategoryIndexCheck < (int)mCategories.size();
				++nCategoryIndexCheck
			)
			{
				IWindow* pPaletteCategoryWin = mCategories[nCategoryIndexCheck]->
					mpMainFrame->FindWindowByID(CONTROL_ID_PALETTE);
				if (pPaletteCategoryWin == g_pWinProc->GetCategoryWindow())
				{
					g_nScenarioCustomizationCategoryIndex = nCategoryIndexCheck;
					break;
				}
			}

		SwitchPaletteCategory();
		if (g_pWinProc && nCategoryIndex == g_nScenarioCustomizationCategoryIndex)
			g_pWinProc->Init(false);
		original_function(this, nCategoryIndex);
	}
};

member_detour(cScenarioEditModeDisplayStrategy_SetMode, cScenarioEditModeDisplayStrategy, void(int))
{
	void detoured(int nMode)
	{
		SwitchPaletteCategory();
		original_function(this, nMode);
	}
};
#pragma endregion

#pragma region Undo & Redo
virtual_detour(
	cScenarioTerraformHistoryEntry_Undo,
	cScenarioTerraformHistoryEntry,
	IScenarioEditHistoryEntry,
	void()
)
{
	void detoured()
	{
		original_function(this);
		SwitchPaletteCategory();
		if (g_pWinProc)
			g_pWinProc->Init(false);
	}
};

virtual_detour(
	cScenarioTerraformHistoryEntry_Redo,
	cScenarioTerraformHistoryEntry,
	IScenarioEditHistoryEntry,
	void()
)
{
	void detoured()
	{
		original_function(this);
		if (g_pWinProc)
			g_pWinProc->Init(false);
	}
};
#pragma endregion

member_detour(
	ScenarioEditModeSculptFloraUI_UpdateFloraCategoryUI,
	ScenarioEditModeSculptFloraUI,
	void()
)
{
	void detoured()
	{
		original_function(this);
		IWindow* pFloraGroundCoverWin = nullptr;
		if (UILayout* pLayout = (UILayout*)field(this, 0x168))
			pFloraGroundCoverWin = pLayout->FindWindowByID(CONTROL_ID_PALETTE_FLORA_GROUND_COVER);

		if (g_pFloraGroundCoverLock &&
			g_pFloraGroundCoverLock->GetInitializedWindow() == pFloraGroundCoverWin
		)
			g_pFloraGroundCoverLock->Update();
		else
			g_pFloraGroundCoverLock = new ScenarioFloraGroundCoverLock(
				this,
				g_pFloraGroundCoverLock->IsLocked()
			);
	}
};

member_detour(
	cScenarioTerraformMode_SetVisualStyle,
	cScenarioTerraformMode,
	void(ResourceKey*)
)
{
	void detoured(ResourceKey* pKey)
	{
		original_function(this, pKey);
		cScenarioMode& rScenarioMode = ScenarioMode;
		if (g_pWinProc && rScenarioMode.GetMode() == cScenarioMode::Mode::EditMode)
			g_pWinProc->InitEffects(true);
	}
};


void AttachDetours()
{
	PaletteCategoryUI_Load::attach(GetAddress(PaletteCategoryUI, Load));
	PaletteCategory_ReadProp::attach(GetAddress(PaletteCategory, ReadProp));
	PaletteUI_SetActiveCategory::attach(GetAddress(PaletteUI, SetActiveCategory));
	cScenarioEditModeDisplayStrategy_SetMode::attach(
		GetAddress(SSSTE::cScenarioEditModeDisplayStrategy, SetMode)
	);
	cScenarioTerraformHistoryEntry_Undo::attach(
		GetAddress(SSSTE::cScenarioTerraformHistoryEntry, Undo)
	);
	cScenarioTerraformHistoryEntry_Redo::attach(
		GetAddress(SSSTE::cScenarioTerraformHistoryEntry, Redo)
	);
	ScenarioEditModeSculptFloraUI_UpdateFloraCategoryUI::attach(
		GetAddress(SSSTE::ScenarioEditModeSculptFloraUI, UpdateFloraCategoryUI)
	);
	cScenarioTerraformMode_SetVisualStyle::attach(
		GetAddress(SSSTE::cScenarioTerraformMode, SetVisualStyle)
	);
}


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
