//----------------------------------------------------------------------------
// Divider4_MainWnd.cpp : 区切り線その４の管理を行う
//----------------------------------------------------------------------------
#include <windows.h>
#include "../App.h"
#include "../Common/CommandList.h"
#include "Explorer.h"
#include "MainWnd.h"
#include "Menu_MainWnd.h"
#include "Divider4_MainWnd.h"
#include "PanSlider_MainWnd.h"
#include "Sound.h"
//----------------------------------------------------------------------------
// 高さを得る
//----------------------------------------------------------------------------
int CDivider4_MainWnd::GetHeight() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return CStatic::GetHeight() + m_rMainWnd.GetControlOffset();
	else return m_rMainWnd.GetPanSlider().GetHeight();
}
//----------------------------------------------------------------------------
// 縦位置を得る
//----------------------------------------------------------------------------
int CDivider4_MainWnd::GetTop() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return CStatic::GetTop();
	else return m_rMainWnd.GetPanSlider().GetTop();
}
//----------------------------------------------------------------------------
// 位置を再設定
//----------------------------------------------------------------------------
void CDivider4_MainWnd::ResetPos()
{
	POINT pt;
	pt.y = m_rMainWnd.GetPanSlider().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	SetPos(m_rMainWnd.GetExplorer().GetWidth(),
		pt.y + m_rMainWnd.GetPanSlider().GetHeight()
		+ m_rMainWnd.GetControlOffset());
}
//----------------------------------------------------------------------------
