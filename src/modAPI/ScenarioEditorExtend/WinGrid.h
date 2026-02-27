#pragma once

#include <Spore\UTFWin\Window.h>
#include "Spore\UTFWin\OutlineFormat.h"
#include "Spore\UTFWin\ScrollbarDrawable.h"
#include <Spore\UTFWin\InteractiveWindow.h>
#include "Spore\UTFWin\Image.h"
#include "WinScrollbar.h"

#include "Spore\Clock.h"

#include <EASTL\map.h>
#include <EASTL\set.h>
#include <EASTL\bonus\sparse_matrix.h>

//#define WinGridPtr eastl::intrusive_ptr<UTFWin::WinGrid>

namespace UTFWin
{
    struct RectInt{
        int left;
        int top;
        int right;
        int bottom;
    };

    struct CellFormat {
        uint32_t nTextStyle;
        uint32_t nColor[8];
        signed char nWrapping;
        signed char nHAlignment;
        signed char nVAlignment;
        uint32_t nBorder;
        uint32_t nBorderColor;
    };
    struct CellFormatExtra : public CellFormat {
        float nSizeDefault;
        float nSizeCachedActual;
    };

    struct CellData {
        int nCellType;
        int nColumn;
        int nRow;
        void* pCellData;
        CellFormat* pCellFormat;
        void* pExtraData;
        uint32_t nCellFlags;
    };

    struct CellCoordinates {
        int nColumn;
        int nRow;
    };

    enum CursorHitTestResult {
        kCursorHitTestResultNone = 0,
        kCursorHitTestResultGutterArea = 1,
        kCursorHitTestResultRowHeadingArea = 2,
        kCursorHitTestResultColumnHeadingArea = 3,
        kCursorHitTextResultHeadingSelectAllArea = 4,
        kCursorHitTestResultCellArea = 5,
        kCursorHitTestResultCount = 6
    };

    struct CursorHitTestResultData {
        CursorHitTestResult cursorHitTestResult;
        int nColumn;
        int nRow;
    };

    class IWinGrid
    {
    public:
        static const uint32_t TYPE = 0xaf1ee902;
        IWinGrid();

        typedef int(__cdecl* SortCallback)(const CellData*, const CellData*);

        virtual int AddRef();
        virtual int Release();
        virtual ~IWinGrid();
        virtual void* Cast(uint32_t typeID) const;
        virtual IWindow* AsIWindow();
        virtual bool Refresh();
        virtual void AutoSize(unsigned int nColumnCount, unsigned int nRowCount, float fMinWidth, float fMinHeight);
        virtual bool GetGridArea(Math::Rectangle& rectGridArea);
        virtual bool GetCellArea(Math::Rectangle& rectCellArea);
        virtual unsigned int GetGridFlags() const;
        virtual void SetGridFlags(unsigned int nFlags);
        virtual void SetGridFlag(const unsigned int nFlag, bool bOn);
        virtual bool GetGridFlag(const unsigned int nFlag);
        virtual bool SetDefaultTextStyle(unsigned int nTextStyleID);
        virtual bool GetDefaultTextStyle(unsigned int& nTextStyleID) const;
        virtual bool SetDefaultTextColor(unsigned int nColor, unsigned int nColorType);
        virtual bool GetDefaultTextColor(unsigned int& nColor, unsigned int nColorType);
        virtual bool SetDefaultTextColorByColumns(unsigned int nColor, unsigned int nColorType, int nColumnStart, int nColumnCount);
        virtual bool SetDefaultTextColorByRows(unsigned int nColor, unsigned int nColorType, int nRowStart, int nRowCount);
        virtual bool SetDefaultTextWrapping(bool bWrapping);
        virtual bool GetDefaultTextWrapping();
        virtual bool GetGutters(float& nLeft, float& nTop, float& nRight, float& nBottom);
        virtual bool SetGutters(float nLeft, float nTop, float nRight, float nBottom);
        virtual bool GetCellGutters(float& nLeft, float& nTop, float& nRight, float& nBottom);
        virtual bool SetCellGutters(float nLeft, float nTop, float nRight, float nBottom);
        virtual void SetOutlineUse(bool bNewDrawOutline, unsigned int nNewOutlineColor);
        virtual unsigned int GetOutlineColor();
        virtual void SetOutlineColor(unsigned int nColor);
        virtual int PauseRecalculations(bool bPause);
        virtual bool ActLikeAListbox(int nColumnCount, int nMaxSelectionCount, bool bAllowNoSelections);
        virtual bool ScrollCellsVertically(int nCount);
        virtual bool ScrollCellsHorizontally(int nCount);
        virtual bool ScrollPagesVertically(int nCount);
        virtual bool ScrollPagesHorizontally(int nCount);
        virtual void ScrollToColumn(int nColumn);
        virtual void ScrollToRow(int nRow);
        virtual void ScrollToCell(int nColumn, int nRow);
        virtual bool GetSelectionRules(int& nSelectionRule, int& nMaxSelectionCount);
        virtual bool SetSelectionRules(int nSelectionRule, int nMaxSelectionCount);
        virtual int GetSelectionCount();
        virtual bool GetSelection(CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow);
        virtual bool GetSelection2(CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow);
        virtual bool SetSelection(int nColumn, int nRow);
        virtual bool AddCellToSelection(int nColumn, int nRow, bool bAdd);
        virtual bool GetCellIsSelected(int nColumn, int nRow);
        virtual void ClearSelection();
        virtual void EnableFixedRanges(bool bEnableColumnLimit, bool bEnableRowLimit);
        virtual void SetCellLimits(int nColumns, int nRows);
        virtual bool GetCellLimits(int& nColumns, int& nRows);
        virtual bool GetCellUsedRange(RectInt& rectUsedCellRange);
        virtual bool GetCellVisibleRange(RectInt& rectRange);
        virtual bool IsColumnWithinBounds(int nColumn);
        virtual bool IsRowWithinBounds(int nRow);
        virtual bool IsCellWithinBounds(int nColumn, int nRow);
        virtual bool GetMostRecentlyAddedCell(int& nColumn, int& nRow);
        virtual bool SetGridColors(unsigned int nColumnGridColor, unsigned int nRowGridColor);
        virtual void GetGridColors(unsigned int& nColumnGridColor, unsigned int& nRowGridColor);
        virtual void SetDefaultColumnWidth(float nDefaultColumnWidth);
        virtual bool SetColumnWidth(int nColumnStart, int nCount, float nWidth);
        virtual void SetDefaultRowHeight(float nDefaultRowHeight);
        virtual bool SetRowHeight(int nRowStart, int nCount, float nHeight);
        virtual float GetDefaultColumnWidth();
        virtual float GetDefaultRowHeight();
        virtual float GetColumnWidth(int nColumnStart, int nCount);
        virtual float GetRowHeight(int nRowStart, int nCount);
        virtual bool GetColumnHeadingArea(int nColumn, Math::Rectangle& rectHeading);
        virtual bool GetRowHeadingArea(int nRow, Math::Rectangle& rectHeading);
        virtual bool EnableHeadings(bool bEnableColumnHeadings, bool bEnableRowHeadings);
        virtual bool GetHeadingSizes(float& nColumnHeadingSize, float& nRowHeadingSize);
        virtual bool SetHeadingSizes(float nColumnHeadingSize, float nRowHeadingSize);
        virtual bool SetHeadingFormat(const CellFormat& headingFormat);
        virtual void GetColumnHeading(int nColumn, wchar_t*& pHeading);
        virtual void SetColumnHeading(int nColumn, const wchar_t* pHeading);
        virtual void GetRowHeading(int nRow, wchar_t*& pHeading);
        virtual void SetRowHeading(int nRow, const wchar_t* pHeading);
        virtual bool ClearAllRowHeadings();
        virtual bool ClearAllColumnHeadings();
        virtual bool GetCellFormat(int nColumn, int nRow, CellFormat& cellFormat);
        virtual bool SetCellFormat(int nColumn, int nRow, const CellFormat& cellFormat);
        virtual bool ClearCellFormat(int nColumn, int nRow);
        virtual bool GetColCellFormatExtra(int nColumn, CellFormatExtra*& pCellFormatExtra);
        virtual bool GetRowCellFormatExtra(int nRow, CellFormatExtra*& pCellFormatExtra);
        virtual bool GetColRowCellFormatExtra(int nColumn, int nRow, CellFormatExtra*& pCellFormatExtra);
        virtual bool SetCellTextStyle(int nColumn, int nRow, unsigned int nTextStyleID);
        virtual bool SetCellColors(int nColumn, int nRow, int nColorType, unsigned int nColor);
        virtual bool SetCellWrapping(int nColumn, int nRow, int nWrapping);
        virtual bool SetCellAlignment(int nColumn, int nRow, int nHAlignment, int nVAlignment);
        virtual bool SetCellBorder(int nColumn, int nRow, int nBorder, int nBorderColor);
        virtual bool GetCellType(int nColumn, int nRow, int& nCellType);
        virtual bool ClearCell(int nColumn, int nRow);
        virtual bool ClearCellRect(int nColumn, int nRow, int nColumnCount, int nRowCount);
        virtual bool ClearAllCells();
        virtual bool DeleteCell(int __formal0, int __formal1, int __formal2, int nHowToMakeRoom);
        virtual bool DeleteColumns(int nColumnStart, int nColumnCount, int nHowToMakeRoom);
        virtual bool DeleteRows(int nRowStart, int nRowCount, int nHowToMakeRoom);
        virtual bool CopyColumns(int nColumnSource, int nColumnDestination, int nColumnCount, bool bClearSourceColumns);
        virtual bool CopyRows(int nRowSource, int nRowDestination, int nRowCount, bool bClearSourceRows);
        virtual bool IsCellVisible(int nColumn, int nRow);
        virtual bool MakeCellVisible(int nColumn, int nRow);
        virtual bool GetCellRect(int nColumn, int nRow, Math::Rectangle& rectCell);
        virtual bool GetFirstVisibleCell(int& nColumn, int& nRow);
        virtual bool SetFirstVisibleCell(int nColumn, int nRow);
        virtual bool GetVisibleColumnAndRowCount(float& fColumnCount, float& fRowCount);
        virtual bool DoCursorHitTest(float nCursorX, float nCursorY, CursorHitTestResult& cursorHitTestResult, CellCoordinates& cellCoordinates);
        virtual bool GetCellTextValue(int nColumn, int nRow, wchar_t*& pText);
        virtual bool SetCellTextValue(int nColumn, int nRow, const wchar_t* pText, unsigned int nTextLength, int nHowToMakeRoom, bool bEditable);
        virtual bool GetCellImage(int nColumn, int nRow, Image*& pImage);
        virtual bool SetCellImage(int nColumn, int nRow, Image* pImage, int nHowToMakeRoom);
        virtual bool GetCellWindow(int nColumn, int nRow, IWindow*& pWindow);
        virtual bool SetCellWindow(int nColumn, int nRow, IWindow* pWindow, bool bStretchHorizontal, bool bStretchVertical, int nHowToMakeRoom);
        virtual bool IsWindowACellWindow(IWindow* pWindow, unsigned int nID, int* pColumn, int* pRow);
        virtual void ResizeColumnToFitCellWindows(int nColumn, int nCount);
        virtual bool SetCellExtraData(int nColumn, int nRow, void* pData);
        virtual void* GetCellExtraData(int nColumn, int nRow);
        virtual bool SetColumnExtraData(int nColumn, void* pData);
        virtual void* GetColumnExtraData(int nColumn);
        virtual bool SetRowExtraData(int nRow, void* pData);
        virtual void* GetRowExtraData(int nRow);
        virtual bool SetExtraData(void* pData);
        virtual void* GetExtraData();
        virtual void* GetCellExtraDataObject(int nColumn, int nRow, uint32_t nIID);
        virtual bool SetCellExtraDataObject(int nColumn, int nRow, Object* pObject);
        virtual void* GetExtraDataObject(uint32_t nIID);
        virtual bool SetExtraDataObject(Object* pObject);
        virtual void SortColumn(int nColumn, int nSortDirection, bool bOtherColumnsFollow, SortCallback* pSortFunction);
        virtual void SortRow(int nRow, int nSortDirection, bool bOtherRowsFollow, SortCallback* pSortFunction);
        virtual bool FindCellText(const wchar_t* pText, unsigned int nTextLength, int nColumnStart, int nColumnCount, int nRowStart, int nRowCount, int& nColumnResult, int& nRowResult, bool bMatchEntireCell);
        virtual bool SetDefaultAlignment(int nAlignmentH, int nAlignmentV);
        virtual bool SetDefaultAlignmentByColumns(int nColumnStart, int nCount, int nAlignmentH, int nAlignmentV);
        virtual bool SetDefaultAlignmentByRows(int nRowStart, int nCount, int nAlignmentH, int nAlignmentV);
        virtual bool SetScrollBarDrawableVertical(IScrollbarDrawable* pScrollbarDrawable);
        virtual bool SetScrollBarDrawableHorizontal(IScrollbarDrawable* pScrollbarDrawable);
        virtual IScrollbarDrawable* GetScrollBarDrawableVertical();
        virtual IScrollbarDrawable* GetScrollBarDrawableHorizontal();
        virtual bool SetSoundID(int nSoundType, unsigned int nSoundID);
    };

	class WinGrid
        : public InteractiveWindow
        , public IWinGrid
	{
    public:
        static const uint32_t TYPE = 0xaf1ee902;
        WinGrid();

        virtual int AddRef() override;
        virtual int Release() override;
        virtual ~WinGrid();
        virtual void* Cast(uint32_t typeID) const override;
        virtual IWindow* AsIWindow() override;
        virtual bool Refresh() override;
        virtual void AutoSize(unsigned int nColumnCount, unsigned int nRowCount, float fMinWidth, float fMinHeight) override;
        virtual bool GetGridArea(Math::Rectangle& rectGridArea) override;
        virtual bool GetCellArea(Math::Rectangle& rectCellArea) override;
        virtual unsigned int GetGridFlags() const override;
        virtual void SetGridFlags(unsigned int nFlags) override;
        virtual void SetGridFlag(const unsigned int nFlag, bool bOn) override;
        virtual bool GetGridFlag(const unsigned int nFlag) override;
        virtual bool SetDefaultTextStyle(unsigned int nTextStyleID) override;
        virtual bool GetDefaultTextStyle(unsigned int& nTextStyleID) const override;
        virtual bool SetDefaultTextColor(unsigned int nColor, unsigned int nColorType) override;
        virtual bool GetDefaultTextColor(unsigned int& nColor, unsigned int nColorType) override;
        virtual bool SetDefaultTextColorByColumns(unsigned int nColor, unsigned int nColorType, int nColumnStart, int nColumnCount) override;
        virtual bool SetDefaultTextColorByRows(unsigned int nColor, unsigned int nColorType, int nRowStart, int nRowCount) override;
        virtual bool SetDefaultTextWrapping(bool bWrapping) override;
        virtual bool GetDefaultTextWrapping() override;
        virtual bool GetGutters(float& nLeft, float& nTop, float& nRight, float& nBottom) override;
        virtual bool SetGutters(float nLeft, float nTop, float nRight, float nBottom) override;
        virtual bool GetCellGutters(float& nLeft, float& nTop, float& nRight, float& nBottom) override;
        virtual bool SetCellGutters(float nLeft, float nTop, float nRight, float nBottom) override;
        virtual void SetOutlineUse(bool bNewDrawOutline, unsigned int nNewOutlineColor) override;
        virtual unsigned int GetOutlineColor() override;
        virtual void SetOutlineColor(unsigned int nColor) override;
        virtual int PauseRecalculations(bool bPause) override;
        virtual bool ActLikeAListbox(int nColumnCount, int nMaxSelectionCount, bool bAllowNoSelections) override;
        virtual bool ScrollCellsVertically(int nCount) override;
        virtual bool ScrollCellsHorizontally(int nCount) override;
        virtual bool ScrollPagesVertically(int nCount) override;
        virtual bool ScrollPagesHorizontally(int nCount) override;
        virtual void ScrollToColumn(int nColumn) override;
        virtual void ScrollToRow(int nRow) override;
        virtual void ScrollToCell(int nColumn, int nRow) override;
        virtual bool GetSelectionRules(int& nSelectionRule, int& nMaxSelectionCount) override;
        virtual bool SetSelectionRules(int nSelectionRule, int nMaxSelectionCount) override;
        virtual int GetSelectionCount() override;
        virtual bool GetSelection(CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow) override;
        virtual bool GetSelection2(CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow) override;
        virtual bool SetSelection(int nColumn, int nRow) override;
        virtual bool AddCellToSelection(int nColumn, int nRow, bool bAdd) override;
        virtual bool GetCellIsSelected(int nColumn, int nRow) override;
        virtual void ClearSelection() override;
        virtual void EnableFixedRanges(bool bEnableColumnLimit, bool bEnableRowLimit) override;
        virtual void SetCellLimits(int nColumns, int nRows) override;
        virtual bool GetCellLimits(int& nColumns, int& nRows) override;
        virtual bool GetCellUsedRange(RectInt& rectUsedCellRange) override;
        virtual bool GetCellVisibleRange(RectInt& rectRange) override;
        virtual bool IsColumnWithinBounds(int nColumn) override;
        virtual bool IsRowWithinBounds(int nRow) override;
        virtual bool IsCellWithinBounds(int nColumn, int nRow) override;
        virtual bool GetMostRecentlyAddedCell(int& nColumn, int& nRow) override;
        virtual bool SetGridColors(unsigned int nColumnGridColor, unsigned int nRowGridColor) override;
        virtual void GetGridColors(unsigned int& nColumnGridColor, unsigned int& nRowGridColor) override;
        virtual void SetDefaultColumnWidth(float nDefaultColumnWidth) override;
        virtual bool SetColumnWidth(int nColumnStart, int nCount, float nWidth) override;
        virtual void SetDefaultRowHeight(float nDefaultRowHeight) override;
        virtual bool SetRowHeight(int nRowStart, int nCount, float nHeight) override;
        virtual float GetDefaultColumnWidth() override;
        virtual float GetDefaultRowHeight() override;
        virtual float GetColumnWidth(int nColumnStart, int nCount) override;
        virtual float GetRowHeight(int nRowStart, int nCount) override;
        virtual bool GetColumnHeadingArea(int nColumn, Math::Rectangle& rectHeading) override;
        virtual bool GetRowHeadingArea(int nRow, Math::Rectangle& rectHeading) override;
        virtual bool EnableHeadings(bool bEnableColumnHeadings, bool bEnableRowHeadings) override;
        virtual bool GetHeadingSizes(float& nColumnHeadingSize, float& nRowHeadingSize) override;
        virtual bool SetHeadingSizes(float nColumnHeadingSize, float nRowHeadingSize) override;
        virtual bool SetHeadingFormat(const CellFormat& headingFormat) override;
        virtual void GetColumnHeading(int nColumn, wchar_t*& pHeading) override;
        virtual void SetColumnHeading(int nColumn, const wchar_t* pHeading) override;
        virtual void GetRowHeading(int nRow, wchar_t*& pHeading) override;
        virtual void SetRowHeading(int nRow, const wchar_t* pHeading) override;
        virtual bool ClearAllRowHeadings() override;
        virtual bool ClearAllColumnHeadings() override;
        virtual bool GetCellFormat(int nColumn, int nRow, CellFormat& cellFormat) override;
        virtual bool SetCellFormat(int nColumn, int nRow, const CellFormat& cellFormat) override;
        virtual bool ClearCellFormat(int nColumn, int nRow) override;
        virtual bool GetColCellFormatExtra(int nColumn, CellFormatExtra*& pCellFormatExtra) override;
        virtual bool GetRowCellFormatExtra(int nRow, CellFormatExtra*& pCellFormatExtra) override;
        virtual bool GetColRowCellFormatExtra(int nColumn, int nRow, CellFormatExtra*& pCellFormatExtra) override;
        virtual bool SetCellTextStyle(int nColumn, int nRow, unsigned int nTextStyleID) override;
        virtual bool SetCellColors(int nColumn, int nRow, int nColorType, unsigned int nColor) override;
        virtual bool SetCellWrapping(int nColumn, int nRow, int nWrapping) override;
        virtual bool SetCellAlignment(int nColumn, int nRow, int nHAlignment, int nVAlignment) override;
        virtual bool SetCellBorder(int nColumn, int nRow, int nBorder, int nBorderColor) override;
        virtual bool GetCellType(int nColumn, int nRow, int& nCellType) override;
        virtual bool ClearCell(int nColumn, int nRow) override;
        virtual bool ClearCellRect(int nColumn, int nRow, int nColumnCount, int nRowCount) override;
        virtual bool ClearAllCells() override;
        virtual bool DeleteCell(int __formal0, int __formal1, int __formal2, int nHowToMakeRoom) override;
        virtual bool DeleteColumns(int nColumnStart, int nColumnCount, int nHowToMakeRoom) override;
        virtual bool DeleteRows(int nRowStart, int nRowCount, int nHowToMakeRoom) override;
        virtual bool CopyColumns(int nColumnSource, int nColumnDestination, int nColumnCount, bool bClearSourceColumns) override;
        virtual bool CopyRows(int nRowSource, int nRowDestination, int nRowCount, bool bClearSourceRows) override;
        virtual bool IsCellVisible(int nColumn, int nRow) override;
        virtual bool MakeCellVisible(int nColumn, int nRow) override;
        virtual bool GetCellRect(int nColumn, int nRow, Math::Rectangle& rectCell) override;
        virtual bool GetFirstVisibleCell(int& nColumn, int& nRow) override;
        virtual bool SetFirstVisibleCell(int nColumn, int nRow) override;
        virtual bool GetVisibleColumnAndRowCount(float& fColumnCount, float& fRowCount) override;
        virtual bool DoCursorHitTest(float nCursorX, float nCursorY, CursorHitTestResult& cursorHitTestResult, CellCoordinates& cellCoordinates) override;
        virtual bool GetCellTextValue(int nColumn, int nRow, wchar_t*& pText) override;
        virtual bool SetCellTextValue(int nColumn, int nRow, const wchar_t* pText, unsigned int nTextLength, int nHowToMakeRoom, bool bEditable) override;
        virtual bool GetCellImage(int nColumn, int nRow, Image*& pImage) override;
        virtual bool SetCellImage(int nColumn, int nRow, Image* pImage, int nHowToMakeRoom) override;
        virtual bool GetCellWindow(int nColumn, int nRow, IWindow*& pWindow) override;
        virtual bool SetCellWindow(int nColumn, int nRow, IWindow* pWindow, bool bStretchHorizontal, bool bStretchVertical, int nHowToMakeRoom) override;
        virtual bool IsWindowACellWindow(IWindow* pWindow, unsigned int nID, int* pColumn, int* pRow) override;
        virtual void ResizeColumnToFitCellWindows(int nColumn, int nCount) override;
        virtual bool SetCellExtraData(int nColumn, int nRow, void* pData) override;
        virtual void* GetCellExtraData(int nColumn, int nRow) override;
        virtual bool SetColumnExtraData(int nColumn, void* pData) override;
        virtual void* GetColumnExtraData(int nColumn) override;
        virtual bool SetRowExtraData(int nRow, void* pData) override;
        virtual void* GetRowExtraData(int nRow) override;
        virtual bool SetExtraData(void* pData) override;
        virtual void* GetExtraData() override;
        virtual void* GetCellExtraDataObject(int nColumn, int nRow, uint32_t nIID) override;
        virtual bool SetCellExtraDataObject(int nColumn, int nRow, Object* pObject) override;
        virtual void* GetExtraDataObject(uint32_t nIID) override;
        virtual bool SetExtraDataObject(Object* pObject) override;
        virtual void SortColumn(int nColumn, int nSortDirection, bool bOtherColumnsFollow, SortCallback* pSortFunction) override;
        virtual void SortRow(int nRow, int nSortDirection, bool bOtherRowsFollow, SortCallback* pSortFunction) override;
        virtual bool FindCellText(const wchar_t* pText, unsigned int nTextLength, int nColumnStart, int nColumnCount, int nRowStart, int nRowCount, int& nColumnResult, int& nRowResult, bool bMatchEntireCell) override;
        virtual bool SetDefaultAlignment(int nAlignmentH, int nAlignmentV) override;
        virtual bool SetDefaultAlignmentByColumns(int nColumnStart, int nCount, int nAlignmentH, int nAlignmentV) override;
        virtual bool SetDefaultAlignmentByRows(int nRowStart, int nCount, int nAlignmentH, int nAlignmentV) override;
        virtual bool SetScrollBarDrawableVertical(IScrollbarDrawable* pScrollbarDrawable) override;
        virtual bool SetScrollBarDrawableHorizontal(IScrollbarDrawable* pScrollbarDrawable) override;
        virtual IScrollbarDrawable* GetScrollBarDrawableVertical() override;
        virtual IScrollbarDrawable* GetScrollBarDrawableHorizontal() override;
        virtual bool SetSoundID(int nSoundType, unsigned int nSoundID) override;
    public:
        uint32_t mGridFlags;
        bool mbRecalculateAllNeeded;
        int mnRecalculationPauseCount;
        Math::Rectangle mfGutter;
        Math::Rectangle mfCellGutter;
        float mfColumnHeadingHeight;
        float mfRowHeadingWidth;
        float mfVisibleGridAreaWidth;
        float mfVisibleGridAreaHeight;
        float mfVisibleCellAreaWidth;
        float mfVisibleCellAreaHeight;
        int mnFirstVisibleColumn;
        int mnFirstVisibleRow;
        float mfVisibleColumnCount;
        float mfVisibleRowCount;
        int mnColumnCount;
        int mnRowCount;
        float mfDefaultColumnWidth;
        float mfDefaultRowHeight;
        uint32_t mnOutlineColor;
        uint32_t mnColumnGridColor;
        uint32_t mnRowGridColor;
        float mnColumnGridThickness;
        float mnRowGridThickness;
        int mnMostRecentlyAddedColumn;
        int mnMostRecentlyAddedRow;
        Clock mTimerDoubleClick;
        int mExtraDataType;
        int mnSelectionRule;
        int mnMaxSelectionCount;
        eastl::list<CellCoordinates> mCurrentSelection; //list<CellCoordinates,eastl::allocator>
        char mMatrixDefaultColumnFormatting[48];    //SparseMatrix<CellFormatExtra>
        char mMatrixDefaultRowFormatting[48];   //SparseMatrix<CellFormatExtra>
        char mMatrixCellData[48];   //SparseMatrix<EA::UTFWinControls::IWinGrid::CellData_*>
        CellFormat mDefaultFormatting;
        eastl::set<CellData> mCellWindowList;  //set<EA::UTFWinControls::IWinGrid::CellData,eastl::less<EA::UTFWinControls::IWinGrid::CellData>,eastl::allocator>
        CursorHitTestResultData mLastCursorHitTestResultData;
        CursorHitTestResultData mLastMouseOverCell;
        eastl::hash_map<uint32_t, void*> mRowExtraData;    //hash_map<unsigned_int,void_*,eastl::hash<unsigned_int>,eastl::equal_to<unsigned_int>,eastl::allocator,0>
        eastl::hash_map<uint32_t, void*> mColumnExtraData; //hash_map<unsigned_int,void_*,eastl::hash<unsigned_int>,eastl::equal_to<unsigned_int>,eastl::allocator,0>
        void* mpExtraData;
        IWinScrollbarPtr mpScrollBarVertical;
        IWinScrollbarPtr mpScrollBarHorizontal;
        IScrollbarDrawablePtr mpScrollBarDrawableVertical;
        IScrollbarDrawablePtr mpScrollBarDrawableHorizontal;
        uint32_t mSoundIDs[3];
        eastl::hash_map<uint32_t, eastl::string16> mColumnHeadings;  //hash_map<unsigned_int,eastl::basic_string<wchar_t,eastl::allocator>,eastl::hash<unsigned_int>,eastl::equal_to<unsigned_int>,eastl::allocator,0>
        eastl::hash_map<uint32_t, eastl::string16> mRowHeadings; //hash_map<unsigned_int,eastl::basic_string<wchar_t,eastl::allocator>,eastl::hash<unsigned_int>,eastl::equal_to<unsigned_int>,eastl::allocator,0>
        CellFormat mDefaultHeadingFormatting;
        char mTypesetter[1296]; //Typesetter
        char mLineLayout[208];  //LineLayout
        OutlineFormat mDropShadow;
        int field_A8C;
	};

    namespace Addresses(WinGrid)
    {
        DeclareAddress(AsIWindow);
        DeclareAddress(Refresh);
        DeclareAddress(AutoSize);
        DeclareAddress(GetGridArea);
        DeclareAddress(GetCellArea);
        DeclareAddress(GetGridFlags);
        DeclareAddress(SetGridFlags);
        DeclareAddress(SetGridFlag);
        DeclareAddress(GetGridFlag);
        DeclareAddress(SetDefaultTextStyle);
        DeclareAddress(GetDefaultTextStyle);
        DeclareAddress(SetDefaultTextColor);
        DeclareAddress(GetDefaultTextColor);
        DeclareAddress(SetDefaultTextColorByColumns);
        DeclareAddress(SetDefaultTextColorByRows);
        DeclareAddress(SetDefaultTextWrapping);
        DeclareAddress(GetDefaultTextWrapping);
        DeclareAddress(GetGutters);
        DeclareAddress(SetGutters);
        DeclareAddress(GetCellGutters);
        DeclareAddress(SetCellGutters);
        DeclareAddress(SetOutlineUse);
        DeclareAddress(GetOutlineColor);
        DeclareAddress(SetOutlineColor);
        DeclareAddress(PauseRecalculations);
        DeclareAddress(ActLikeAListbox);
        DeclareAddress(ScrollCellsVertically);
        DeclareAddress(ScrollCellsHorizontally);
        DeclareAddress(ScrollPagesVertically);
        DeclareAddress(ScrollPagesHorizontally);
        DeclareAddress(ScrollToColumn);
        DeclareAddress(ScrollToRow);
        DeclareAddress(ScrollToCell);
        DeclareAddress(GetSelectionRules);
        DeclareAddress(SetSelectionRules);
        DeclareAddress(GetSelectionCount);
        DeclareAddress(GetSelection);
        DeclareAddress(GetSelection2);
        DeclareAddress(SetSelection);
        DeclareAddress(AddCellToSelection);
        DeclareAddress(GetCellIsSelected);
        DeclareAddress(ClearSelection);
        DeclareAddress(EnableFixedRanges);
        DeclareAddress(SetCellLimits);
        DeclareAddress(GetCellLimits);
        DeclareAddress(GetCellUsedRange);
        DeclareAddress(GetCellVisibleRange);
        DeclareAddress(IsColumnWithinBounds);
        DeclareAddress(IsRowWithinBounds);
        DeclareAddress(IsCellWithinBounds);
        DeclareAddress(GetMostRecentlyAddedCell);
        DeclareAddress(SetGridColors);
        DeclareAddress(GetGridColors);
        DeclareAddress(SetDefaultColumnWidth);
        DeclareAddress(SetColumnWidth);
        DeclareAddress(SetDefaultRowHeight);
        DeclareAddress(SetRowHeight);
        DeclareAddress(GetDefaultColumnWidth);
        DeclareAddress(GetDefaultRowHeight);
        DeclareAddress(GetColumnWidth);
        DeclareAddress(GetRowHeight);
        DeclareAddress(GetColumnHeadingArea);
        DeclareAddress(GetRowHeadingArea);
        DeclareAddress(EnableHeadings);
        DeclareAddress(GetHeadingSizes);
        DeclareAddress(SetHeadingSizes);
        DeclareAddress(SetHeadingFormat);
        DeclareAddress(GetColumnHeading);
        DeclareAddress(SetColumnHeading);
        DeclareAddress(GetRowHeading);
        DeclareAddress(SetRowHeading);
        DeclareAddress(ClearAllRowHeadings);
        DeclareAddress(ClearAllColumnHeadings);
        DeclareAddress(GetCellFormat);
        DeclareAddress(SetCellFormat);
        DeclareAddress(ClearCellFormat);
        DeclareAddress(GetColCellFormatExtra);
        DeclareAddress(GetRowCellFormatExtra);
        DeclareAddress(GetColRowCellFormatExtra);
        DeclareAddress(SetCellTextStyle);
        DeclareAddress(SetCellColors);
        DeclareAddress(SetCellWrapping);
        DeclareAddress(SetCellAlignment);
        DeclareAddress(SetCellBorder);
        DeclareAddress(GetCellType);
        DeclareAddress(ClearCell);
        DeclareAddress(ClearCellRect);
        DeclareAddress(ClearAllCells);
        DeclareAddress(DeleteCell);
        DeclareAddress(DeleteColumns);
        DeclareAddress(DeleteRows);
        DeclareAddress(CopyColumns);
        DeclareAddress(CopyRows);
        DeclareAddress(IsCellVisible);
        DeclareAddress(MakeCellVisible);
        DeclareAddress(GetCellRect);
        DeclareAddress(GetFirstVisibleCell);
        DeclareAddress(SetFirstVisibleCell);
        DeclareAddress(GetVisibleColumnAndRowCount);
        DeclareAddress(DoCursorHitTest);
        DeclareAddress(GetCellTextValue);
        DeclareAddress(SetCellTextValue);
        DeclareAddress(GetCellImage);
        DeclareAddress(SetCellImage);
        DeclareAddress(GetCellWindow);
        DeclareAddress(SetCellWindow);
        DeclareAddress(IsWindowACellWindow);
        DeclareAddress(ResizeColumnToFitCellWindows);
        DeclareAddress(SetCellExtraData);
        DeclareAddress(GetCellExtraData);
        DeclareAddress(SetColumnExtraData);
        DeclareAddress(GetColumnExtraData);
        DeclareAddress(SetRowExtraData);
        DeclareAddress(GetRowExtraData);
        DeclareAddress(SetExtraData);
        DeclareAddress(GetExtraData);
        DeclareAddress(GetCellExtraDataObject);
        DeclareAddress(SetCellExtraDataObject);
        DeclareAddress(GetExtraDataObject);
        DeclareAddress(SetExtraDataObject);
        DeclareAddress(SortColumn);
        DeclareAddress(SortRow);
        DeclareAddress(FindCellText);
        DeclareAddress(SetDefaultAlignment);
        DeclareAddress(SetDefaultAlignmentByColumns);
        DeclareAddress(SetDefaultAlignmentByRows);
        DeclareAddress(SetScrollBarDrawableVertical);
        DeclareAddress(SetScrollBarDrawableHorizontal);
        DeclareAddress(GetScrollBarDrawableVertical);
        DeclareAddress(GetScrollBarDrawableHorizontal);
        DeclareAddress(SetSoundID);
    }

    ASSERT_SIZE(WinGrid, 0xA90);
}