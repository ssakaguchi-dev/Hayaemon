//----------------------------------------------------------------------------
// App.cpp : �A�v���P�[�V�����̊Ǘ����s��
//----------------------------------------------------------------------------
#include <windows.h>
#include "App.h"
//----------------------------------------------------------------------------
// WinMain �֐�
//----------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	CApp app(nCmdShow);
	return app.Run();
}
//----------------------------------------------------------------------------
// �R���X�g���N�^
//----------------------------------------------------------------------------
CApp::CApp(const int & nCmdShow)
#if JP
	: m_nCmdShow(nCmdShow), m_cstrName(_T("���X�n��������")),
	  m_cstrAuthorName(_T("���[��")),
	  m_cstrAuthorEMail(_T("ryota.yamauch@gmail.com")),
	  m_cstrAuthorWebSiteName(_T("�t���[�\�t�g�J����")),
#else // JP
	: m_nCmdShow(nCmdShow), m_cstrName(_T("Hayaemon")),
	  m_cstrAuthorName(_T("Ryota")),
	  m_cstrAuthorEMail(_T("ryota.yamauch@gmail.com")),
	  m_cstrAuthorWebSiteName(_T("Free Software Development Station")),
#endif // JP
	  m_cstrVersionInfo(_T("2.76")),
	  m_cstrAuthorURL(_T("http://soft.edolfzoku.com/")),
	  m_wnd(*this)
{
	TCHAR pathName[MAX_PATH] = _T("");
	if(GetModuleFileName(NULL, pathName, MAX_PATH)) {
		TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR];
		_tsplitpath_s(pathName, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0,
					  NULL, 0);
		TCHAR path[_MAX_DRIVE + _MAX_DIR];
		lstrcpy(path, drive);
		lstrcat(path, dir);
		m_strPath = path;
	}

	char chPathName[MAX_PATH] = "";
	if(GetModuleFileNameA(NULL, chPathName, MAX_PATH)) {
		char chDrive[_MAX_DRIVE], chDir[_MAX_DIR];
		_splitpath_s(chPathName, chDrive, _MAX_DRIVE, chDir, _MAX_DIR, NULL,
			0, NULL, 0);
		char chPath[_MAX_DRIVE + _MAX_DIR];
		strcpy_s(chPath, chDrive);
		strcat_s(chPath, chDir);
		m_strPathA = chPath;
	}
}
//----------------------------------------------------------------------------
// �f�X�g���N�^
//----------------------------------------------------------------------------
CApp::~CApp()
{
}
//----------------------------------------------------------------------------
// ���d�N���̃`�F�b�N
//----------------------------------------------------------------------------
BOOL CApp::CheckDoubleRun()
{
	HANDLE hMutex = CreateMutex(NULL, false, m_cstrName.c_str());
	if(!hMutex) {
#if JP
		ShowError(_T("�~���[�e�b�N�X�I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B"));
#else // JP
		ShowError(_T("failed to create mutex object."));
#endif // JP
		return TRUE;
	}

	if(GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hWnd = FindWindow(_T("Hayaemon_MainWndClass"), NULL);
		if(hWnd && !IsWindowVisible(hWnd)) {
			while(IsWindow(hWnd)) {
				// �O��I���҂�
			}
		}

		TCHAR buf[255];
		GetPrivateProfileString(_T("Options"), _T("DoubleRun"), _T("1"), buf,
								255, (GetFilePath() +
								_T("Setting.ini")).c_str());
		if(_ttoi(buf)) return FALSE;

		if(hWnd) {
			if(IsIconic(hWnd)) ShowWindowAsync(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);

			// �A�C�R���Ƀh���b�v���ꂽ�t�@�C����n��
			if(__argc > 1) {
				COPYDATASTRUCT cd;
				cd.dwData = __argc;
				cd.cbData = 0;
				for (int i = 0; i < __argc; i++)
					cd.cbData += (_tcslen(__targv[i]) + 1) * sizeof(TCHAR);
				cd.lpData = *__targv;
				SendMessage(hWnd, WM_COPYDATA, 0L, (LPARAM)&cd);
			}
		}
		return TRUE;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
// ���s
//----------------------------------------------------------------------------
int CApp::Run()
{
	_tsetlocale(LC_ALL, _T(""));

	if(CheckDoubleRun()) return 0;
	if(!m_wnd.Create()) return 0;

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		if(!TranslateAccelerator(m_wnd, m_accelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
//----------------------------------------------------------------------------
// �G���[���o�͂��ďI��
//----------------------------------------------------------------------------
void CApp::ShowError(tstring strError)
{
#if JP
	tstring _strError = strError + _T("\n�A�v���P�[�V�������I�����܂��B");
	MessageBox(m_wnd, _strError.c_str(), _T("�G���["), MB_ICONERROR);
#else // JP
	tstring _strError = strError + _T("\nApplication exit.");
	MessageBox(m_wnd, _strError.c_str(), _T("Error"), MB_ICONERROR);
#endif // JP
	m_wnd.Destroy();
}
//----------------------------------------------------------------------------