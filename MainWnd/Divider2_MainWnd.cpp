//----------------------------------------------------------------------------
// Divider2_MainWnd.cpp : 区切り線その２の管理を行う
//----------------------------------------------------------------------------
#include <windows.h>
#include "../App.h"
#include "../Common/CommandList.h"
#include "MainWnd.h"
#include "Menu_MainWnd.h"
#include "Divider2_MainWnd.h"
#include "TimeSlider_MainWnd.h"
//----------------------------------------------------------------------------
// 作成
//----------------------------------------------------------------------------
BOOL CDivider2_MainWnd::Create()
{
	CStatic::Create(SS_SUNKEN);
	if(!m_hWnd) return FALSE;
	SetParent(m_rMainWnd);
	ResetPos();
	ResetSize();

	return TRUE;
}
//----------------------------------------------------------------------------
// 高さを得る
//----------------------------------------------------------------------------
int CDivider2_MainWnd::GetHeight() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_SPEED) || 
		m_rMainWnd.GetMenu().IsItemChecked(ID_FREQ) ||
		m_rMainWnd.GetMenu().IsItemChecked(ID_PITCH))
		return CStatic::GetHeight() + m_rMainWnd.GetControlOffset();
	else return m_rMainWnd.GetTimeSlider().GetHeight();
}
//----------------------------------------------------------------------------
// 縦位置を得る
//----------------------------------------------------------------------------
int CDivider2_MainWnd::GetTop() const
{
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_SPEED) || 
		m_rMainWnd.GetMenu().IsItemChecked(ID_FREQ) ||
		m_rMainWnd.GetMenu().IsItemChecked(ID_PITCH))
		return CStatic::GetTop();
	else return m_rMainWnd.GetTimeSlider().GetTop();
}
//----------------------------------------------------------------------------
// 位置を再設定
//----------------------------------------------------------------------------
void CDivider2_MainWnd::ResetPos()
{
	POINT pt;
	pt.y = m_rMainWnd.GetTimeSlider().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	SetPos(0, pt.y + m_rMainWnd.GetTimeSlider().GetHeight()
			+ m_rMainWnd.GetControlOffset());
}
//----------------------------------------------------------------------------
