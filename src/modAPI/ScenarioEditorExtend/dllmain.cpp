// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Global.h"
#include "TerrainTest.h"
#include "ScenarioCustomization.h"
#include "ScenarioFloraGroundCoverLock.h"
#include <Spore\UI\ScrollFrameVertical.h>

using namespace App;
using namespace ArgScript;
using namespace Palettes;
using namespace Simulator;
using namespace UI;
using namespace UTFWin;

int g_ScenarioCustomizationCategoryIndex = -1;
ScenarioCustomizationPtr g_pWinProc = nullptr;
ScenarioFloraGroundCoverLockPtr g_pFloraGroundCoverLock = nullptr;


#ifdef _DEBUG
class VisualEffectTestCheat
	: public ICommand
{
public:
	VisualEffectTestCheat() {}
	~VisualEffectTestCheat() {}

	uint32_t ParseUniversal(const char* pArgument)
	{
		bool bIsInteger = true;
		if (pArgument[0] == '0' && pArgument[1] == 'x')
			return mpFormatParser->ParseUInt(pArgument);
		for (const char* pChar = pArgument; *pChar != '\0'; ++pChar)
			if (*pChar < '0' || *pChar > '9')
			{
				bIsInteger = false;
				break;
			}
		return bIsInteger
			? mpFormatParser->ParseInt(pArgument)
			: id(pArgument);
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

#pragma region Switch Palette Category
static inline void SwitchPaletteCategory()
{
	if (g_pWinProc)
		g_pWinProc->SwitchPaletteCategory();
}

member_detour(PaletteUI_SetActiveCategory, PaletteUI, void(int))
{
	void detoured(int categoryIndex)
	{
		if (g_pWinProc && g_ScenarioCustomizationCategoryIndex == -1)
			for (int categoryIndexCheck = 0;
				categoryIndexCheck < (int)this->mCategories.size();
				++categoryIndexCheck
			)
			{
				IWindow* pPaletteCategoryWin = this->mCategories[categoryIndexCheck]->
					mpLayout->FindWindowByID(CONTROL_ID_PALETTE);
				if (pPaletteCategoryWin == g_pWinProc->GetCategoryWindow())
				{
					g_ScenarioCustomizationCategoryIndex = categoryIndexCheck;
					break;
				}
			}

		SwitchPaletteCategory();
		if (g_pWinProc && categoryIndex == g_ScenarioCustomizationCategoryIndex)
			g_pWinProc->Init(false);
		original_function(this, categoryIndex);
	}
};

member_detour(cScenarioEditModeDisplayStrategy_SetMode, cScenarioEditModeDisplayStrategy, void(int))
{
	void detoured(int mode)
	{
		SwitchPaletteCategory();
		original_function(this, mode);
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
