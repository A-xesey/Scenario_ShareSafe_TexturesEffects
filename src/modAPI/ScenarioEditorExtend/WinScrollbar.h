#pragma once

#include <Spore\UTFWin\Window.h>
#include <Spore\UTFWin\Image.h>
#include <Spore\UTFWin\InteractiveWindow.h>
#include <Spore\Clock.h>

#define IWinScrollbarPtr eastl::intrusive_ptr<UTFWin::IWinScrollbar>

namespace UTFWin
{
	class IWinScrollbar : public UTFWinObject
	{
	public:
		virtual IWindow* AsIWindow();
		virtual void SetOrientation(unsigned int nOrientation);
		virtual unsigned int GetOrientation() const;
		virtual void SetScrollbarFlag(unsigned int nScrollbarFlag, bool bSet);
		virtual bool GetScrollbarFlag(unsigned int nScrollbarFlag) const;
		virtual void SetValue(int nValue, bool bNotify);
		virtual int GetValue() const;
		virtual void SetMinimumValue(int nValue, bool bNotify);
		virtual int GetMinimumValue() const;
		virtual void SetMaximumValue(int nValue, bool bNotify);
		virtual int GetMaximumValue() const;
		virtual void SetVisibleRange(int nValue, bool bNotify);
		virtual int GetVisibleRange() const;
		virtual void SetPageOverlapRange(int nValue);
		virtual int GetPageOverlapRange() const;
		virtual void SetIncrementDelta(int nValue);
		virtual int GetIncrementDelta() const;
		virtual void SetMinimumThumbSize(int nValue);
		virtual int GetMinimumThumbSize() const;
		virtual void SetDrawable(IDrawable* pDrawable);
		virtual IDrawable* GetDrawable() const;

		static const uint32_t TYPE = 0x2ef0c885;
	};

	///
	/// The standard IDrawable for a scrollbar component.
	///
	class WinScrollbar
		: public InteractiveWindow
		, public IWinScrollbar
	{
	public:
		WinScrollbar();
		virtual ~WinScrollbar();

		//// OVERRIDES ////

		virtual int AddRef() override;
		virtual int Release() override;
		virtual void* Cast(uint32_t type) const override;

		virtual IWindow* AsIWindow() override;
		virtual void SetOrientation(unsigned int nOrientation) override;
		virtual unsigned int GetOrientation() const override;
		virtual void SetScrollbarFlag(unsigned int nScrollbarFlag, bool bSet) override;
		virtual bool GetScrollbarFlag(unsigned int nScrollbarFlag) const override;
		virtual void SetValue(int nValue, bool bNotify) override;
		virtual int GetValue() const override;
		virtual void SetMinimumValue(int nValue, bool bNotify) override;
		virtual int GetMinimumValue() const override;
		virtual void SetMaximumValue(int nValue, bool bNotify) override;
		virtual int GetMaximumValue() const override;
		virtual void SetVisibleRange(int nValue, bool bNotify) override;
		virtual int GetVisibleRange() const override;
		virtual void SetPageOverlapRange(int nValue) override;
		virtual int GetPageOverlapRange() const override;
		virtual void SetIncrementDelta(int nValue) override;
		virtual int GetIncrementDelta() const override;
		virtual void SetMinimumThumbSize(int nValue) override;
		virtual int GetMinimumThumbSize() const override;
		virtual void SetDrawable(IDrawable* pDrawable) override;
		virtual IDrawable* GetDrawable() const override;

	protected:
		int mnCurrentValue;
		int mnMinimumValue;
		int mnMaximumValue;
		int mnVisibleRange;
		int mnPageOverlap;
		int mnArrowDelta;
		int mnPageDelta;
		int mnMinimumThumbSize;
		uint32_t mnOrientation;
		unsigned char mnHighlightedComponent;
		unsigned char mnSelectedComponent;
		float mnThumbCursorOffset;
		bool mbRefresh;
		uint32_t mScrollbarFlags;
		int field_244;
		Clock mTimer;
		Math::Rectangle mrComponentArea[7];
	};

	/////////////////////////////////
	//// INTERNAL IMPLEMENTATION ////
	/////////////////////////////////

	/*namespace Addresses(WinScrollbar)
	{
		DeclareAddress(Paint);
		DeclareAddress(IsColliding);
		DeclareAddress(GetDimensions);
		DeclareAddress(UseCollision);

		DeclareAddress(SetSerializer);
		DeclareAddress(GetProxyID);

		DeclareAddress(ToDrawable);
		DeclareAddress(SetImage);
	}*/
	ASSERT_SIZE(WinScrollbar, 0x2D0);

	inline WinScrollbar::WinScrollbar()
		: mnCurrentValue()
		, mnMinimumValue()
		, mnMaximumValue()
		, mnVisibleRange()
		, mnPageOverlap()
		, mnArrowDelta()
		, mnPageDelta()
		, mnMinimumThumbSize()
		, mnOrientation()
		, mnHighlightedComponent()
		, mnSelectedComponent()
		, mnThumbCursorOffset()
		, mbRefresh()
		, mScrollbarFlags()
		, field_244()
		, mTimer()
		, mrComponentArea()
	{
	}

	inline WinScrollbar::~WinScrollbar() {}
}