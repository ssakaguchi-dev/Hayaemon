﻿//----------------------------------------------------------------------------
// Sound.cpp : 音の再生管理
//----------------------------------------------------------------------------
#include "Sound.h"
#include <algorithm>
#include <cctype>
#include "../Common/Define.h"
#include "MainWnd.h"
//----------------------------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------------------------
CSound::CSound(CMainWnd & mainWnd, BOOL bMainStream)
	: m_rMainWnd(mainWnd), m_bLoop(FALSE), m_nLoopPosA(0), m_nLoopPosB(0),
		m_bABLoopA(FALSE), m_bABLoopB(FALSE), m_nCurFile(0), m_peak(0),
		m_bMainStream(bMainStream), m_hFx20Hz(0), m_hFx25Hz(0), m_hFx31_5Hz(0),
		m_hFx40Hz(0), m_hFx50Hz(0), m_hFx63Hz(0), m_hFx80Hz(0), m_hFx100Hz(0),
		m_hFx125Hz(0), m_hFx160Hz(0), m_hFx200Hz(0), m_hFx250Hz(0), m_hFx315Hz(0),
		m_hFx400Hz(0), m_hFx500Hz(0), m_hFx630Hz(0), m_hFx800Hz(0), m_hFx1KHz(0),
		m_hFx1_25KHz(0), m_hFx1_6KHz(0), m_hFx2KHz(0), m_hFx2_5KHz(0),
		m_hFx3_15KHz(0), m_hFx4KHz(0), m_hFx5KHz(0), m_hFx6_3KHz(0), m_hFx8KHz(0),
		m_hFx10KHz(0), m_hFx12_5KHz(0), m_hFx16KHz(0), m_hFx20KHz(0),
		m_hFxReverb(0), m_hFx3DReverb(0), m_hFxDelay(0), m_hFxChorus(0),
		m_hFxVolume(0),
		m_hFx20Hz_2(0), m_hFx25Hz_2(0), m_hFx31_5Hz_2(0), m_hFx40Hz_2(0),
		m_hFx50Hz_2(0), m_hFx63Hz_2(0), m_hFx80Hz_2(0), m_hFx100Hz_2(0),
		m_hFx125Hz_2(0), m_hFx160Hz_2(0), m_hFx200Hz_2(0), m_hFx250Hz_2(0),
		m_hFx315Hz_2(0), m_hFx400Hz_2(0), m_hFx500Hz_2(0), m_hFx630Hz_2(0),
		m_hFx800Hz_2(0), m_hFx1KHz_2(0), m_hFx1_25KHz_2(0), m_hFx1_6KHz_2(0),
		m_hFx2KHz_2(0), m_hFx2_5KHz_2(0), m_hFx3_15KHz_2(0), m_hFx4KHz_2(0),
		m_hFx5KHz_2(0), m_hFx6_3KHz_2(0), m_hFx8KHz_2(0), m_hFx10KHz_2(0),
		m_hFx12_5KHz_2(0), m_hFx16KHz_2(0), m_hFx20KHz_2(0), m_hMonoralDsp(0),
		m_hVocalCancelDsp(0), m_hOnlyLeftDsp(0), m_hOnlyRightDsp(0),
		m_hChangeLRDsp(0), m_hNormalizeDsp(0)
{
	// BASS_FXSetParametersでボリュームを変更しようとするだけでは、bass_fxが
	// ロードされないので、明示的にbass_fxの関数を実行する。
	auto v = BASS_FX_GetVersion();
}
//----------------------------------------------------------------------------
// マーカーの追加
//----------------------------------------------------------------------------
void CSound::AddMarker(QWORD nPos)
{
	m_arrayMarker.push_back(nPos);
	std::sort(m_arrayMarker.begin(), m_arrayMarker.end());
}
//----------------------------------------------------------------------------
// マーカーの位置を変更
//----------------------------------------------------------------------------
int CSound::ChangeMarkerPos(int nMarker, QWORD nPos)
{
	m_arrayMarker[nMarker] = nPos;
	std::sort(m_arrayMarker.begin(), m_arrayMarker.end());
	for(int i = 0; i < (int)m_arrayMarker.size(); i++) {
		if(m_arrayMarker[i] == nPos) return i;
	}
	return 0;
}
//----------------------------------------------------------------------------
// マーカーの削除
//----------------------------------------------------------------------------
void CSound::EraseMarker(int nMarker)
{
	m_arrayMarker.erase(m_arrayMarker.begin() + nMarker);
}
//----------------------------------------------------------------------------
// ファイルの読み込み
//----------------------------------------------------------------------------
BOOL CSound::StreamCreateFile(LPCTSTR lpFilePath, BOOL bDecode, int nCount)
{
	BOOL bRet = FALSE;
	if(!m_bMainStream) {
		bRet = CBassFx::StreamCreateFile(lpFilePath);
		TempoCreate(bDecode);
		m_strCurFile = lpFilePath;
		return bRet;
	}

	tstring ext;
	tstring filePath(lpFilePath);
	auto pos = filePath.rfind('.');
	if(pos != tstring::npos) {
		ext = filePath.substr(pos);
		std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);
	}

	if(ext == _T(".nsf"))
		return FALSE;

	if(ext == _T(".avi") || ext == _T(".wmv") ||
		 ext == _T(".mp4") || ext == _T(".mkv") ||
		 ext == _T(".flv")) {
		StreamFree();
		return FALSE;
	}

	if(!bRet) {
		bRet = CBassFx::StreamCreateFile(lpFilePath);
		ReverseCreate();
		TempoCreate(bDecode);
		m_strCurFile = lpFilePath;
		m_hFx20Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx25Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx31_5Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx40Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx50Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx63Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx80Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx100Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx125Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx160Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx200Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx250Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx315Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx400Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx500Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx630Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx800Hz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1_25KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1_6KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx2KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx2_5KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx3_15KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx4KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx5KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx6_3KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx8KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx10KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx12_5KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx16KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx20KHz = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx20Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx25Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx31_5Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx40Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx50Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx63Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx80Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx100Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx125Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx160Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx200Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx250Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx315Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx400Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx500Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx630Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx800Hz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1_25KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx1_6KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx2KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx2_5KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx3_15KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx4KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx5KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx6_3KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx8KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx10KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx12_5KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx16KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFx20KHz_2 = ChannelSetFX(BASS_FX_BFX_PEAKEQ, 0);
		m_hFxVolume = ChannelSetFX(BASS_FX_BFX_VOLUME, 1);
	}
	return bRet;
}
//----------------------------------------------------------------------------
// モノラル化の設定
//----------------------------------------------------------------------------
void CSound::SetMonoral(BOOL bMonoral)
{
	if(bMonoral)
		m_hMonoralDsp = BASS_ChannelSetDSP(m_hStream, &Monoral, 0, 1);
	else BASS_ChannelRemoveDSP(m_hStream, m_hMonoralDsp);
}
//----------------------------------------------------------------------------
// モノラル化用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::Monoral(HDSP handle, DWORD channel,
	void *buffer, DWORD length, void *user)
{
	float *data = (float*)buffer;
	int max = length / 4;
	for(int a = 0; a < max; a += 2)
		data[a] = data[a + 1] = ((data[a]) + data[a + 1]) * 0.5f;
}
//----------------------------------------------------------------------------
// ボーカルキャンセルの設定
//----------------------------------------------------------------------------
void CSound::SetVocalCancel(BOOL bVocalCancel)
{
	if(bVocalCancel)
		m_hVocalCancelDsp = BASS_ChannelSetDSP(m_hStream, &VocalCancel, 0, 1);
	else BASS_ChannelRemoveDSP(m_hStream, m_hVocalCancelDsp);
}
//----------------------------------------------------------------------------
// ボーカルキャンセル用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::VocalCancel(HDSP handle, DWORD channel,
	void *buffer, DWORD length, void *user)
{
	float *data = (float*)buffer;
	int max = length / 4;
	for(int a = 0; a < max; a += 2)
		data[a] = data[a + 1] = ((-data[a]) + data[a + 1]) * 0.5f;
}
//----------------------------------------------------------------------------
// 左右入れ替えの設定
//----------------------------------------------------------------------------
void CSound::SetChangeLR(BOOL bChangeLR)
{
	if(bChangeLR)
		m_hChangeLRDsp = BASS_ChannelSetDSP(m_hStream, &ChangeLR, 0, 2);
	else BASS_ChannelRemoveDSP(m_hStream, m_hChangeLRDsp);
}
//----------------------------------------------------------------------------
// 左右入れ替え用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::ChangeLR(HDSP handle, DWORD channel,
	void *buffer, DWORD length, void *user)
{
	float *data = (float*)buffer;
	int max = length / 4;
	float f;
	for(int a = 0; a < max; a += 2) {
		f = data[a];
		data[a] = data[a + 1];
		data[a + 1] = f;
	}
}
//----------------------------------------------------------------------------
// 左のみ再生の設定
//----------------------------------------------------------------------------
void CSound::SetOnlyLeft(BOOL bOnlyLeft)
{
	if(bOnlyLeft)
		m_hOnlyLeftDsp = BASS_ChannelSetDSP(m_hStream, &OnlyLeft, 0, 2);
	else BASS_ChannelRemoveDSP(m_hStream, m_hOnlyLeftDsp);
}
//----------------------------------------------------------------------------
// 左のみ再生用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::OnlyLeft(HDSP handle, DWORD channel,
	void *buffer, DWORD length, void *user)
{
	float *data = (float*)buffer;
	int max = length / 4;
	for(int a = 0; a < max; a += 2) data[a + 1] = data[a];
}
//----------------------------------------------------------------------------
// 右のみ再生の設定
//----------------------------------------------------------------------------
void CSound::SetOnlyRight(BOOL bOnlyRight)
{
	if(bOnlyRight)
		m_hOnlyRightDsp = BASS_ChannelSetDSP(m_hStream, &OnlyRight, 0, 2);
	else BASS_ChannelRemoveDSP(m_hStream, m_hOnlyRightDsp);
}
//----------------------------------------------------------------------------
// 右のみ再生用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::OnlyRight(HDSP handle, DWORD channel,
	void *buffer, DWORD length, void *user)
{
	float *data = (float*)buffer;
	int max = length / 4;
	for(int a = 0; a < max; a += 2) data[a] = data[a + 1];
}
//----------------------------------------------------------------------------
// ノーマライズの設定
//----------------------------------------------------------------------------
void CSound::SetNormalize(BOOL bNormalize)
{
	if(bNormalize) {
		HSTREAM decoder = BASS_StreamCreateFile(FALSE, m_strCurFile.c_str(), 0,
			0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE | BASS_IF_UNICODE);
		if(!decoder) decoder = BASS_APE_StreamCreateFile(FALSE,
			m_strCurFile.c_str(), 0, 0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_IF_UNICODE);
		if(!decoder) decoder = BASS_CD_StreamCreateFile(
			(char*)m_strCurFile.c_str(),
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_IF_UNICODE);
		if(!decoder) decoder = BASS_FLAC_StreamCreateFile(FALSE,
			m_strCurFile.c_str(), 0, 0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_IF_UNICODE);
		if(!decoder) decoder = BASS_AAC_StreamCreateFile(FALSE,
			m_strCurFile.c_str(), 0, 0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_IF_UNICODE);
		if(!decoder) decoder = BASS_MP4_StreamCreateFile(FALSE,
			m_strCurFile.c_str(), 0, 0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_IF_UNICODE);
		BASS_ChannelSetPosition(decoder, 0, BASS_POS_DECODETO);
		m_peak = 0;
		while(BASS_ChannelIsActive(decoder)) {
			DWORD level = BASS_ChannelGetLevel(decoder);
			int left = LOWORD(level);
			int right = HIWORD(level);
			if(m_peak < left) m_peak = left;
			if(m_peak < right) m_peak = right;
		}
		BASS_StreamFree(decoder);
		if(m_peak > 32767) m_peak = 32767;

		m_hNormalizeDsp = BASS_ChannelSetDSP(m_hStream, &Normalize, &m_peak,
											 3);
	}
	else BASS_ChannelRemoveDSP(m_hStream, m_hNormalizeDsp);
}
//----------------------------------------------------------------------------
// ノーマライズ用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::Normalize(HDSP handle, DWORD channel,
									   void *buffer, DWORD length, void *user)
{
	int *peak = (int*)user;
	float *data = (float*)buffer;
	int max = length / 4;
	for(int a = 0; a < max; a++) data[a] = data[a] * 32767 / *peak;
}
//----------------------------------------------------------------------------
// 再生
//----------------------------------------------------------------------------
BOOL CSound::ChannelPlay()
{
	ChannelRemoveSync();
	if(m_bABLoopB && m_nLoopPosB < ChannelGetLength())
		ChannelSetSync(BASS_SYNC_POS, m_nLoopPosB, LoopSyncProc,
						(DWORD *)this);
	else ChannelSetSync(BASS_SYNC_END, 0, LoopSyncProc, (DWORD *)this);
	return CBass::ChannelPlay();
}
//----------------------------------------------------------------------------
// イコライザ ( 20Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ20(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx20Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx20Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 25Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ25(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx25Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx25Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx25Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx25Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx25Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx25Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 31.5Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ31_5(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx31_5Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx31_5Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx31_5Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx31_5Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx31_5Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx31_5Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 40Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ40(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx40Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx40Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx40Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx40Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx40Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx40Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 50Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ50(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx50Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx50Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx50Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx50Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx50Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx50Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 63Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ63(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx63Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx63Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx63Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx63Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx63Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx63Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 80Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ80(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx80Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx80Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx80Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx80Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx80Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx80Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 100Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ100(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx100Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx100Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx100Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx100Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx100Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx100Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 125Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ125(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx125Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx125Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx125Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx125Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx125Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx125Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 160Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ160(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx160Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx160Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx160Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx160Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx160Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx160Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 200Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ200(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx200Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx200Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx200Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx200Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx200Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx200Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 250Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ250(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx250Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx250Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx250Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx250Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx250Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx250Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 315Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ315(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx315Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx315Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx315Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx315Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx315Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx315Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 400Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ400(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx400Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx400Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx400Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx400Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx400Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx400Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 500Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ500(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx500Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx500Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx500Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx500Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx500Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx500Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 630Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ630(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx630Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx630Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx630Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx630Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx630Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx630Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 800Hz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ800(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx800Hz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx800Hz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx800Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx800Hz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx800Hz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx800Hz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 1KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ1K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 1.25KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ1_25K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1_25KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1_25KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_25KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_25KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_25KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_25KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 1.6KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ1_6K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1_6KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx1_6KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_6KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_6KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_6KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx1_6KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 2KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ2K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx2KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx2KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 2.5KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ2_5K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx2_5KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx2_5KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2_5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2_5KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2_5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx2_5KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 3.15KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ3_15K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx3_15KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx3_15KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx3_15KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx3_15KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx3_15KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx3_15KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 4KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ4K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx4KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx4KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx4KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx4KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx4KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx4KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 5KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ5K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx5KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx5KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx5KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx5KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 6.3KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ6_3K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx6_3KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx6_3KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx6_3KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx6_3KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx6_3KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx6_3KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 8KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ8K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx8KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx8KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx8KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx8KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx8KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx8KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 10KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ10K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx10KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx10KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx10KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx10KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx10KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx10KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 12.5KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ12_5K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx12_5KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx12_5KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx12_5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx12_5KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx12_5KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx12_5KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 16KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ16K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx16KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx16KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx16KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx16KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx16KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx16KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// イコライザ ( 20KHz ) の設定
//----------------------------------------------------------------------------
void CSound::SetEQ20K(float fCenter, float fBandwidth, float fGain)
{
	if(-15.0f <= fGain && fGain <= 15.0f) { 
		BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, fGain,
			BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx20KHz, &p);
		BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, 0.0f, BASS_BFX_CHANALL};
		BASS_FXSetParameters(m_hFx20KHz_2, &p2);
	}
	else {
		if(fGain < -15.0f) {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, -15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain + 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20KHz_2, &p2);
		}
		else {
			BASS_BFX_PEAKEQ p = {0, fBandwidth, 0.0f, fCenter, 15.0f,
				BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20KHz, &p);
			BASS_BFX_PEAKEQ p2 = {0, fBandwidth, 0.0f, fCenter, fGain - 15.0f, BASS_BFX_CHANALL};
			BASS_FXSetParameters(m_hFx20KHz_2, &p2);
		}
	}
}
//----------------------------------------------------------------------------
// リバーブのパラメータを得る
//----------------------------------------------------------------------------
BOOL CSound::GetReverb(BASS_DX8_REVERB * bdr)
{
	if(!m_hFxReverb) return FALSE;
	return BASS_FXGetParameters(m_hFxReverb, bdr);
}
//----------------------------------------------------------------------------
// リバーブの設定
//----------------------------------------------------------------------------
void CSound::SetReverb(BOOL bReverb)
{
	if(bReverb) {
		if(m_hFxReverb) ChannelRemoveFX(m_hFxReverb);
		m_hFxReverb = ChannelSetFX(BASS_FX_DX8_REVERB, 0);
		SetReverb();
	}
	else ChannelRemoveFX(m_hFxReverb), m_hFxReverb = 0;
}
//----------------------------------------------------------------------------
// リバーブの設定
//----------------------------------------------------------------------------
void CSound::SetReverb()
{
	BASS_FXSetParameters(m_hFxReverb, &m_bdr);
}
//----------------------------------------------------------------------------
// リバーブの設定
//----------------------------------------------------------------------------
void CSound::SetReverb(float fInGain, float fReverbMix, float fReverbTime,
					   float fHighFreqRTRatio, BOOL bReverb)
{
	BASS_DX8_REVERB _bdr = {fInGain, fReverbMix, fReverbTime,
							fHighFreqRTRatio};
	m_bdr = _bdr;
	BASS_FXSetParameters(m_hFxReverb, &m_bdr);
}
//----------------------------------------------------------------------------
// ３Ｄリバーブのパラメータを得る
//----------------------------------------------------------------------------
BOOL CSound::Get3DReverb(BASS_DX8_I3DL2REVERB * bdir)
{
	if(!m_hFx3DReverb) return FALSE;
	return BASS_FXGetParameters(m_hFx3DReverb, bdir);
}
//----------------------------------------------------------------------------
// ３Ｄリバーブの設定
//----------------------------------------------------------------------------
void CSound::Set3DReverb(BOOL b3DReverb)
{
	if(b3DReverb) {
		if(m_hFx3DReverb) ChannelRemoveFX(m_hFx3DReverb);
		m_hFx3DReverb = ChannelSetFX(BASS_FX_DX8_I3DL2REVERB, 0);
		Set3DReverb();
	}
	else ChannelRemoveFX(m_hFx3DReverb), m_hFx3DReverb = 0;
}
//----------------------------------------------------------------------------
// ３Ｄリバーブの設定
//----------------------------------------------------------------------------
void CSound::Set3DReverb()
{
	BASS_FXSetParameters(m_hFx3DReverb, &m_bdir);
}
//----------------------------------------------------------------------------
// ３Ｄリバーブの設定
//----------------------------------------------------------------------------
void CSound::Set3DReverb(int lRoom, int lRoomHF, float flRoomRolloffFactor,
					   float flDecayTime, float flDecayHFRatio,
					   int lReflections, float flReflectionsDelay, int lReverb,
					   float flReverbDelay, float flDiffusion, float flDensity,
					   float flHFReference, BOOL b3DReverb)
{
	BASS_DX8_I3DL2REVERB _bdir = {lRoom, lRoomHF, flRoomRolloffFactor,
								  flDecayTime, flDecayHFRatio, lReflections,
								  flReflectionsDelay, lReverb, flReverbDelay, 
								  flDiffusion, flDensity, flHFReference};
	m_bdir = _bdir;
	BASS_FXSetParameters(m_hFx3DReverb, &m_bdir);
}
//----------------------------------------------------------------------------
// ディレイのパラメータを得る
//----------------------------------------------------------------------------
BOOL CSound::GetDelay(BASS_DX8_ECHO * bde)
{
	if(!m_hFxDelay) return FALSE;
	return BASS_FXGetParameters(m_hFxDelay, bde);
}
//----------------------------------------------------------------------------
// ディレイの設定
//----------------------------------------------------------------------------
void CSound::SetDelay(BOOL bDelay)
{
	if(bDelay) {
		if(m_hFxDelay) ChannelRemoveFX(m_hFxDelay);
		m_hFxDelay = ChannelSetFX(BASS_FX_DX8_ECHO, 0);
		SetDelay();
	}
	else
		ChannelRemoveFX(m_hFxDelay), m_hFxDelay = 0;
}
//----------------------------------------------------------------------------
// ディレイの設定
//----------------------------------------------------------------------------
void CSound::SetDelay()
{
	BASS_FXSetParameters(m_hFxDelay, &m_bde);
}
//----------------------------------------------------------------------------
// ディレイの設定
//----------------------------------------------------------------------------
void CSound::SetDelay(float fWetDryMix, float fFeedback, float fLeftDelay,
					  float fRightDelay, BOOL lPanDelay, BOOL bDelay)
{
	BASS_DX8_ECHO _bde = {fWetDryMix, fFeedback, fLeftDelay, fRightDelay,
						  lPanDelay};
	m_bde = _bde;
	BASS_FXSetParameters(m_hFxDelay, &m_bde);
}
//----------------------------------------------------------------------------
// コーラスのパラメータを得る
//----------------------------------------------------------------------------
BOOL CSound::GetChorus(BASS_DX8_CHORUS * bdc)
{
	if(!m_hFxChorus) return FALSE;
	return BASS_FXGetParameters(m_hFxChorus, bdc);
}
//----------------------------------------------------------------------------
// コーラスの設定
//----------------------------------------------------------------------------
void CSound::SetChorus(BOOL bChorus)
{
	if(bChorus) {
		if(m_hFxChorus) ChannelRemoveFX(m_hFxChorus);
		m_hFxChorus = ChannelSetFX(BASS_FX_DX8_CHORUS, 0);
		SetChorus();
	}
	else
		ChannelRemoveFX(m_hFxChorus), m_hFxChorus = 0;
}
//----------------------------------------------------------------------------
// コーラスの設定
//----------------------------------------------------------------------------
void CSound::SetChorus()
{
	BASS_FXSetParameters(m_hFxChorus, &m_bdc);
}
//----------------------------------------------------------------------------
// コーラスの設定
//----------------------------------------------------------------------------
void CSound::SetChorus(float fWetDryMix, float fDepth, float fFeedback,
					   float fFreq, DWORD lWaveform, float fDelay,
					   DWORD lPhase, BOOL bChorus)
{
	BASS_DX8_CHORUS _bdc = {fWetDryMix, fDepth, fFeedback, fFreq,
							lWaveform, fDelay, lPhase};
	m_bdc = _bdc;
	BASS_FXSetParameters(m_hFxChorus, &m_bdc);
}
//----------------------------------------------------------------------------
// ループ用コールバック関数
//----------------------------------------------------------------------------
void CALLBACK CSound::LoopSyncProc(HSYNC handle, DWORD channel,
	DWORD data, void *user)
{
	CSound* pthis = (CSound*)user;
	CMainWnd* mainWnd = &pthis->GetMainWnd();
 	if(!pthis->OnLoop()) mainWnd->SetFinish(TRUE);
}
//----------------------------------------------------------------------------
// ループの設定
//----------------------------------------------------------------------------
void CSound::SetLoop(BOOL bLoop)
{
	m_bLoop = bLoop;
}
//----------------------------------------------------------------------------
// ABループ ( A ) の設定
//----------------------------------------------------------------------------
void CSound::SetABLoopA(BOOL bLoop)
{
	m_bABLoopA = bLoop;
	m_nLoopPosA = 0;
}
//----------------------------------------------------------------------------
// ABループ ( B ) の設定
//----------------------------------------------------------------------------
void CSound::SetABLoopB(BOOL bLoop)
{
	m_bABLoopB = bLoop;
	m_nLoopPosB = ChannelGetLength();
	ChannelRemoveSync();
	ChannelSetSync(BASS_SYNC_END, 0, LoopSyncProc, (DWORD *)this);
}
//----------------------------------------------------------------------------
// ABループ ( A ) の設定
//----------------------------------------------------------------------------
void CSound::SetLoopPosA(QWORD nPos)
{
	m_nLoopPosA = nPos;
}
//----------------------------------------------------------------------------
// ABループ ( B ) の設定
//----------------------------------------------------------------------------
void CSound::SetLoopPosB(QWORD nPos)
{
	m_bABLoopB = TRUE;
	m_nLoopPosB = nPos;
	ChannelRemoveSync();
	if(nPos >= ChannelGetLength())
		ChannelSetSync(BASS_SYNC_END, 0, LoopSyncProc, (DWORD *)this);
	else
		ChannelSetSync(BASS_SYNC_POS, m_nLoopPosB, LoopSyncProc, (DWORD *)this);
}
//----------------------------------------------------------------------------
// テンポを得る
//----------------------------------------------------------------------------
float CSound::GetTempo()
{
	return CBassFx::GetTempo() + 100.0f;
}
//----------------------------------------------------------------------------
// テンポの設定
//----------------------------------------------------------------------------
BOOL CSound::SetTempo(float tempo)
{
	return CBassFx::SetTempo(tempo - 100.0f);
}
//----------------------------------------------------------------------------
// サンプルレートを得る
//----------------------------------------------------------------------------
float CSound::GetSampleRate()
{
	return CBassFx::GetSampleRate() * 100.0f / ChannelGetFreq();
}
//----------------------------------------------------------------------------
// サンプルレートの設定
//----------------------------------------------------------------------------
BOOL CSound::SetSampleRate(float samplerate)
{
	return CBassFx::SetSampleRate(ChannelGetFreq() * samplerate / 100.0f);
}
//----------------------------------------------------------------------------
// 音量の設定
//----------------------------------------------------------------------------
BOOL CSound::ChannelSetVolume(float volume)
{
	BASS_BFX_VOLUME p = {0, volume / 100.0f};
	return BASS_FXSetParameters(m_hFxVolume, &p);
}
//----------------------------------------------------------------------------
// ループ
//----------------------------------------------------------------------------
UINT CSound::OnLoop()
{
	if(m_bABLoopA || m_bABLoopB || m_bLoop) {
		if(!m_rMainWnd.CheckLoop()) return 1;
		if(!ChannelSetPosition(m_nLoopPosA)) return 0;
		if(ChannelIsStopped()) {
			return ChannelPlay();
		}
		return 1;
	}
	return 0;
}
//----------------------------------------------------------------------------
