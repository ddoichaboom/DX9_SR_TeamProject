#include "pch.h"
#include "CEffectToolBar.h"
#include "CEffectScene.h"
#include "CBlood.h"
#include "CParticleEmitter.h"

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
        m_pCurParticle = m_pEffectScene->GetEmitter();
        if (m_pCurParticle)
        {
            ImGui::Text("Particle");
            ImGui::Separator();
            bool input = ImGui::Button("Reset");
            if (input) m_pCurParticle->Reset();

            ImGui::Separator();

            _int textureCnt = m_pCurParticle->GetTextureCnt();
            _int textureState = m_pCurParticle->GetState();

            if (textureCnt && ImGui::DragInt("Texture State", &textureState, 1, 0, textureCnt-1))
            {
                m_pCurParticle->ChangeState(textureState);
              //  m_pCurParticle->Reset();
            }


            _vec3 vPos = m_pCurParticle->GetPos();
            _float fPos[3] = { vPos.x, vPos.y, vPos.z };
            if (ImGui::DragFloat3("Position", fPos, 0.1f))
            {
                m_pCurParticle->SetPos(_vec3(fPos[0], fPos[1], fPos[2]));
            }

            _vec3 vVelo = m_pCurParticle->GetVelocity();
            _float fVelo[3] = { vVelo.x, vVelo.y, vVelo.z };
            if (ImGui::DragFloat3("Velocity", fVelo, 0.1f))
            {
                m_pCurParticle->SetVelocity(_vec3(fVelo[0], fVelo[1], fVelo[2]));
            }

            _vec2 vScale = m_pCurParticle->GetSize();
            if (ImGui::DragFloat2("Size", vScale, 0.1f, 1.f, 100.f))
            {
                m_pCurParticle->SetSize(vScale);
            }

            D3DXCOLOR color = m_pCurParticle->GetColor();
            _float fColor[4] = { color.r, color.g, color.b ,color.a};
            if (ImGui::DragFloat4("COLOR", fColor, 1.f, 1.f, 100.f))
            {
                m_pCurParticle->SetColor(fColor);
            }

           _float animSpeed = m_pCurParticle->GetAnimSpeed();
            if (ImGui::DragFloat("AnimSpeed", &animSpeed, 0.1f, 0.1f, 10.f))
            {
                m_pCurParticle->SetAnimSpeed(animSpeed);
            }

            bool bLoop = m_pCurParticle->GetLoop();
            if (ImGui::Checkbox("LOOP", &bLoop))
            {
                m_pCurParticle->SetLoop(bLoop);
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