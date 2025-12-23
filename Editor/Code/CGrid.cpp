#include "pch.h"
#include "CGrid.h"

CGrid::CGrid(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pGraphicDev(pGraphicDev)
    , m_pVB(nullptr)
    , m_dwVtxCount(0)
    , m_bVisible(true)
{
    m_pGraphicDev->AddRef();
}

CGrid::~CGrid()
{
}

HRESULT CGrid::Ready_Grid(_uint iCountX, _uint iCountZ, _float fInterval)
{
    // 정점 개수 계산
    m_dwVtxCount = (iCountX + 1) * 2 + (iCountZ + 1) * 2;

    // Vertex Buffer 생성
    if (FAILED(m_pGraphicDev->CreateVertexBuffer(
        m_dwVtxCount * sizeof(VTXCOL),
        D3DUSAGE_WRITEONLY,
        FVF_COL,
        D3DPOOL_MANAGED,
        &m_pVB,
        NULL)))
    {
        MSG_BOX("Grid VB Create Failed");
        return E_FAIL;
    }

    // 정점 데이터 작성
    VTXCOL* pVertex = nullptr;
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    _uint iIndex = 0;

    // X축 평행선 (Z방향)
    for (_uint i = 0; i <= iCountX; ++i)
    {
        _float fX = -(_float)iCountX * 0.5f * fInterval + i * fInterval;

        pVertex[iIndex].vPosition = _vec3(fX, 0.f, -(_float)iCountZ * 0.5f * fInterval);
        pVertex[iIndex].dwColor = D3DCOLOR_ARGB(255, 100, 100, 100);
        ++iIndex;

        pVertex[iIndex].vPosition = _vec3(fX, 0.f, (_float)iCountZ * 0.5f * fInterval);
        pVertex[iIndex].dwColor = D3DCOLOR_ARGB(255, 100, 100, 100);
        ++iIndex;
    }

    // Z축 평행선 (X방향)
    for (_uint i = 0; i <= iCountZ; ++i)
    {
        _float fZ = -(_float)iCountZ * 0.5f * fInterval + i * fInterval;

        pVertex[iIndex].vPosition = _vec3(-(_float)iCountX * 0.5f * fInterval, 0.f, fZ);
        pVertex[iIndex].dwColor = D3DCOLOR_ARGB(255, 100, 100, 100);
        ++iIndex;

        pVertex[iIndex].vPosition = _vec3((_float)iCountX * 0.5f * fInterval, 0.f, fZ);
        pVertex[iIndex].dwColor = D3DCOLOR_ARGB(255, 100, 100, 100);
        ++iIndex;
    }

    m_pVB->Unlock();

    return S_OK;
}

_int CGrid::Update_GameObject(const _float& fTimeDelta)
{
    return 0;
}

void CGrid::Render_GameObject()
{
    if (!m_bVisible)
        return;

    if (nullptr == m_pVB)
        return;

    // World Matrix (Identity)
    _matrix matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    // Stream 설정
    m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXCOL));
    m_pGraphicDev->SetFVF(FVF_COL);

    // 렌더링
    m_pGraphicDev->DrawPrimitive(D3DPT_LINELIST, 0, m_dwVtxCount / 2);
}

CGrid* CGrid::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    _uint iCountX, _uint iCountZ, _float fInterval)
{
    CGrid* pInstance = new CGrid(pGraphicDev);

    if (FAILED(pInstance->Ready_Grid(iCountX, iCountZ, fInterval)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Grid Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CGrid::Free()
{
    if (m_pVB)
    {
        m_pVB->Release();
        m_pVB = nullptr;
    }

    Safe_Release(m_pGraphicDev);

    CGameObject::Free();
}