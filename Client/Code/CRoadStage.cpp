#include "pch.h"
#include "CRoadStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"
#include "CUIManager.h"
#include "CVideoMgr.h"


// 필수 게임 오브젝트 
#include "CRoadCamera.h"
#include "CCursor.h"
#include "CRoadPlayer.h"

//스카이 박스
#include "CSkyBox.h"

//미니건 관련
#include "CMinigun.h"
#include "CPannel.h"
#include "CChain.h"

//환경 로직 오브젝트
#include "CDisplayCubeObject.h"

// 게임 로직 오브젝트
#include "CFlyMon.h"
#include "CPlayerBullet.h"


// 이펙트 로직 오브젝트
#include "CFlare.h"
#include "CExplosion.h"
#include "CBodyEmit.h"
#include "CHitUI.h"
#include "CMapCollider.h"
#include "CLoadingEX.h"
#include "CBackGround.h"

wstring CRoadStage::szRoadMapBGM = L"RoadMap_BGM.wav";

CRoadStage::CRoadStage(LPDIRECT3DDEVICE9 pGraphicDev) 
	: CStage(pGraphicDev), m_iFileIndex(4), m_pEndMapCollider(nullptr)
{
	fill(m_pRoom, m_pRoom + ROOM_CNT, nullptr);
}

CRoadStage::~CRoadStage()
{
}

HRESULT CRoadStage::Ready_Scene()
{
	m_pBackGround = CBackGround::Create(m_pGraphicDev);
	m_pLoadingEX = CLoadingEX::Create(m_pGraphicDev);

	if (!m_pLoadingEX) return E_FAIL;

	for (int i = 0; i < ROOM_CNT; i++)
	{
		m_pRoom[i] = new CRoom();
		m_pRoom[i]->SetOriginZPos(m_fZHalfRadius * 2 * i + m_fZHalfRadius);
		m_pRoom[i]->SetSize({1,1,m_fZHalfRadius});
		m_qRoomOrder.push(i);
	}

	const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();

	if (!vecMapFiles.empty())
		m_wstrCurrentMapFile = vecMapFiles[m_iFileIndex];
	else
		return E_FAIL;

	//1단계
	//이전 스테이지 이후 필요없는 오브젝트 풀 제거  
	m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Remove_PrevObjectPool(); });
	//텍스쳐 제외 프로토타입 생성
	m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Ready_Prototype(); });
	//캐릭터 텍스쳐 생성 
	m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Ready_CharacterTextureProto(); });

	//2단계
	//1단계에서 만들어진 텍스쳐로 캐릭터 오브젝트 풀 생성 
	m_pLoadingEX->AddTask(CLoadingEX::Lv2_CHAR_RES, [this]() { this->Ready_ObjectPool_Character(); });
	m_pLoadingEX->AddTask(CLoadingEX::Lv2_CHAR_RES, [this]() { this->Ready_TerrainTextureProto(); });

	//3단계
	m_pLoadingEX->AddTask(CLoadingEX::Lv3_TERRAIN_RES, [this]() { this->Ready_ObjectPool_Terrain(); });
	m_pLoadingEX->AddTask(CLoadingEX::Lv3_TERRAIN_RES, [this]() { this->Ready_UITextureProto(); });

	//4단계
	m_pLoadingEX->AddTask(CLoadingEX::Lv4_UI_RES, [this]() { this->Ready_ObjectPool_UI(); });
	m_pLoadingEX->AddTask(CLoadingEX::Lv4_UI_RES, [this]() { this->Ready_EffectTextureProto(); });

	//5단계
	m_pLoadingEX->AddTask(CLoadingEX::Lv5_EFFECT_RES, [this]() { this->Ready_ObjectPool_Effect(); });

	//6단계 맵 - 환경 로드
	m_pLoadingEX->AddTask(CLoadingEX::Lv6_MAP_ENV_LOAD, [this]() { this->Ready_Environment_Layer(L"Environment_Layer"); });

	//7단계 맵 - 게임로직 로드 
	m_pLoadingEX->AddTask(CLoadingEX::Lv7_MAP_GAME_LOAD, [this]() { this->Ready_GameLogic_Layer(L"GameLogic_Layer"); });


	//if (FAILED(Ready_CharacterTextureProto())) return E_FAIL;
	//if (FAILED(Ready_ObjectPool_Character())) return E_FAIL;


	//if (FAILED(Ready_TerrainTextureProto())) return E_FAIL;
	//if (FAILED(Ready_ObjectPool_Terrain())) return E_FAIL;

	//if (FAILED(Ready_UITextureProto())) return E_FAIL;
	//if (FAILED(Ready_ObjectPool_UI())) return E_FAIL;

	//if (FAILED(Ready_EffectTextureProto())) return E_FAIL;
	//if (FAILED(Ready_ObjectPool_Effect())) return E_FAIL;

	//if (FAILED(Ready_Environment_Layer(L"Environment_Layer"))) return E_FAIL;
	//if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer"))) return E_FAIL;

	return S_OK;
}

_int CRoadStage::Update_Scene(const _float& fTimeDelta)
{
	if (m_pLoadingEX->IsEnd() == false)
	{
		m_pBackGround->Update_GameObject(fTimeDelta);
		m_pLoadingEX->Update_Loading(fTimeDelta);

		if (m_pLoadingEX->IsEnd())
		{
			CSoundMgr::GetInstance()->PlayBGM(szRoadMapBGM.c_str(), 1.4f);
			m_bStartSound = true;
		}
		else return 0;
	}

	if (m_bStageEnd)
	{
		return RET_DEAD;
	}

	Move_EnvObjects(fTimeDelta);
	for (int i = 0; i < ROOM_CNT; i++)
	{
		m_pRoom[i]->Update_Room(fTimeDelta);
	}

	if (m_pEndMapCollider) m_pEndMapCollider->Update_GameObject(fTimeDelta);

	int iExit = CStage::Update_Scene(fTimeDelta);

	if (CDInputMgr::GetInstance()->Key_Down(DIK_P))
	{
		m_bStageEnd = true;
	}

	return 0;
}

void CRoadStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	if(m_pLoadingEX->IsEnd() == false) return;

	for (int i = 0; i < ROOM_CNT; i++)
	{
		m_pRoom[i]->LateUpdate_Room(fTimeDelta);
	}
	if(m_pEndMapCollider) m_pEndMapCollider->LateUpdate_GameObject(fTimeDelta);

	CStage::LateUpdate_Scene(fTimeDelta);

	Check_Collision();
}

void CRoadStage::Render_Scene()
{
}

HRESULT CRoadStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();

	if (nullptr == pLayer) return E_FAIL;

	CGameObject* pGameObject = nullptr;
	CGameObject* pEndMapCollider = nullptr; 


	for (_int idx = 0; idx < ROOM_CNT; idx++)
	{
		if (FAILED(CMapLoader::GetInstance()->Load_EnvObject_ToRoom(
			m_wstrCurrentMapFile,
			idx,  // roomIndex
			pLayer,
			m_pGraphicDev,
			pLayerTag,
			m_pRoom[idx], idx==0?&pEndMapCollider:nullptr			
		)))
		{
			// 1번방이 있으면 오류 체크 위해 주석 해제 
			MSG_BOX("Road Stage Env Room Load Failed");
			return E_FAIL;
		}
		else
		{
			m_setLoadedRooms.insert(idx);
		}
	}
	
	m_iCurrentRoomIndex = 0;
	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pEnvironment_Layer = pLayer;

	if (pEndMapCollider)
	{
		m_pEndMapCollider = dynamic_cast<CMapCollider*>(pEndMapCollider);
		if (!m_pEndMapCollider) return E_FAIL;
		m_pEndMapCollider->SetPos(m_vEndColliderZPos);
	}
	return S_OK;
}

HRESULT CRoadStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)	return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// 플레이어
	pGameObject = CRoadPlayer::Create(m_pGraphicDev);
	if (pGameObject == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 카메라
	_vec3 vPlayerPos = { 0.f, 0.f, 0.f };
	_vec3 vEye = vPlayerPos;
	_vec3 vAt = { vPlayerPos.x, vPlayerPos.y, vPlayerPos.z +10.f };
	_vec3 vUp = { 0.f, 1.f, 0.f };
	pGameObject = CRoadCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 스카이 박스
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 커서
	pGameObject = CCursor::Create(m_pGraphicDev);
	if (pGameObject == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject = CFlyMon::Create(m_pGraphicDev);
	if (pGameObject == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject->SetPos({ 0.f, 21.f, 30.f });

	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pGameLogic_Layer = pLayer;

	return S_OK;
}

HRESULT CRoadStage::Ready_Prototype()
{
	return S_OK;
}

HRESULT CRoadStage::Remove_PrevObjectPool()
{

	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Character()
{
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CPlayerBullet>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CPlayerBullet>(m_pGraphicDev)))
		{
			MSG_BOX("Bullet Pool Create Failed");
			return E_FAIL;
		}
	}


	if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlyMon>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlyMon>(m_pGraphicDev)))
		{
			MSG_BOX("FlyMon Pool Create Failed");
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Terrain()
{
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CDisplayObject>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDisplayObject>(m_pGraphicDev)))
		{
			MSG_BOX("DisplayObject Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CDisplayCubeObject>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDisplayCubeObject>(m_pGraphicDev)))
		{
			MSG_BOX("Display Cube Object Pool Create Failed");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_UI()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Effect()
{
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlare>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlare>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Flare Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CExplosion>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CExplosion>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Explosion Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CBodyEmit>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBodyEmit>(m_pGraphicDev)))
		{
			MSG_BOX("Effect BodyEmit Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CHitUI>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CHitUI>(m_pGraphicDev)))
		{
			MSG_BOX("Effect HitUI Pool Create Failed");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CRoadStage::Ready_CharacterTextureProto()
{
	CTexture* pCom_Texture = nullptr;

	//Minigun
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CMinigun::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MinigunTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MinigunAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CMinigun::GetAnimSources()))))
		return E_FAIL;

	//Minigun Pannel
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CPannel::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PannelTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PannelAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CPannel::GetAnimSources()))))
		return E_FAIL;

	//Minigun Chain
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CChain::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChainTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChainAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CChain::GetAnimSources()))))
		return E_FAIL;

	// Monster
	//pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlyMon::GetTextureSources());
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonTexture", pCom_Texture)))
	//	return E_FAIL;
	//
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CFlyMon::GetAnimSources()))))
	//	return E_FAIL;

	//Bullet Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CPlayerBullet::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerBulletTexture", pCom_Texture)))
		return E_FAIL;
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerBulletAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CPlayerBullet::GetAnimSource()))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoadStage::Ready_TerrainTextureProto()
{
	CCubeTexture* pCom_Cube_Texture = nullptr;
	//Displays Cube Object 
	pCom_Cube_Texture = Engine::CCubeTexture::Create(m_pGraphicDev, CDisplayCubeObject::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DisplayCubeObject_Texture", pCom_Cube_Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoadStage::Ready_UITextureProto()
{
	// ㅣ필수ㅣ TEST 할때 최초 세팅을 위해 생성 나중에 지워야함 
	//CUIManager::GetInstance()->Ready_GameObject(m_pGraphicDev);
	return S_OK;
}

HRESULT CRoadStage::Ready_EffectTextureProto()
{
	// ㅣ필수ㅣTEST 할때 최초 세팅을 위해 생성 나중에 지워야함 
	CTexture* pCom_Texture = nullptr;

	return S_OK;
}

void CRoadStage::Check_Collision()
{
	auto iter_Map_Mon = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_MONSTER);
	auto iter_Map_Bullet = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_BULLET);


	for (multimap<OBJ_ID, CGameObject*>::iterator it_Mon = iter_Map_Mon.first; it_Mon != iter_Map_Mon.second; it_Mon++)
	{
		CCollision* pMonCollision = static_cast<CCollision*>(
			it_Mon->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));

		if (!pMonCollision)
			continue;

		CCollider* pMonCollider = pMonCollision->GetCollider();
		if (!pMonCollider)
			continue;

		for (multimap<OBJ_ID, CGameObject*>::iterator it_bullet = iter_Map_Bullet.first; it_bullet != iter_Map_Bullet.second; it_bullet++)
		{
			CCollision* mapBul_Collision = static_cast<CCollision*>(it_bullet->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
			CCollider* mapCollider = mapBul_Collision->GetCollider();
			if (!mapCollider) continue;


			CCollision::Collision_Base(pMonCollider, mapCollider);
		}
	}

	// 맵 End 콜라이더랑 맨 앞 방의 콜라이더랑 충돌체크
	if (!m_pEndMapCollider) return;

	CCollision* pEndCollision = static_cast<CCollision*>(m_pEndMapCollider->Get_Component(ID_DYNAMIC, L"Com_Collision"));
	CCollider* pEndCol = nullptr;

	if (pEndCollision) pEndCol = pEndCollision->GetCollider();

	_int iFrontRoom = m_qRoomOrder.front();

	auto iter_Map_Col = m_pEnvironment_Layer->Get_Objects(OBJ_COL);
	for (multimap<OBJ_ID, CGameObject*>::iterator it_col = iter_Map_Col.first; it_col != iter_Map_Col.second; it_col++)
	{
		if (it_col->second->GetRoomIndex() != iFrontRoom) continue;

		CCollision* mapCol_Collision = static_cast<CCollision*>(it_col->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		CCollider* mapCollider = mapCol_Collision->GetCollider();
		if (!mapCollider) continue;

		bool bCollision = CCollision::CheckCollision(mapCollider, pEndCol);
		if (bCollision)
		{
			//맨 뒤 방의 번호 
			_int iLastRoomIdx = m_qRoomOrder.back();
			_float iLastZPos = m_pRoom[iLastRoomIdx]->GetOriginZPos();
			_float iLastScale = m_pRoom[iLastRoomIdx]->GetSize().z;
			//맨 뒤 방의 다음에 붙이기 
			m_pRoom[iFrontRoom]->Set_Pos_Room(iLastZPos + iLastScale * 2.f);
			
			m_qRoomOrder.pop();
			m_qRoomOrder.push(iFrontRoom);
			return;
		}
	}
	
}

void CRoadStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_ENDING)
	{
		//m_bStageEnd = true;
	}
}

void CRoadStage::Move_EnvObjects(const _float& fTimeDelta)
{
	_float fDelta = m_fSpeed * fTimeDelta;
	for (int i = 0; i < ROOM_CNT; i++)
	{
		m_pRoom[i]->Move_Room(fDelta);
	}
}

CRoadStage* CRoadStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRoadStage* pRoadStage = new CRoadStage(pGraphicDev);

	if (FAILED(pRoadStage->Ready_Scene()))
	{
		Safe_Release(pRoadStage);
		MSG_BOX("Road Stage Create Failed");
		return nullptr;
	}

	return pRoadStage;
}

void CRoadStage::Free()
{
	for (int i = 0; i < ROOM_CNT; i++)
		Safe_Release(m_pRoom[i]);

	m_pEndMapCollider->ReturnToPool();

	m_qRoomOrder = queue<int>();
	Safe_Release(m_pBackGround);
	Safe_Release(m_pLoadingEX);
	CScene::Free();
}