// ==========================================================================
//		 					Class Specification : 
//						COXTreeSubItem & COXTreeItem
// ==========================================================================

// Header file : COXTreeItem.h

// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                         
// //////////////////////////////////////////////////////////////////////////
                         
#ifndef _COXTreeItem_H
#define _COXTreeItem_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include <afxtempl.h>

// subitem flags
// flags specifies which subitem options are set, can be
// any combination of the next:
//				OX_SUBITEM_IMAGE	-	there is image associated 
//										with the subitem
//				OX_SUBITEM_FONT		-	there is font set to be used 
//										to draw the subitem
//				OX_SUBITEM_COLOR	-	there is color set to be used 
//										to draw the subitem text
//				OX_SUBITEM_TEXT		- 	there is text set to be used 
//										as the subitem text
#define OX_SUBITEM_IMAGE	1
#define OX_SUBITEM_FONT		2
#define OX_SUBITEM_COLOR	4
#define OX_SUBITEM_TEXT		8

// tree item edit mode flags
#define OXET_EDIT			0
#define OXET_COMBO			1
#define OXET_CALENDAR		2
#define OXET_NOEDIT			3

struct COXTreeSubItem
{
	CString sItem;
	int nCol;
	UINT uFlags;
	UINT nImage;
	UINT m_nEllipsisFormat;
	CStringArray m_saTextEx;
	UINT m_uEditMode;

//	LOGFONT m_lf;
	CFont m_font;
	COLORREF m_clr;

	// --- In  :	nColumn		-	number of column in the tree control
	//				lpszItem	-	text to be used as the subitem's text
	// --- Out : 
	// --- Returns:
	// --- Effect : constructors
	COXTreeSubItem():nCol(-1),uFlags(0),m_uEditMode(OXET_EDIT),
		m_nEllipsisFormat(DT_END_ELLIPSIS){};
	COXTreeSubItem(int nColumn):nCol(nColumn),uFlags(0),m_uEditMode(OXET_EDIT),
		m_nEllipsisFormat(DT_END_ELLIPSIS) {};
	COXTreeSubItem(int nColumn,LPCTSTR lpszItem):nCol(nColumn),sItem(lpszItem),
		nImage(0),m_uEditMode(OXET_EDIT),uFlags(0),m_clr(0),
		m_nEllipsisFormat(DT_END_ELLIPSIS){ /*memset(&m_lf,0,sizeof(m_lf));*/}

    // Copy constructor
    COXTreeSubItem& operator=( const COXTreeSubItem& xtsi );

public:
	
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if there is image associated with the subitem
	// --- Effect : 
	inline BOOL HasImage()	{ return (BOOL)(uFlags & OX_SUBITEM_IMAGE);}

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if there is font associated with the subitem text
	// --- Effect : 
	inline BOOL HasFont()	{ return (BOOL)(uFlags & OX_SUBITEM_FONT);}

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if there is color associated with the subitem text
	// --- Effect : 
	inline BOOL HasColor()	{ return (BOOL)(uFlags & OX_SUBITEM_COLOR);}

	// --- In  :	saTextEx	-	extended strings depending of edit mode:
	//								array of strings that is used to fill combo box 
	//								(OXET_COMBO) 
	//								or to define min and max date for calendar control 
	//								(OXET_CALENDAR),
	//								if OXET_EDIT is used than this array is ignored
	// --- Out : 
	// --- Returns:
	// --- Effect : 
	void SetTextEx(CStringArray& saText);

	// --- In  :
	// --- Out : 
	// --- Returns:	array of strings using in extended edit modes
	//					as following:
	//					if EditMode is OXET_COMBO this array contain strings to fill
	//						in-place combobox
	//					if EditMode is OXET_CALENDAR this array contain min and max dates
	//						available to pick up throuth in-place date 
	//						picker (COXTreeCal class)
	// --- Effect: 
	CStringArray& GetTextEx();

	// --- In  :	
	// --- Out : 
	// --- Returns:	edit mode code as following:
	//				OXET_EDIT - plain edit 
	//				OXET_COMBO - in-place combobox
	//				OXET_CALENDAR	- in-place calendar (date picker)
	//				OXET_NOEDIT - editing is not allowed 
	// --- Effect : 
	UINT GetEditMode();

	// --- In  :	uMode		-	edit mode code as following:
	//								OXET_EDIT - plain edit 
	//								OXET_COMBO - in-place combobox
	//								OXET_CALENDAR	- in-place calendar (date picker)
	//								OXET_NOEDIT - editing is not allowed 
	//				saTextEx	-	extended strings depending of edit mode:
	//								array of strings that is used to fill combo box 
	//								(OXET_COMBO) 
	//								or to define min and max date for calendar control 
	//								(OXET_CALENDAR),
	//								if OXET_EDIT or OXET_NOEDIT is used than 
	//								this array is ignored
	// --- Out : 
	// --- Returns:
	// --- Effect : set the subitem's edit mode
	void SetEditMode(UINT uMode,CStringArray& saTextEx);


	// --- In:		clr - color for the subitem
	// --- Out : 
	// --- Returns: 
	// --- Effect :	set color for the subitem
	void SetColor(COLORREF clr);

	void SetDrawEllipsis(UINT nEllipsisFormat);
	// --- In  :	nEllipsisFormat	-	ellipsis format used to draw the subitem, 
	//									can be one of:
	//
	//									DT_END_ELLIPSIS		Replaces the end of the 
	//														item text with ellipses, 
	//														if necessary, so that the 
	//														result fits in the item
	//														rectangle	
	//									DT_PATH_ELLIPSIS	Replaces the middle of the 
	//														item text with ellipses, 
	//														if necessary, so that the 
	//														result fits in the item
	//														rectangle
	//									DT_WORD_ELLIPSIS	the same as 
	//														DT_END_ELLIPSIS
	// --- Out : 
	// --- Returns:	
	// --- Effect : set ellipsis format used when the subitem is being drawn
	
	inline UINT GetDrawEllipsis() { return m_nEllipsisFormat; }
	// --- In  :
	// --- Out : 
	// --- Returns:	ellipsis format used to draw the subitem, can be one of:
	//
	//				DT_END_ELLIPSIS		Replaces the end of the 
	//									item text with ellipses, 
	//									if necessary, so that the 
	//									result fits in the item
	//									rectangle	
	//				DT_PATH_ELLIPSIS	Replaces the middle of the 
	//									item text with ellipses, 
	//									if necessary, so that the 
	//									result fits in the item
	//									rectangle
	//				DT_WORD_ELLIPSIS	the same as 
	//									DT_END_ELLIPSIS
	// --- Effect : get ellipsis format used when the subitem is being drawn

	BOOL SetFont(CFont *pFont);
	// --- In  :	pFont - specifies the new font for drawing the subitem
	// --- Out : 
	// --- Returns:
	// --- Effect : set font to tree item
};


class COXTreeCtrl;//forward declaration

class OX_CLASS_DECL COXTreeItem
{
	friend class COXTreeCtrl;
public:
//	LOGFONT m_lf;
	CFont m_font;
	COLORREF m_clr;
	TV_ITEM m_tvi;
	UINT m_nEllipsisFormat;

	void SetTextEx(CStringArray& saText,int nCol=0);
	// --- In  :	saTextEx	-	extended strings depending of edit mode:
	//								array of strings that is used to fill combo box 
	//								(OXET_COMBO) 
	//								or to define min and max date for calendar control 
	//								(OXET_CALENDAR),
	//								if OXET_EDIT is used than this array is ignored
	//				nCol		-	column number
	// --- Out : 
	// --- Returns:
	// --- Effect : 

	CStringArray& GetTextEx(int nCol=0);
	// --- In  :	nCol		-	column number
	// --- Out : 
	// --- Returns:	array of strings using in extended edit modes
	//					as following:
	//					if EditMode is OXET_COMBO this array contain strings to fill
	//						in-place combobox
	//					if EditMode is OXET_CALENDAR this array contain min and max dates
	//						available to pick up throuth in-place date 
	//						picker (COXTreeCal class)
	// --- Effect: 
	
	UINT GetEditMode(int nCol=0);
	// --- In  :	nCol		-	column number
	// --- Out : 
	// --- Returns:	edit mode code as following:
	//				OXET_EDIT - plain edit 
	//				OXET_COMBO - in-place combobox
	//				OXET_CALENDAR	- in-place calendar (date picker)
	// --- Effect : 

	void SetEditMode(UINT uMode,CStringArray& saTextExt,int nCol=0);
	// --- In  :	uMode		-	edit mode code as following:
	//								OXET_EDIT - plain edit 
	//								OXET_COMBO - in-place combobox
	//								OXET_CALENDAR	- in-place calendar (date picker)
	//				saTextEx	-	extended strings depending of edit mode:
	//								array of strings that is used to fill combo box 
	//								(OXET_COMBO) 
	//								or to define min and max date for calendar control 
	//								(OXET_CALENDAR),
	//								if OXET_EDIT is used than this array is ignored
	//				nCol		-	column number
	// --- Out : 
	// --- Returns:
	// --- Effect : set the item's edit mode

	BOOL IsDisabled();
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if the item is disabled, or FALSE otherwise
	// --- Effect : 
	void SetDisabled(BOOL bDisable=TRUE);
	// --- In  :	bDisable	-	if TRUE if then the item will be disabled, 
	//								otherwise it will be enabled
	// --- Out : 
	// --- Returns:	
	// --- Effect : disable or enable the item

	CFont* GetItemFont(int nCol=0);
	// --- In  :	nCol	-	column number
	// --- Out : 
	// --- Returns:	pointer to the font used to draw the subitem text in the 
	//				specified nCol column
	// --- Effect : 
	BOOL SetFont(int nCol,CFont * pFont);
	// --- In  :	pFont	-	specifies the new font for drawing the subitem in the 
	//							specified nCol column
	//				nCol	-	column number
	// --- Out : 
	// --- Returns:
	// --- Effect : set font to tree subitem
	BOOL HasFont(int nCol);
	// --- In  :	nCol	-	column number
	// --- Out : 
	// --- Returns:	TRUE if there is font associated with the subitem text in the
	//				specified nCol column
	// --- Effect : 

	COLORREF GetItemColor(int nCol = 0);
	// --- In  :	nCol	-	column number
	// --- Out : 
	// --- Returns:	color used to draw the subitem text in the 
	//				specified nCol column
	// --- Effect : 
	void SetColor(COLORREF clr);
	// --- In  :	clr		-	specifies the new color for drawing the subitem text 
	//							in the specified nCol column
	//				nCol	-	column number
	// --- Out : 
	// --- Returns:
	// --- Effect : set color to tree subitem
	BOOL HasColor(int nCol=0);
	// --- In  :	nCol	-	column number
	// --- Out : 
	// --- Returns:	TRUE if there is specific color associated with the subitem text 
	//				in the specified nCol column
	// --- Effect : 
	
	UINT GetDrawEllipsis(int nCol = 0);
	// --- In  :	nCol	-	column number
	// --- Out : 
	// --- Returns:	ellipsis format used to draw the specified subitem, can be one of:
	//
	//				DT_END_ELLIPSIS		Replaces the end of the 
	//									item text with ellipses, 
	//									if necessary, so that the 
	//									result fits in the item
	//									rectangle	
	//				DT_PATH_ELLIPSIS	Replaces the middle of the 
	//									item text with ellipses, 
	//									if necessary, so that the 
	//									result fits in the item
	//									rectangle
	//				DT_WORD_ELLIPSIS	the same as 
	//									DT_END_ELLIPSIS
	// --- Effect : get ellipsis format used when the subitem is being drawn
	void SetDrawEllipsis(UINT nEllipsisFormat, int nCol=0);
	// --- In  :	nEllipsisFormat	-	ellipsis format used to draw the specified 
	//									subitem, can be one of:
	//
	//									DT_END_ELLIPSIS		Replaces the end of the 
	//														item text with ellipses, 
	//														if necessary, so that the 
	//														result fits in the item
	//														rectangle	
	//									DT_PATH_ELLIPSIS	Replaces the middle of the 
	//														item text with ellipses, 
	//														if necessary, so that the 
	//														result fits in the item
	//														rectangle
	//									DT_WORD_ELLIPSIS	the same as 
	//														DT_END_ELLIPSIS
	// --- Out : 
	// --- Returns:	
	// --- Effect : set ellipsis format used when the specified subitem is being drawn
	
	BOOL NeedDrawButton();
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if the button with plus or minus sign should be drawn before 
	//				the item, or FALSE otherwise
	// --- Effect : 
	
	BOOL IsVisible();
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if the item is visible, or FALSE otherwise
	// --- Effect : 
	
	COXTreeItem* GetPrevInTree();
	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to the previous item in the tree, or NULL if the item
	//				is the first one in the tree
	// --- Effect : 
	COXTreeItem* GetNextInTree();
	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to the next item in the tree, or NULL if the item
	//				is the last one in the tree
	// --- Effect : 
	
	void SetHidden(BOOL bHidden=TRUE);
	// --- In  :	bHidden	- if TRUE then item will be hidden, or unhidden otherwise
	// --- Out : 
	// --- Returns:	
	// --- Effect : hide or unhide the item
	BOOL IsHidden();
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if the item is hidden, or FALSAE otherwise
	// --- Effect : 
	BOOL IsLastUnhidden();
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if the item is last unhidden item in the tree, 
	//				or FALSAE otherwise
	// --- Effect : 
	
	int GetSubItemImage(int nCol);
	// --- In  :	nCol		-	column number (more than zero)
	// --- Out : 
	// --- Returns:	index for image associated with the specified subitem
	// --- Effect : 
	int GetItemImg(int nCol = 0, BOOL bSelImg = FALSE);
	// --- In  :	nCol		-	column number
	//				bSelImg		-	if TRUE then the index of selected image will be returned.
	// --- Out : 
	// --- Returns:	index for selected or normal image associated with the 
	//				specified subitem
	// --- Effect : 
	
	LPCTSTR GetSubItemText(int nCol);
	// --- In  :	nCol		-	column number (more than zero)
	// --- Out : 
	// --- Returns:	text associated with the specified subitem
	// --- Effect : 
	BOOL SetSubItemText(LPCTSTR lpszText,int nCol = 0);
	// --- In  :	nCol		-	column number (more than zero)
	//				lpszText	-	text to be set to the specified subitem
	// --- Out : 
	// --- Returns:	TRUE if success, or FALSE otherwise
	// --- Effect : 

	COXTreeSubItem* GetSubItem(int nCol);
	// --- In  :	nCol		-	column number (more than zero)
	// --- Out : 
	// --- Returns:	pointer to the subitem
	// --- Effect : 
	BOOL SetSubItem(int nCol, UINT uMask, LPCTSTR lpszText, int nImage=0, 
		CFont* pFont=NULL, COLORREF clr=0);
	// --- In  :	nCol		-	column number (more than zero)
	//				uFlags		-	one or more of following:
	//				OX_SUBITEM_IMAGE	- nImage parameter is valid
	//				OX_SUBITEM_FONT		- pFont parameter is valid
	//				OX_SUBITEM_COLOR	- clr parameter is valid
	//				OX_SUBITEM_TEXT		- lpszText parameter is valid
	//									subitem attributes
	//				lpszText	- text
	//				nImage		- image
	//				pFont		- font
	//				clr 		- color
	// --- Out : 
	// --- Returns:	TRUE if success, or FALSE otherwise
	// --- Effect : set specified attributes for subitem

	BOOL ItemHasChildren();
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if item has children, or FALSE otherwise
	// --- Effect : 
	UINT GetChildrenCount();
	// --- In  :	
	// --- Out : 
	// --- Returns:	the number of children
	// --- Effect : 
	int GetItemLevel();
	// --- In  :	
	// --- Out : 
	// --- Returns:	the item's level, where top items have 0 level
	// --- Effect : 

	BOOL Expand(UINT nCode, COXTreeCtrl *pCtrl);
	// --- In  :	nCode	-	A flag indicating the type of action to be taken. 
	//							This flag can have one of the following values:
	//							TVE_COLLAPSE		-	Collapses the list.
	//							TVE_COLLAPSERESET   -	Collapses the list and removes 
	//													the child items.
	//							TVE_EXPAND			-	Expands the list.
	//							TVE_TOGGLE			-	Collapses the list if it is 
	//													currently expanded or expands it 
	//													if it is currently collapsed.
	//				pCtrl	-	pointer to corresponding tree control
	// --- Out : 
	// --- Returns:	TRUE if success, or FALSE otherwise
	// --- Effect : set specified attributes for subitem
	inline BOOL IsExpanded(){ return m_bExpand;}
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if item is expanded, or FALSE otherwise
	// --- Effect : 
	inline BOOL IsExpandedOnce() { return m_bExpandedOnce; }
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if item's list of child items has been expanded at least once
	// --- Effect : Not implemented, always returns FALSE

	int GetSubitemsCount();
	// --- In  :	
	// --- Out : 
	// --- Returns:	number of subitems
	// --- Effect : 
	inline void SetExpand(BOOL bExpand=TRUE) {
		m_bExpand = bExpand;
		if(m_bExpand)
			m_bExpandedOnce=TRUE;
	}
	// --- In  :	bExpand		-	specifies whether will be set being expanded or not
	// --- Out : 
	// --- Returns:	
	// --- Effect : set/unset item being expanded. 
	void AddChild(COXTreeItem *pNewChild,COXTreeItem *pInsAfter=NULL);
	// --- In  :	pNewChild	-	pointer to the new item
	//				pInsAfter	-	pointer to the item to insert after
	// --- Out : 
	// --- Returns:	
	// --- Effect : add new child item 
	COXTreeItem* CopyChild(COXTreeItem* pChildToCopy, COXTreeItem* pInsAfter=NULL,
		BOOL bCopyDescendants=TRUE);
	// --- In  :	pChildToCopy	-	pointer to the item to copy
	//				pInsAfter		-	pointer to the item to insert after
	//				bCopyDescendants-	if TRUE then all pChildToCopy descendants
	//									will be copied too
	// --- Out : 
	// --- Returns:	
	// --- Effect : make copy of tree item and its descendants
	COXTreeItem* GetLastChild();
	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to the last child, or NULL if there is no child
	// --- Effect : 
	void Unlink();
	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect : remove item from the inner tree structure
	void DeleteChildren();
	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect : delete all children

	COXTreeItem* FindItemToInsertAfter(LPCTSTR lpszItem, BOOL bAscending=TRUE);
	// --- In  :	lpszItem	-	text of the new item
	//				bAscending	-	if TRUE then tree control is sorted ascendingly,
	//								or FALSE if tree control is sorted descendingly
	// --- Out : 
	// --- Returns:	pointer to the to insert new item after
	// --- Effect : 
	BOOL SortChildren(int nCol=0, BOOL bOnlyChildren=TRUE, BOOL bAscending=TRUE, 
		PFNTVCOMPARE lpfnCompare=NULL, LPARAM lParam=NULL);
	//				nCol		-	number of column
	//				bAscending	-	sort direction (TRUE - ascending, 
	//												FALSE - descending)
	//				bOnlyChildren	sort only direct children of the item
	//				lpfnCompare	-	if it's NULL then items will be compared using 
	//								string comparison, otherwise it is the
	//								pointer to an application-defined comparison
	//								function. It is called during a sort 
	//								operation each time the relative order of 
	//								two tree items needs to be compared. The 
	//								comparison function has the following form:
	//	
	//								int CALLBACK CompareFunc(LPARAM lParam1, 
	//														LPARAM lParam2, 
	//														LPARAM lParamSort);
	//
	//								The comparison function must return a 
	//								negative value if the first item should 
	//								precede the second, a positive value if the 
	//								first item should follow the second, or 
	//								zero if the two items are equivalent. The 
	//								lParam1 and lParam2 parameters correspond 
	//								to the lParam member of the TV_ITEM 
	//								structure for the two items being compared. 
	//								The lParamSort parameter corresponds to the 
	//								lParam argument of the SortChildren function.
	//				lParam		-	parameter that will be sent to lpfnCompare
	//								as lParamSort argument
	BOOL Compare(COXTreeItem* xti, int nCol=0, BOOL bAscending=TRUE);
	// --- In  :	xti			-	pointer to the item to compare to
	//				nCol		-	number of column to make comparison
	//				bAscending	-	sort direction (TRUE - ascending, 
	//												FALSE - descending)
	// --- Out : 
	// --- Returns:	TRUE if the item is "more" than xti
	// --- Effect : 
	void Swap(COXTreeItem* xti);
	// --- In  :	xti			-	pointer to the item to change position with
	// --- Out : 
	// --- Returns:	
	// --- Effect : swap items

	COXTreeItem();
	COXTreeItem(LPCTSTR lpszItem);
	// --- In  :	lpszItem	-	text to be used as the item's text
	// --- Out : 
	// --- Returns:
	// --- Effect : constructors

	~COXTreeItem();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : destructors

    // Copy constructor
    COXTreeItem& operator=( const COXTreeItem& xti );

protected:
	enum { XTIS_VISIBLE=1,XTIS_DISABLED=2,XTIS_HASCOLOR=4,XTIS_HASFONT=8};

	DWORD dwStateEx;
	BOOL m_bExpand;
	BOOL m_bExpandedOnce;
	COXTreeItem* pxParent;
	COXTreeItem* pxNext;
	COXTreeItem *pxPrev;
	COXTreeItem *pxFirstChild;
	CArray <COXTreeSubItem*,COXTreeSubItem*> m_Items;

	CStringArray m_saTextEx;
	UINT m_uEditMode;

	int GetChildOffset(COXTreeItem *pChild,BOOL bCalcHidden=FALSE);
	void Exp(COXTreeCtrl *pCtrl);
	void RemoveChildrenFromCtrl(COXTreeCtrl *pCtrl);
	int AddChildrenToCtrl(COXTreeCtrl *pCtrl,int startPos);

};

#endif  // _COXTreeItem_H

