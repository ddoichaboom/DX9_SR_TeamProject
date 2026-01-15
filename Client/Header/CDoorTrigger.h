#pragma once
#include "CTrigger.h"

//충돌시 호출되는 함수를 DOOR_IN/ OUT 이벤트를 호출하도록 고정함 
class CDoorTrigger : public CTrigger
{
protected:
	explicit			CDoorTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CDoorTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale);
	virtual				~CDoorTrigger();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

public:
	static CDoorTrigger*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CDoorTrigger*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale);

	void				OnBeginCollision() override;
	void				OnEndCollision() override;

protected:
	virtual		void	Free();

protected:
	EventData			m_EventData;


};

