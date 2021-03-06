//----------------------------------------------------------------------------
// Divider3_MainWnd.cpp : 区切り線その３の管理を行う
//----------------------------------------------------------------------------
#include <windows.h>
#include "../App.h"
#include "../Common/CommandList.h"
#include "Explorer.h"
#include "MainWnd.h"
#include "Menu_MainWnd.h"
#include "Divider3_MainWnd.h"
#include "PitchSlider_MainWnd.h"
//----------------------------------------------------------------------------
// 高さを得る
//----------------------------------------------------------------------------
int CDivider3_MainWnd::GetHeight() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_VOLUME) || 
		m_rMainWnd.GetMenu().IsItemChecked(ID_PAN))
		return CStatic::GetHeight() + m_rMainWnd.GetControlOffset();
	else return m_rMainWnd.GetPitchSlider().GetHeight();
}
//----------------------------------------------------------------------------
// 縦位置を得る
//----------------------------------------------------------------------------
int CDivider3_MainWnd::GetTop() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_VOLUME) || 
		m_rMainWnd.GetMenu().IsItemChecked(ID_PAN))
		return CStatic::GetTop();
	else return m_rMainWnd.GetPitchSlider().GetTop();
}
//----------------------------------------------------------------------------
// 位置を再設定
//----------------------------------------------------------------------------
void CDivider3_MainWnd::ResetPos()
{
	POINT pt;
	pt.y = m_rMainWnd.GetPitchSlider().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	SetPos(m_rMainWnd.GetExplorer().GetWidth(),
		pt.y + m_rMainWnd.GetPitchSlider().GetHeight()
		+ m_rMainWnd.GetControlOffset());
}
//----------------------------------------------------------------------------
