#include "stdafx.h"
#include "ScenarioCustomizationItem.h"
#include <string>
#include <iostream>

ScenarioCustomizationItem::ScenarioCustomizationItem()
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

void ScenarioCustomizationItem::SetCustomizationAndImage(const App::PropertyList& propList)
{
	this->LoadByID(id("PlanetCustomizationItem"));

	ResourceKey CustomizationID;
	ResourceKey Thumbnail;
	LocalizedString name;
	App::Property::GetKey(&propList, id("PlanetCustomizationDirectory"), CustomizationID);
	App::Property::GetKey(&propList, id("PlanetCustomizationImage"), Thumbnail);
	App::Property::GetText(&propList, id("PlanetCustomizationName"), name);
	IWindowPtr winThumb = this->FindWindowByID(id("PlanetCustomizationItemThumbnail"));
	if (winThumb != nullptr && Thumbnail.instanceID != 0)
		UTFWin::Image::SetBackgroundByKey(winThumb.get(), Thumbnail);

	IWindowPtr winButton = this->FindWindowByID(id("PlanetCustomizationItem"));
	if (winButton != nullptr && name.GetText() != nullptr)
		winButton->SetCaption(name.GetText());

	IWindowPtr winDirectory = this->FindWindowByID(id("PlanetCustomizationItemDirectory"));
	if (winDirectory != nullptr && CustomizationID.instanceID != 0)
	{
		std::u16string CustomizationIDString = uintToString(CustomizationID.groupID) + u"!" + uintToString(CustomizationID.instanceID) + u"." + uintToString(CustomizationID.typeID);
		winDirectory->SetCaption(CustomizationIDString.c_str());
	}
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


