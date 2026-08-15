#pragma once

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
	SporeTooltipWinProc* mpTooltipWinProc;
public:
	static const uint32_t TYPE = id(PrivateName("ScenarioCustomizationItem"));
	
	ScenarioCustomizationItem();
	~ScenarioCustomizationItem();

	virtual void SetCustomizationAndImage(const App::PropertyList* pPropList, ResourceKey thumbnailKey, IWinProc* pHandler);
	virtual void SetSelection(bool bIsSelected);
	inline ResourceKey GetCustomization() { return mCustomizationKey; }
	inline ResourceKey GetThumbnail() { return mThumbnailKey; }
	inline LocalizedString* GetName() { return &mName; }
	inline IWindow* GetButtonWindow() { return mpButton.get(); }
	inline bool IsSelected() const { return mbIsSelected; }

	int AddRef() override;
	int Release() override;
};
