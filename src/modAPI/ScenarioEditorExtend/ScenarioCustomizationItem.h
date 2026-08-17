#pragma once

#define ScenarioCustomizationItemPtr intrusive_ptr<ScenarioCustomizationItem>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class ScenarioCustomizationItem 
	: public UILayout
{
private:
#pragma region Constants
	static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_THUMBNAIL = id("PlanetCustomizationItemThumbnail");
	static const uint32_t CONTROL_ID_CUSTOMIZATION_ITEM_CURSOR = id("PlanetCustomizationItemSelected");
#pragma endregion

protected:
	ResourceKey mCustomizationKey;
	ResourceKey mThumbnailKey;
	LocalizedString mName;
	bool mbIsSelected;
	bool mbIgnoreGroundCoverLock;
	IWindowPtr mpButton;
	SporeTooltipWinProc* mpTooltipWinProc;

public:
	static const uint32_t TYPE = id(PrivateName("ScenarioCustomizationItem"));
	
	ScenarioCustomizationItem();
	~ScenarioCustomizationItem();

	bool SetCustomizationAndImage(const App::PropertyList* pPropList, ResourceKey thumbnailKey, IWinProc* pHandler);
	void SetSelection(bool bIsSelected);
	inline ResourceKey GetCustomization() { return mCustomizationKey; }
	inline ResourceKey GetThumbnail() { return mThumbnailKey; }
	inline LocalizedString* GetName() { return &mName; }
	inline bool IsGroundCoverLockIgnored() { return mbIgnoreGroundCoverLock; }
	inline IWindow* GetButtonWindow() { return mpButton.get(); }
	inline bool IsSelected() const { return mbIsSelected; }

	int AddRef() override;
	int Release() override;
};
