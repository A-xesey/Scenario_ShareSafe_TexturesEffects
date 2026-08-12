#pragma once

#include <Spore\BasicIncludes.h>

#define ScenarioCustomizationItemPtr intrusive_ptr<ScenarioCustomizationItem>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class ScenarioCustomizationItem 
	: public UILayout
{
protected:
	ResourceKey mCustomizationKey;
	ResourceKey mThumbnailKey;
	LocalizedString mName;
	bool mbIsSelected;
	IWindowPtr mpButton;
	SporeTooltipWinProcPtr mpTooltipWinProc;
public:
	static const uint32_t TYPE = id("ScenarioCustomizationItem");
	
	ScenarioCustomizationItem();
	~ScenarioCustomizationItem();

	virtual void SetCustomizationAndImage(const App::PropertyList* pPropList, ResourceKey thumbnailKey, IWinProc* pHandler);
	virtual void SetSelection(bool bIsSelected);
	inline ResourceKey* ScenarioCustomizationItem::GetCustomization() { return &mCustomizationKey; }
	inline ResourceKey* ScenarioCustomizationItem::GetThumbnail() { return &mThumbnailKey; }
	inline LocalizedString* ScenarioCustomizationItem::GetName() { return &mName; }
	inline IWindow* ScenarioCustomizationItem::GetButtonWindow() { return mpButton.get(); }
	inline bool ScenarioCustomizationItem::IsSelected() const { return mbIsSelected; }

	int AddRef() override;
	int Release() override;
};
