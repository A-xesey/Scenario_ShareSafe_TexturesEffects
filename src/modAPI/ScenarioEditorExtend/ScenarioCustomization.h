#pragma once

#include <Spore\BasicIncludes.h>
#include "ScenarioCustomizationItem.h"

#define ScenarioCustomizationPtr intrusive_ptr<ScenarioCustomization>

// To avoid repeating UTFWin:: all the time.
using namespace UTFWin;

class ScenarioCustomization 
	: public IWinProc
	, public DefaultRefCounted
{
protected:
	vector<ScenarioCustomizationItemPtr> items;
	bool initialized;
	uint32_t openedWinID;
public:
	static const uint32_t TYPE = id("ScenarioCustomization");
	
	ScenarioCustomization();
	~ScenarioCustomization();

	virtual void InitItems(IWindow* window);

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	
	int GetEventFlags() const override;
	// This is the function you have to implement, called when a window you added this winproc to received an event
	bool HandleUIMessage(IWindow* pWindow, const Message& message) override;
	
};
