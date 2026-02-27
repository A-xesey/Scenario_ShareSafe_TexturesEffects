#include "stdafx.h"
#include "WinGrid.h"
#include <SourceCode\Utility.h>
#include "Spore\UTFWin\Image.h"

namespace UTFWin
{
	WinGrid::WinGrid():
        mGridFlags(0x19be6),
        mbRecalculateAllNeeded(true),
        mnRecalculationPauseCount(),
        mfGutter(5.0f, 5.0f, 5.0f, 5.0f),
        mfCellGutter(1.0f, 1.0f, 1.0f, 1.0f),
        mfColumnHeadingHeight(80.0f),
        mfRowHeadingWidth(20.0f),
        mfVisibleGridAreaWidth(1.0f),
        mfVisibleGridAreaHeight(1.0f),
        mfVisibleCellAreaWidth(1.0f),
        mfVisibleCellAreaHeight(1.0f),
        mnFirstVisibleColumn(),
        mnFirstVisibleRow(),
        mfVisibleColumnCount(1.0f),
        mfVisibleRowCount(1.0f),
        mnColumnCount(-1),
        mnRowCount(-1),
        mfDefaultColumnWidth(100.0f),
        mfDefaultRowHeight(20.0f),
        mnOutlineColor(),
        mnColumnGridColor(),
        mnRowGridColor(),
        mnColumnGridThickness(1.0f),
        mnRowGridThickness(1.0f),
        mnMostRecentlyAddedColumn(-1),
        mnMostRecentlyAddedRow(-1),
        mTimerDoubleClick(Clock::Mode::Milliseconds,false),
        mExtraDataType(),
        mnSelectionRule(1),
        mnMaxSelectionCount(0x7fffffff),
        mCurrentSelection(),
        mMatrixDefaultColumnFormatting(),
        mMatrixDefaultRowFormatting(),
        mMatrixCellData(),
        mDefaultFormatting(),
        mCellWindowList(nullptr),
        mLastCursorHitTestResultData(),
        mLastMouseOverCell(),
        mRowExtraData(),
        mColumnExtraData(),
        mpExtraData(nullptr),
        mpScrollBarVertical(nullptr),
        mpScrollBarHorizontal(nullptr),
        mpScrollBarDrawableVertical(nullptr),
        mpScrollBarDrawableHorizontal(nullptr),
        mSoundIDs(),
        mColumnHeadings(),
        mRowHeadings(),
        mDefaultHeadingFormatting(),
        mTypesetter(),
        mLineLayout(),
        mDropShadow(),
        field_A8C()
    {
    }

    DEFAULT_REFCOUNT(WinGrid);

    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, IWindow*, AsIWindow);
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, bool, Refresh);
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, AutoSize, Args(unsigned int nColumnCount, unsigned int nRowCount, float fMinWidth, float fMinHeight), Args(nColumnCount, nRowCount, fMinWidth, fMinHeight));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetGridArea, Args(Math::Rectangle& rectGridArea), Args(rectGridArea));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellArea, Args(Math::Rectangle& rectCellArea), Args(rectCellArea));
    auto_METHOD_VIRTUAL_const_(WinGrid, IWinGrid, unsigned int, GetGridFlags);
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetGridFlags, Args(unsigned int nFlags), Args(nFlags));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetGridFlag, Args(const unsigned int nFlag, bool bOn), Args(nFlag, bOn));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetGridFlag, Args(const unsigned int nFlag), Args(nFlag));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultTextStyle, Args(unsigned int nTextStyleID), Args(nTextStyleID));
    auto_METHOD_VIRTUAL_const(WinGrid, IWinGrid, bool, GetDefaultTextStyle, Args(unsigned int& nTextStyleID), Args(nTextStyleID));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultTextColor, Args(unsigned int nColor, unsigned int nColorType), Args(nColor, nColorType));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetDefaultTextColor, Args(unsigned int& nColor, unsigned int nColorType), Args(nColor, nColorType));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultTextColorByColumns, Args(unsigned int nColor, unsigned int nColorType, int nColumnStart, int nColumnCount), Args(nColor, nColorType, nColumnStart, nColumnCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultTextColorByRows, Args(unsigned int nColor, unsigned int nColorType, int nRowStart, int nRowCount), Args(nColor, nColorType, nRowStart, nRowCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultTextWrapping, Args(bool bWrapping), Args(bWrapping));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, bool, GetDefaultTextWrapping);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetGutters, Args(float& nLeft, float& nTop, float& nRight, float& nBottom), Args(nLeft, nTop, nRight, nBottom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetGutters, Args(float nLeft, float nTop, float nRight, float nBottom), Args(nLeft, nTop, nRight, nBottom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellGutters, Args(float& nLeft, float& nTop, float& nRight, float& nBottom), Args(nLeft, nTop, nRight, nBottom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellGutters, Args(float nLeft, float nTop, float nRight, float nBottom), Args(nLeft, nTop, nRight, nBottom));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetOutlineUse, Args(bool bNewDrawOutline, unsigned int nNewOutlineColor), Args(bNewDrawOutline, nNewOutlineColor));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, unsigned int, GetOutlineColor);
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetOutlineColor, Args(unsigned int nColor), Args(nColor));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, int, PauseRecalculations, Args(bool bPause), Args(bPause));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ActLikeAListbox, Args(int nColumnCount, int nMaxSelectionCount, bool bAllowNoSelections), Args(nColumnCount, nMaxSelectionCount, bAllowNoSelections));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ScrollCellsVertically, Args(int nCount), Args(nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ScrollCellsHorizontally, Args(int nCount), Args(nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ScrollPagesVertically, Args(int nCount), Args(nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ScrollPagesHorizontally, Args(int nCount), Args(nCount));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, ScrollToColumn, Args(int nColumn), Args(nColumn));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, ScrollToRow, Args(int nRow), Args(nRow));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, ScrollToCell, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetSelectionRules, Args(int& nSelectionRule, int& nMaxSelectionCount), Args(nSelectionRule, nMaxSelectionCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetSelectionRules, Args(int nSelectionRule, int nMaxSelectionCount), Args(nSelectionRule, nMaxSelectionCount));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, int, GetSelectionCount);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetSelection, Args(UTFWin::CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow), Args(pCellCoordinates, nCount, nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetSelection2, Args(UTFWin::CellCoordinates* pCellCoordinates, int& nCount, int& nColumn, int& nRow), Args(pCellCoordinates, nCount, nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetSelection, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, AddCellToSelection, Args(int nColumn, int nRow, bool bAdd), Args(nColumn, nRow, bAdd));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellIsSelected, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL_VOID_(WinGrid, IWinGrid, ClearSelection);
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, EnableFixedRanges, Args(bool bEnableColumnLimit, bool bEnableRowLimit), Args(bEnableColumnLimit, bEnableRowLimit));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetCellLimits, Args(int nColumns, int nRows), Args(nColumns, nRows));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellLimits, Args(int& nColumns, int& nRows), Args(nColumns, nRows));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellUsedRange, Args(UTFWin::RectInt& rectUsedCellRange), Args(rectUsedCellRange));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellVisibleRange, Args(UTFWin::RectInt& rectRange), Args(rectRange));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, IsColumnWithinBounds, Args(int nColumn), Args(nColumn));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, IsRowWithinBounds, Args(int nRow), Args(nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, IsCellWithinBounds, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetMostRecentlyAddedCell, Args(int& nColumn, int& nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetGridColors, Args(unsigned int nColumnGridColor, unsigned int nRowGridColor), Args(nColumnGridColor, nRowGridColor));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, GetGridColors, Args(unsigned int& nColumnGridColor, unsigned int& nRowGridColor), Args(nColumnGridColor, nRowGridColor));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetDefaultColumnWidth, Args(float nDefaultColumnWidth), Args(nDefaultColumnWidth));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetColumnWidth, Args(int nColumnStart, int nCount, float nWidth), Args(nColumnStart, nCount, nWidth));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetDefaultRowHeight, Args(float nDefaultRowHeight), Args(nDefaultRowHeight));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetRowHeight, Args(int nRowStart, int nCount, float nHeight), Args(nRowStart, nCount, nHeight));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, float, GetDefaultColumnWidth);
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, float, GetDefaultRowHeight);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, float, GetColumnWidth, Args(int nColumnStart, int nCount), Args(nColumnStart, nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, float, GetRowHeight, Args(int nRowStart, int nCount), Args(nRowStart, nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetColumnHeadingArea, Args(int nColumn, Math::Rectangle& rectHeading), Args(nColumn, rectHeading));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetRowHeadingArea, Args(int nRow, Math::Rectangle& rectHeading), Args(nRow, rectHeading));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, EnableHeadings, Args(bool bEnableColumnHeadings, bool bEnableRowHeadings), Args(bEnableColumnHeadings, bEnableRowHeadings));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetHeadingSizes, Args(float& nColumnHeadingSize, float& nRowHeadingSize), Args(nColumnHeadingSize, nRowHeadingSize));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetHeadingSizes, Args(float nColumnHeadingSize, float nRowHeadingSize), Args(nColumnHeadingSize, nRowHeadingSize));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetHeadingFormat, Args(const UTFWin::CellFormat& headingFormat), Args(headingFormat));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, GetColumnHeading, Args(int nColumn, wchar_t*& pHeading), Args(nColumn, pHeading));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetColumnHeading, Args(int nColumn, const wchar_t* pHeading), Args(nColumn, pHeading));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, GetRowHeading, Args(int nRow, wchar_t*& pHeading), Args(nRow, pHeading));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SetRowHeading, Args(int nRow, const wchar_t* pHeading), Args(nRow, pHeading));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, bool, ClearAllRowHeadings);
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, bool, ClearAllColumnHeadings);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellFormat, Args(int nColumn, int nRow, UTFWin::CellFormat& cellFormat), Args(nColumn, nRow, cellFormat));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellFormat, Args(int nColumn, int nRow, const UTFWin::CellFormat& cellFormat), Args(nColumn, nRow, cellFormat));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ClearCellFormat, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetColCellFormatExtra, Args(int nColumn, UTFWin::CellFormatExtra*& pCellFormatExtra), Args(nColumn, pCellFormatExtra));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetRowCellFormatExtra, Args(int nRow, UTFWin::CellFormatExtra*& pCellFormatExtra), Args(nRow, pCellFormatExtra));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetColRowCellFormatExtra, Args(int nColumn, int nRow, UTFWin::CellFormatExtra*& pCellFormatExtra), Args(nColumn, nRow, pCellFormatExtra));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellTextStyle, Args(int nColumn, int nRow, unsigned int nTextStyleID), Args(nColumn, nRow, nTextStyleID));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellColors, Args(int nColumn, int nRow, int nColorType, unsigned int nColor), Args(nColumn, nRow, nColorType, nColor));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellWrapping, Args(int nColumn, int nRow, int nWrapping), Args(nColumn, nRow, nWrapping));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellAlignment, Args(int nColumn, int nRow, int nHAlignment, int nVAlignment), Args(nColumn, nRow, nHAlignment, nVAlignment));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellBorder, Args(int nColumn, int nRow, int nBorder, int nBorderColor), Args(nColumn, nRow, nBorder, nBorderColor));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellType, Args(int nColumn, int nRow, int& nCellType), Args(nColumn, nRow, nCellType));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ClearCell, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, ClearCellRect, Args(int nColumn, int nRow, int nColumnCount, int nRowCount), Args(nColumn, nRow, nColumnCount, nRowCount));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, bool, ClearAllCells);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, DeleteCell, Args(int __formal0, int __formal1, int __formal2, int nHowToMakeRoom), Args(__formal0, __formal1, __formal2, nHowToMakeRoom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, DeleteColumns, Args(int nColumnStart, int nColumnCount, int nHowToMakeRoom), Args(nColumnStart, nColumnCount, nHowToMakeRoom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, DeleteRows, Args(int nRowStart, int nRowCount, int nHowToMakeRoom), Args(nRowStart, nRowCount, nHowToMakeRoom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, CopyColumns, Args(int nColumnSource, int nColumnDestination, int nColumnCount, bool bClearSourceColumns), Args(nColumnSource, nColumnDestination, nColumnCount, bClearSourceColumns));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, CopyRows, Args(int nRowSource, int nRowDestination, int nRowCount, bool bClearSourceRows), Args(nRowSource, nRowDestination, nRowCount, bClearSourceRows));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, IsCellVisible, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, MakeCellVisible, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellRect, Args(int nColumn, int nRow, Math::Rectangle& rectCell), Args(nColumn, nRow, rectCell));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetFirstVisibleCell, Args(int& nColumn, int& nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetFirstVisibleCell, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetVisibleColumnAndRowCount, Args(float& fColumnCount, float& fRowCount), Args(fColumnCount, fRowCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, DoCursorHitTest, Args(float nCursorX, float nCursorY, UTFWin::CursorHitTestResult& cursorHitTestResult, UTFWin::CellCoordinates& cellCoordinates), Args(nCursorX, nCursorY, cursorHitTestResult, cellCoordinates));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellTextValue, Args(int nColumn, int nRow, wchar_t*& pText), Args(nColumn, nRow, pText));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellTextValue, Args(int nColumn, int nRow, const wchar_t* pText, unsigned int nTextLength, int nHowToMakeRoom, bool bEditable), Args(nColumn, nRow, pText, nTextLength, nHowToMakeRoom, bEditable));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellImage, Args(int nColumn, int nRow, Image*& pImage), Args(nColumn, nRow, pImage));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellImage, Args(int nColumn, int nRow, Image* pImage, int nHowToMakeRoom), Args(nColumn, nRow, pImage, nHowToMakeRoom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, GetCellWindow, Args(int nColumn, int nRow, IWindow*& pWindow), Args(nColumn, nRow, pWindow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellWindow, Args(int nColumn, int nRow, IWindow* pWindow, bool bStretchHorizontal, bool bStretchVertical, int nHowToMakeRoom), Args(nColumn, nRow, pWindow, bStretchHorizontal, bStretchVertical, nHowToMakeRoom));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, IsWindowACellWindow, Args(IWindow* pWindow, unsigned int nID, int* pColumn, int* pRow), Args(pWindow, nID, pColumn, pRow));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, ResizeColumnToFitCellWindows, Args(int nColumn, int nCount), Args(nColumn, nCount));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellExtraData, Args(int nColumn, int nRow, void* pData), Args(nColumn, nRow, pData));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, void*, GetCellExtraData, Args(int nColumn, int nRow), Args(nColumn, nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetColumnExtraData, Args(int nColumn, void* pData), Args(nColumn, pData));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, void*, GetColumnExtraData, Args(int nColumn), Args(nColumn));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetRowExtraData, Args(int nRow, void* pData), Args(nRow, pData));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, void*, GetRowExtraData, Args(int nRow), Args(nRow));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetExtraData, Args(void* pData), Args(pData));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, void*, GetExtraData);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, void*, GetCellExtraDataObject, Args(int nColumn, int nRow,uint32_t nIID), Args(nColumn, nRow, nIID));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetCellExtraDataObject, Args(int nColumn, int nRow, Object* pObject), Args(nColumn, nRow, pObject));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, void*, GetExtraDataObject, Args(uint32_t nIID), Args(nIID));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetExtraDataObject, Args(Object* pObject), Args(pObject));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SortColumn, Args(int nColumn, int nSortDirection, bool bOtherColumnsFollow, SortCallback* pSortFunction), Args(nColumn, nSortDirection, bOtherColumnsFollow, pSortFunction));
    auto_METHOD_VIRTUAL_VOID(WinGrid, IWinGrid, SortRow, Args(int nRow, int nSortDirection, bool bOtherRowsFollow, SortCallback* pSortFunction), Args(nRow, nSortDirection, bOtherRowsFollow, pSortFunction));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, FindCellText, Args(const wchar_t* pText, unsigned int nTextLength, int nColumnStart, int nColumnCount, int nRowStart, int nRowCount, int& nColumnResult, int& nRowResult, bool bMatchEntireCell), Args(pText, nTextLength, nColumnStart, nColumnCount, nRowStart, nRowCount, nColumnResult, nRowResult, bMatchEntireCell));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultAlignment, Args(int nAlignmentH, int nAlignmentV), Args(nAlignmentH, nAlignmentV));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultAlignmentByColumns, Args(int nColumnStart, int nCount, int nAlignmentH, int nAlignmentV), Args(nColumnStart, nCount, nAlignmentH, nAlignmentV));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetDefaultAlignmentByRows, Args(int nRowStart, int nCount, int nAlignmentH, int nAlignmentV), Args(nRowStart, nCount, nAlignmentH, nAlignmentV));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetScrollBarDrawableVertical, Args(IScrollbarDrawable* pScrollbarDrawable), Args(pScrollbarDrawable));
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetScrollBarDrawableHorizontal, Args(IScrollbarDrawable* pScrollbarDrawable), Args(pScrollbarDrawable));
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, IScrollbarDrawable*, GetScrollBarDrawableVertical);
    auto_METHOD_VIRTUAL_(WinGrid, IWinGrid, IScrollbarDrawable*, GetScrollBarDrawableHorizontal);
    auto_METHOD_VIRTUAL(WinGrid, IWinGrid, bool, SetSoundID, Args(int nSoundType, unsigned int nSoundID), Args(nSoundType, nSoundID));

}
