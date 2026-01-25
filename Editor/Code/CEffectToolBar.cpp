#include "pch.h"
#include "CEffectToolBar.h"
#include "CEffectScene.h"
#include "CBlood.h"
#include "CTrail.h"
#include "CFlare.h"
#include "CExplosion.h"
#include "CBeamFlare.h"


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
    static const char* emitters[] = { "BLOOD", "TRAIL", "FLARE", "EXPLOSION", "BEAMFLARE", "BODYEMIT","HITUI", "BOSSTRAIL",
        "TAKEDOWN_BLOOD", "TOONFLASH", "TOONFOG", "SODAUI", "BOSSHP"};
    static int selectedIndex = -1;

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
            ImVec2 imv = ImVec2(100, 100);
            if (ImGui::BeginListBox("Emitter Type", imv))
            {
                for (int i = 0; i < size(emitters); i++)
                {
                    bool isSelected = (selectedIndex == i);

                    if (ImGui::Selectable(emitters[i], isSelected))
                    {
                        selectedIndex = i;
                        m_pEffectScene->SetEmitter(EFFECT_TYPE(i));
                        break;
                    }
                }
                ImGui::EndListBox();
            }

            ImGui::Separator();

            if (selectedIndex == 3)
            {
                CExplosion* exp = (CExplosion*)m_pCurParticle;
                _vec2 redPosX{ exp->vRedPosXOffset }, redPosY{ exp->vRedPosYOffset }, redSize{ exp->vRedSizeXOffset },
                    redSpeed{ exp->vRedSpeedOffset }, GrayPosX{ exp->vGrayPosXOffset },
                    GrayPosY{ exp->vGrayPosYOffset }, GraySize{ exp->vGraySizeXOffset }, GraySpeed{ exp->vGraySpeedOffset };
                if (ImGui::DragFloat2("Red Pos X", redPosX, 1.f, -100.f, 100.f))
                {
                    exp->vRedPosXOffset = redPosX;
                }
                if (ImGui::DragFloat2("Red Pos Y", redPosY, 1.f, -100.f, 100.f))
                {
                    exp->vRedPosYOffset = redPosY;
                }
                if (ImGui::DragFloat2("Red Size", redSize, 1.f, -100.f, 100.f))
                {
                    exp->vRedSizeXOffset = redSize;
                }
                if (ImGui::DragFloat2("Red Speed", redSpeed, 1.f, -100.f, 100.f))
                {
                    exp->vRedSpeedOffset = redSpeed;
                }

                if (ImGui::DragFloat2("Gray Pos X", GrayPosX, 1.f, -100.f, 100.f))
                {
                    exp->vGrayPosXOffset = GrayPosX;
                }
                if (ImGui::DragFloat2("Gray Pos Y", GrayPosY, 1.f, -100.f, 100.f))
                {
                    exp->vGrayPosYOffset = GrayPosY;
                }
                if (ImGui::DragFloat2("Gray Size", GraySize, 1.f, -100.f, 100.f))
                {
                    exp->vGraySizeXOffset = GraySize;
                }
                if (ImGui::DragFloat2("Gray Speed", GraySpeed, 1.f, -100.f, 100.f))
                {
                    exp->vGraySpeedOffset = GraySpeed;
                }
                ImGui::End();
                return;

            }


            _int textureCnt = m_pCurParticle->GetTextureCnt();
            _int textureState = m_pCurParticle->GetState();

            if (textureCnt && ImGui::DragInt("Texture State", &textureState, 1, 0, textureCnt-1))
            {
                m_pCurParticle->ChangeState(textureState);
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
            if (ImGui::DragFloat4("COLOR", fColor, 0.01f, 0.f, 1.f))
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

            if (selectedIndex == 1)
            {
                CTrail* trail = (CTrail*)m_pCurParticle;
                _vec3 vStart = trail->GetStartPos();
                _vec3 vEnd = trail->GetEndPos();

                if (ImGui::DragFloat3("Start Pos", vStart, 1.f, 1.f, 300.f))
                {
                    trail->SetTrailPos(vStart, vEnd);
                }
                if (ImGui::DragFloat3("End Pos", vEnd, 1.f, 1.f, 300.f))
                {
                    trail->SetTrailPos(vStart, vEnd);
                }
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