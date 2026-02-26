#pragma once

#include <Spore\BasicIncludes.h>

#define ScenarioCustomizationItemPtr intrusive_ptr<ScenarioCustomizationItem>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class ScenarioCustomizationItem 
	: public UILayout
	, public DefaultRefCounted
{
protected:
	ResourceKey customizationID;
	ResourceKey thumbnail;
	LocalizedString name;
	IWindow* btnWindow;
public:
	static const uint32_t TYPE = id("ScenarioCustomizationItem");
	
	ScenarioCustomizationItem();
	~ScenarioCustomizationItem();

	virtual void SetCustomizationAndImage(const App::PropertyList&, IWinProc* handler);
	virtual ResourceKey* GetCustomizationID();
	virtual ResourceKey* GetThumbnail();
	virtual LocalizedString* GetName();
	IWindow* GetButtonWindow() const;

	int AddRef() override;
	int Release() override;
};
