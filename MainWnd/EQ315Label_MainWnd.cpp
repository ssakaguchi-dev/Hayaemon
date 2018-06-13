//----------------------------------------------------------------------------
// EQ315Label_MainWnd.cpp : イコライザ ( 315Hz ) 表示用ラベルの管理を行う
//----------------------------------------------------------------------------
#include <windows.h>
#include <TCHAR.H>
#include "../App.h"
#include "MainWnd.h"
#include "EQ315Label_MainWnd.h"
//----------------------------------------------------------------------------
// 作成
//----------------------------------------------------------------------------
BOOL CEQ315Label_MainWnd::Create()
{
	CStatic::Create(SS_NOTIFY);
	if(!m_hWnd) return FALSE;
	SetParent(m_rMainWnd);

	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	m_font.CreateIndirect(&ncm.lfMessageFont);
	SetFont(m_font, TRUE);

	SetText(_T("315 Hz : "));
	SetSizeToTextSize();

	m_edit.Create(m_hWnd);
	SendMessage(m_edit, EM_SETLIMITTEXT, 3, 0);
	m_edit.SetStyle(m_edit.GetStyle() | ES_RIGHT);
	m_edit.SetFont(m_font, TRUE);
	POINT pt;
	pt.x = m_rMainWnd.GetEQ12_5KLabel().GetEdit().GetLeft();
	ScreenToClient(m_rMainWnd.GetEQ12_5KLabel(), &pt);
	m_edit.SetPos(pt.x, 0);
	m_edit.SetText(_T("-10"));
	m_edit.SetSizeToTextSize();
	m_edit.Show(SW_SHOW);

	m_ud.Create(m_hWnd, m_edit);
	SendMessage(m_ud, UDM_SETPOS32, 0, 0);
	SendMessage(m_ud, UDM_SETRANGE32, -30, 30);
	m_ud.Show(SW_SHOW);

	m_edit.SetSize(m_edit.GetWidth() + m_ud.GetWidth(), m_edit.GetHeight());
	m_ud.SetPos(pt.x + m_edit.GetWidth(), 0);

	SetSize(pt.x + m_edit.GetWidth() + m_ud.GetWidth(), m_edit.GetHeight());
	SetEQ315(0);

	ResetPos();

	return TRUE;
}
//----------------------------------------------------------------------------
// 高さを得る
//----------------------------------------------------------------------------
int CEQ315Label_MainWnd::GetHeight() const
{
	if(!m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return m_rMainWnd.GetPanLabel().GetHeight();
	else if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ315))
		return CStatic::GetHeight();
	else return m_rMainWnd.GetEQ250Label().GetHeight();
}
//----------------------------------------------------------------------------
// 縦位置を得る
//----------------------------------------------------------------------------
int CEQ315Label_MainWnd::GetTop() const
{
	if(!m_rMainWnd.GetMenu().IsItemChecked(ID_EQ))
		return m_rMainWnd.GetPanLabel().GetTop();
	else if(m_rMainWnd.GetMenu().IsItemChecked(ID_EQ315))
		return CStatic::GetTop();
	else return m_rMainWnd.GetEQ250Label().GetTop();
}
//----------------------------------------------------------------------------
// 表示する周波数を表示
//----------------------------------------------------------------------------
void CEQ315Label_MainWnd::SetEQ315(int nEQ315)
{
	TCHAR text[13];
	_stprintf_s(text, _T("%d"), nEQ315);
	m_edit.SetText(text);
	InvalidateRect(NULL, TRUE);
	SendMessage(m_ud, UDM_SETPOS32, 0, nEQ315);
}
//----------------------------------------------------------------------------
// 位置を再設定
//----------------------------------------------------------------------------
void CEQ315Label_MainWnd::ResetPos()
{
	POINT pt;
	pt.y = m_rMainWnd.GetEQ250Label().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	int nEQLabelBottom = pt.y + m_rMainWnd.GetEQ250Label().GetHeight();
	pt.y = m_rMainWnd.GetEQ250Slider().GetTop();
	ScreenToClient(m_rMainWnd, &pt);
	int nEQSliderBottom = pt.y + m_rMainWnd.GetEQ250Slider().GetHeight();
	int nBigger = nEQLabelBottom > nEQSliderBottom ? nEQLabelBottom
		: nEQSliderBottom;
	int nLeft = m_rMainWnd.GetControlOffset();
	if(m_rMainWnd.GetMenu().IsItemChecked(ID_EXPLORER))
		nLeft += m_rMainWnd.GetExplorer().GetWidth()
			+ m_rMainWnd.GetControlOffset();
	SetPos(nLeft, nBigger);
}
//----------------------------------------------------------------------------
// コマンド
//----------------------------------------------------------------------------
void CEQ315Label_MainWnd::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
	if(hwndCtl == m_edit) {
		if(codeNotify == EN_SETFOCUS) {
			PostMessage(m_edit, WM_LBUTTONUP, 0, 0);
			PostMessage(m_edit, EM_SETSEL, 0, -1);
			return;
		}
		else if(codeNotify == EN_CHANGE) {
			int n = _ttoi(m_edit.GetText().c_str());
			if(-30 <= n && n <= 30) {
				m_rMainWnd.GetEQ315Slider().SetThumbPos(n, TRUE);
				m_rMainWnd.SetEQ315(n);
				SendMessage(m_ud, UDM_SETPOS32, 0, n);
			}
		}
		else if(codeNotify == EN_KILLFOCUS) {
			int n = _ttoi(m_edit.GetText().c_str());
			if(n < -30) n = -30;
			else if(n > 30) n = 30;
			SetEQ315(n);
			m_rMainWnd.GetEQ315Slider().SetThumbPos(n, TRUE);
			m_rMainWnd.SetEQ315(n);
		}
	}
	else m_rClickMenu.OnCommand(id, hwndCtl, codeNotify);
	CStatic::OnCommand(id, hwndCtl, codeNotify);
}
//----------------------------------------------------------------------------
// コンテキストメニュー
//----------------------------------------------------------------------------
LRESULT CEQ315Label_MainWnd::OnContextMenu(HWND hwnd, int xPos, int yPos)
{
	m_rClickMenu.Create();
	POINT pt;
	GetCursorPos(&pt);
	SetForegroundWindow(m_hWnd);
	TrackPopupMenu((HMENU)m_rClickMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x,
		pt.y, 0, m_hWnd, NULL);
	return CStatic::OnContextMenu(hwnd, xPos, yPos);
}
//----------------------------------------------------------------------------
void CEQ315Label_MainWnd::OnVScroll(HWND hwndCtl, UINT code, int pos)
{
	SetEQ315(pos);
	PostMessage(m_edit, WM_LBUTTONUP, 0, 0);
	PostMessage(m_edit, EM_SETSEL, 0, -1);

	CStatic::OnVScroll(hwndCtl, code, pos);
}
//----------------------------------------------------------------------------
