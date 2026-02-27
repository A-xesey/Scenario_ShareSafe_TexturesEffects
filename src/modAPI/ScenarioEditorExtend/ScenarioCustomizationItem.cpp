#include "stdafx.h"
#include "ScenarioCustomizationItem.h"
#include <string>
#include <iostream>

ScenarioCustomizationItem::ScenarioCustomizationItem() : btnWindow(nullptr)
{

}


ScenarioCustomizationItem::~ScenarioCustomizationItem()
{
}



std::u16string uintToString(const unsigned int& i) {
	unsigned int num = i;
	std::string s;
	while (num != 0)
	{
		int mod = num % 16;
		if (mod >= 10)
		{
			if (mod == 10) s = "A" + s;
			if (mod == 11) s = "B" + s;
			if (mod == 12) s = "C" + s;
			if (mod == 13) s = "D" + s;
			if (mod == 14) s = "E" + s;
			if (mod == 15) s = "F" + s;
		}
		else s = std::to_string(mod) + s;
		num /= 16;
	}
	while (s.length() != 8) s = "0" + s;
	return std::u16string(s.begin(), s.end());
}

void ScenarioCustomizationItem::SetCustomizationAndImage(const App::PropertyList& propList, IWinProc* handler)
{
	this->LoadByID(id("PlanetCustomizationItem"));

	App::Property::GetKey(&propList, id("PlanetCustomizationDirectory"), customizationID);
	App::Property::GetKey(&propList, id("PlanetCustomizationImage"), thumbnail);
	App::Property::GetText(&propList, id("PlanetCustomizationName"), name);
	IWindowPtr winThumb = this->FindWindowByID(id("PlanetCustomizationItemThumbnail"));
	if (winThumb != nullptr && thumbnail.instanceID != 0)
		UTFWin::Image::SetBackgroundByKey(winThumb.get(), thumbnail);

	IWindowPtr winButton = this->FindWindowByID(id("PlanetCustomizationItem"));
	if (winButton != nullptr)
	{
		App::ConsolePrintF("winButton");
		//winButton->SetCaption(name.GetText());
		SporeTooltipWinProcPtr tooltip = new UTFWin::SporeTooltipWinProc(u"tooltips", 0x3754E6C, name.GetText(), Math::Point(0, 30.0f), TooltipBehaviour::Default, u"", 0x3754E6C);
		winButton->AddWinProc(handler);
		winButton->AddWinProc(tooltip.get());
		btnWindow = winButton.get();
	}

	IWindowPtr winDirectory = this->FindWindowByID(id("PlanetCustomizationItemDirectory"));
	if (winDirectory != nullptr && customizationID.instanceID != 0)
	{
		std::u16string CustomizationIDString = uintToString(customizationID.groupID) + u"!" + uintToString(customizationID.instanceID) + u"." + uintToString(customizationID.typeID);
		winDirectory->SetCaption(CustomizationIDString.c_str());
	}
}

ResourceKey* ScenarioCustomizationItem::GetCustomizationID()
{
	return &customizationID;
}
ResourceKey* ScenarioCustomizationItem::GetThumbnail()
{
	return &thumbnail;
}
LocalizedString* ScenarioCustomizationItem::GetName()
{
	return &name;
}
IWindow* ScenarioCustomizationItem::GetButtonWindow() const
{
	return btnWindow;
}

// For internal use, do not modify.
int ScenarioCustomizationItem::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ScenarioCustomizationItem::Release()
{
	return DefaultRefCounted::Release();
}


