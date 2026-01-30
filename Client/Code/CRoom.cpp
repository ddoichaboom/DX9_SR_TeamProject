#include "pch.h"
#include "CRoom.h"
//#include "CTransform.h"


CRoom::CRoom()
	:m_fZPos(0.f) ,m_vSize{1,1,1}, m_fMoveDelta(0.f)
{
}

CRoom::~CRoom()
{
}

_int CRoom::Update_Room(const _float& fTimeDelta)
{
	//Stage에서 Move Delta가 모두 계산된 뒤 Update 호출 
	//m_fMoveDelta의 누적값을 합함 
	
	//방의 위치 갱신 
	m_fZPos += m_fMoveDelta;
	//오브젝트 위치 갱신
	for (auto iter = m_vObjectInfo.begin(); iter != m_vObjectInfo.end(); iter++)
	{
		_vec3 pos = (iter->first)->GetPos();
		pos.z += m_fMoveDelta;
		(iter->first)->SetPos(pos);
		//Static Transform 직접 Update
		if (iter->second) iter->second->Update_Component(fTimeDelta);
	}
	m_fMoveDelta = 0.f;

	return 0;

}

void CRoom::LateUpdate_Room(const _float& fTimeDelta)
{

}

void CRoom::PushObject(CGameObject* obj)
{
	if (!obj) return;
	//Static이 아닌 tranform은  nulltr로 들어옴 
	CTransform* pTransform = static_cast<CTransform*>(obj->Get_Component(ID_STATIC, L"Com_Transform"));
	m_vObjectInfo.push_back( make_pair(obj, pTransform));
}

_float CRoom::GetNextPosZ()
{
	return m_fZPos + m_vSize.z * 2.f;
}

void CRoom::Set_Pos_Room(_float _posZ)
{
	m_fMoveDelta += _posZ - m_fZPos;
}

void CRoom::Free()
{
	//레이어에서 제거하기 위해 SetDead 처리 
	for (auto iter = m_vObjectInfo.begin(); iter != m_vObjectInfo.end(); iter++)
	{
		(iter->first)->SetDead();
	}
	m_vObjectInfo.clear();
}
