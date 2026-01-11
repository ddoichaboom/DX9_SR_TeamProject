#include "pch.h"
#include "CBeam.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"

vector<TextureSource> CBeam::m_vTextureSource =
{
	 { 0, L"../Bin/Resource/Texture/Monster/BeamMon/Laser_512.dds" }
};
CBeam::CBeam(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev) , m_pBufferCom(nullptr), m_pTransformCom(nullptr),
	m_pTextureCom(nullptr), m_fTime(0.f)
{
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matTrans);
	D3DXMatrixIdentity(&m_matRot);	
	D3DXMatrixIdentity(&m_matVtxTran);
}

CBeam::~CBeam()
{
}

CBeam* CBeam::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBeam* pBeam = new CBeam(pGraphicDev);

	if (FAILED(pBeam->Ready_GameObject()))
	{
		Safe_Release(pBeam);
		MSG_BOX("Beam Create Failed");
		return nullptr;
	}

	return pBeam;
}

HRESULT CBeam::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pTextureCom->Change_Texture(0);
	//m_pTransformCom->m_vScale = { 0.1f, 40.f ,1.f };
	
	m_matScale._11 = 0.1f;
	m_matScale._22 = 500.f;
	m_matScale._33 = 1.f;

	return S_OK;
}

_int CBeam::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	_vec3 pos;
	memcpy(&pos,&m_matTrans.m[3], sizeof(_vec3));
	Compute_ViewZ(&pos);

	m_fTime += fTimeDelta;
	return iExit;
}

void CBeam::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBeam::Render_GameObject()
{
	//_matrix finalWorld = m_matVtxTran* m_matScale * m_matVtxTranInv * m_matRot * m_matTrans;
	_matrix finalWorld =  m_matScale * m_matVtxTran* m_matRot * m_matTrans;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &finalWorld);
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

bool CBeam::CheckCollision(CCollider* _pCollider)
{
	return CCollision::Collision_Ray(_pCollider, m_vPos, m_vShootDir);
}

HRESULT CBeam::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	//VIBuffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTexUp*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexUp"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BeamTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CBeam::Free()
{
	CGameObject::Free();
}


void CBeam::SetPos(_vec3 _pos)
{
	m_vPos = _pos;
	memcpy(&m_matTrans.m[3], &m_vPos, sizeof(_vec3));
}

//발사 회전축을 따라가면서 그 축을 기준으로 카메라를 쳐다보도록 구현
void CBeam::SetShootDir(_vec3 _dir)
{
	m_vShootDir = _dir;
	D3DXMatrixIdentity(&m_matRot);

	_vec3 vRight, vUp, vLook;
	//Up은 발사 방향 
	D3DXVec3Normalize(&vUp, &_dir);

	_matrix View;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &View);
	D3DXMatrixInverse(&View, NULL, &View);
	_vec3 camPos = { View._41, View._42, View._43 };
	vLook = camPos - m_vPos;
	D3DXVec3Normalize(&vLook, &vLook);

	//Right는 둘의 외적 
	D3DXVec3Cross(&vRight, &vLook, &vUp);
	D3DXVec3Normalize(&vRight, &vRight);

	vLook = { View._31, View._32, View._33 };

	memcpy(&m_matRot.m[INFO_RIGHT],	&vRight,	sizeof(_vec3));
	memcpy(&m_matRot.m[INFO_UP],	&vUp,		sizeof(_vec3));
	memcpy(&m_matRot.m[INFO_LOOK],	&vLook,		sizeof(_vec3));
}

void CBeam::SetScale(ROTATION _Axis, _float _scale)
{
	m_matScale.m[_Axis][_Axis] = _scale;
}

void CBeam::SetPrevTranslation(_vec3 _vec)
{
	D3DXMatrixTranslation(&m_matVtxTran, _vec.x, _vec.y, _vec.z);
}

