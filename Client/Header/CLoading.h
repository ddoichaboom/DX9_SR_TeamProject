#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CLoading : public CBase
{
public:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

	const _tchar* Get_String() { return m_szLoading; }

public:
	HRESULT					Ready_Loading(function<void()> _protoBaseFunc
										, function<void()> _protoTextureFunc
										, function<void()> _objectPoolFunc
										, function<void()> _readyEnvFunc
										, function<void()> _readyGameFunc
	);

	bool					Update_Loading();

public:
	static unsigned int CALLBACK Thread_Proto_Base(void* pArg);
	static unsigned int CALLBACK Thread_Proto_Texture(void* pArg);

	bool	IsEnd()			{ return m_bEnd; }
	_float	GetPercent()	{ return m_fPercent; }
private:
	static CRITICAL_SECTION	m_Crt_Base;
	static CRITICAL_SECTION	m_Crt_Texture;

	LPDIRECT3DDEVICE9		m_pGraphicDev;
	HANDLE					m_hThread[2];
	_tchar					m_szLoading[128];


	function<void()>		m_ProtoBaseFunc;
	function<void()>		m_ProtoTextureFunc;
	function<void()>		m_ObjectPoolFunc;
	function<void()>		m_ReadyEnvFunc;
	function<void()>		m_ReadyGameFunc;

	_float					m_fPercent;
	bool					m_bEnd = false;

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev
		, function<void()> _protoBaseFunc
		, function<void()> _protoTextureFunc
		, function<void()> _objectPoolFunc
		, function<void()> _readyEnvFunc
		, function<void()> _readyGameFunc);

private:
	virtual void		Free();

};
