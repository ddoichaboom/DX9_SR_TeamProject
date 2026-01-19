#include "pch.h"
#include "CEffectToolBar.h"
#include "CEffectScene.h"
#include "CBlood.h"

CEffectToolBar::CEffectToolBar()
{
}

CEffectToolBar::~CEffectToolBar()
{
}


HRESULT CEffectToolBar::Ready_ToolBar(CEffectScene* _pEffectScene)
{
    m_pEffectScene = _pEffectScene;
    return S_OK;
}

void CEffectToolBar::Update_ToolBar()
{

}

void CEffectToolBar::Render_ToolBar()
{
    ImGui::Begin("Effect Bar", nullptr, ImGuiWindowFlags_NoCollapse);
    if (m_pEffectScene)
    {
        CBlood* pObj = m_pEffectScene->GetEmitter();
        if (pObj)
        {
            ImGui::Text("Transform");
            ImGui::Separator();

            _vec3 vPos = pObj->GetPos();
            _float fPos[3] = { vPos.x, vPos.y, vPos.z };
            if (ImGui::DragFloat3("Position", fPos, 0.1f))
            {
                pObj->SetPos(_vec3(fPos[0], fPos[1], fPos[2]));
            }

            _vec3 vVelo = pObj->GetVelocity();
            _float fVelo[3] = { vVelo.x, vVelo.y, vVelo.z };
            if (ImGui::DragFloat3("Velocity", fVelo, 0.1f))
            {
                pObj->SetVelocity(_vec3(fVelo[0], fVelo[1], fVelo[2]));
            }

            _float fScale = pObj->GetSize();
            if (ImGui::DragFloat("Size", &fScale, 0.1f, 1.f, 100.f))
            {
                pObj->SetSize(fScale);
            }

            D3DXCOLOR color = pObj->GetColor();
            _float fColor[4] = { color.r, color.g, color.b ,color.a};
            if (ImGui::DragFloat4("COLOR", fColor, 1.f, 1.f, 100.f))
            {
                pObj->SetColor(fColor);
            }


        }
    }
    ImGui::End();
}

CEffectToolBar* CEffectToolBar::Create(CEffectScene* _pEffectScene)
{
    CEffectToolBar* pInstance = new CEffectToolBar();

    if (FAILED(pInstance->Ready_ToolBar(_pEffectScene)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEffectToolBar Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CEffectToolBar::Free()
{

}