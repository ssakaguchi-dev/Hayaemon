//----------------------------------------------------------------------------
// EQ315Slider_MainWnd.cpp : イコライザ ( 315Hz ) 設定用スライダの管理を行う
//----------------------------------------------------------------------------
#include <windows.h>
#include "../App.h"
#include "../Common/CommandList.h"
#include "MainWnd.h"
#include "Menu_MainWnd.h"
#include "EQ250Slider_MainWnd.h"
#include "EQ315Label_MainWnd.h"
#include "EQ315Slider_MainWnd.h"
#include "PanSlider_MainWnd.h"
//----------------------------------------------------------------------------
// 作成
//----------------------------------------------------------------------------
BOOL CEQ315Slider_MainWnd::Create()
{
	CSliderCtrl::Create(m_rMainWnd);
	if(!m_hWnd) return FALSE;

	SetStyle(GetStyle() | WS_TABSTOP | TBS_AUTOTICKS | TBS_HORZ | TBS_NOTICKS);

	SetRangeMin(-30);
	SetRangeMax(30);
	SetLineSize(1);
	SetPageSize(1);
	SetThumbPos(0);

	ResetPos();
	ResetSize();

	return TRUE;
}
//----------------------------------------------------------------------------
// 高さを得る
//----------------------------------------------------------------------------
int CEQ315Slider_MainWnd::GetHeight() const
{
	if(!m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return m_rMainWnd.GetPanSlider().GetHeight();
	else if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ315))
		return CSliderCtrl::GetHeight();
	else return m_rMainWnd.GetEQ250Slider().GetHeight();
}
//----------------------------------------------------------------------------
// 縦位置を得る
//----------------------------------------------------------------------------
int CEQ315Slider_MainWnd::GetTop() const
{
	if(!m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return m_rMainWnd.GetPanSlider().GetTop();
	else if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ315))
		return CSliderCtrl::GetTop();
	else return m_rMainWnd.GetEQ250Slider().GetTop();
}
//----------------------------------------------------------------------------
// 位置の再設定
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::ResetPos()
{
	POINT pt;
	pt.x = m_rMainWnd.GetEQ315Label().GetLeft();
	pt.y = m_rMainWnd.GetEQ315Label().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	int nLeft = pt.x + m_rMainWnd.GetEQ315Label().GetWidth()
		+ m_rMainWnd.GetControlOffset();
	SetPos(nLeft, pt.y);
}
//----------------------------------------------------------------------------
// サイズの再設定
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::ResetSize()
{
	POINT pt;
	pt.x = GetLeft();
	ScreenToClient(m_rMainWnd, &pt);
	int nWidth = m_rMainWnd.GetClientWidth() - pt.x;
	SetSize(nWidth, (int)(GetSystemMetrics(SM_CYHSCROLL) * 1.5));
}
//----------------------------------------------------------------------------
// コマンド
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
	if(!hwndCtl) m_rClickMenu.OnCommand(id, hwndCtl, codeNotify);
	CSliderCtrl::OnCommand(id, hwndCtl, codeNotify);
}
//----------------------------------------------------------------------------
// スクロールされた
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::OnHScroll(HWND hwndCtl, UINT code, int pos)
{
	LONG trackpos = GetThumbPos();
	m_rMainWnd.SetEQ315((int)trackpos);
	m_rMainWnd.GetEQ315Label().SetEQ315(trackpos);
}
//----------------------------------------------------------------------------
// キーボードが押された
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::OnKeyDown(UINT vk, int cRepeat, UINT flags)
{
	if(vk == VK_TAB) {
		if(GetKeyState(VK_CONTROL) < 0) m_rMainWnd.SetFocusNextTab();
		else m_rMainWnd.SetFocusNextControl();
	}
	else if(vk == VK_HOME) {
		SetThumbPos(0);
		m_rMainWnd.SetEQ315(0);
		m_rMainWnd.GetEQ315Label().SetEQ315(0);
		return;
	}
	else if(vk == VK_UP) vk = VK_DOWN;
	else if(vk == VK_DOWN) vk = VK_UP;
	else if(vk == VK_PRIOR) vk = VK_NEXT;
	else if(vk == VK_NEXT) vk = VK_PRIOR;
	CSliderCtrl::OnKeyDown(vk, cRepeat, flags);
}
//----------------------------------------------------------------------------
// クリックされた
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::OnLButtonDown(int x, int y, UINT keyFlags)
{
	RECT rc;
	SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)&rc);
	if(rc.left < x && x < rc.right &&
		rc.top < y && y < rc.bottom) {
		if(GetTickCount() - dwThumbClickTime <= GetDoubleClickTime()) {
			SetThumbPos(0);
			m_rMainWnd.GetEQ315Label().SetEQ315(0);
			m_rMainWnd.SetEQ315(0);
			return;
		}
		else dwThumbClickTime = GetTickCount();
	}
	CSliderCtrl::OnLButtonDown(x, y, keyFlags);
}
//----------------------------------------------------------------------------
// マウスホイールがスクロールされた
//----------------------------------------------------------------------------
BOOL CEQ315Slider_MainWnd::OnMouseWheel(UINT nFlags, int zDelta, POINTS pt)
{
	tstring strEQ = m_rMainWnd.GetEQ315Label().GetEdit().GetText();
	int n = _ttoi(strEQ.c_str());
	if(zDelta >= 0) n++;
	else n--;
	int nMin = GetRangeMin(), nMax = GetRangeMax();
	if(n < nMin) n = nMin;
	if(n > nMax) n = nMax;
	m_rMainWnd.SetEQ315(n);
	m_rMainWnd.GetEQ315Label().SetEQ315(n);
	return FALSE;
}
//----------------------------------------------------------------------------
// 右クリックが離された
//----------------------------------------------------------------------------
void CEQ315Slider_MainWnd::OnRButtonUp(int x, int y, UINT keyFlags)
{
	m_rClickMenu.Create();
	POINT pt;
	GetCursorPos(&pt);
	SetForegroundWindow(m_hWnd);
	TrackPopupMenu((HMENU)m_rClickMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x,
		pt.y, 0, m_hWnd, NULL);

	CSliderCtrl::OnRButtonUp(x, y, keyFlags);
}
//----------------------------------------------------------------------------
