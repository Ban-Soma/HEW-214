
#include "DebugInfo.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Energy.h"
#include <sstream>
#include <iomanip>

extern Player* g_InfoPlayer = new Player;
extern EnemyManager* g_InfoEnemyManager = new EnemyManager;
extern Energy* g_InfoEnergy = new Energy;
void DebugInfo::RegisterPlayer(Player* player)
{
    g_InfoPlayer = player;
}

void DebugInfo::RegisterEnemyManager(EnemyManager* manager)
{
    g_InfoEnemyManager = manager;
}
void DebugInfo::RegisterEnergy(Energy* energy)
{
    g_InfoEnergy = energy;
}
bool DebugInfo::Initialize(ID3D11Device* device,ID3D11DeviceContext* context,const std::string& fontPath)
{
    Finalize();

    if (!m_fontSystem.Initialize(
        device, context, fontPath, 24))
    {
        return false;
    }

    m_text = std::make_unique<TextObject>(m_fontSystem);

    m_text->SetPosition(20.0f, 20.0f);
    m_text->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_text->SetScale(1.0f);
    m_text->SetLineSpacing(5.0f);
    
    return true;
}

void DebugInfo::Update()
{
    if (!m_text || !m_visible)
        return;

    UpdateFPS();
    values.fps = m_fps;
    //ここからの範囲内に変更する値を入力する
    
    
    DirectX::XMFLOAT3 SavePos=g_InfoPlayer->GetPos();

    values.playerX=SavePos.x;
    values.playerY = SavePos.y;
    values.playerZ = SavePos.z;

    values.energy = g_InfoEnergy->GetEnergy();


    //ここまでの範囲内に変更する値を入力する
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1);

    ss << "[ DEBUG INFO ]\n";

    ss << "FPS         : " << values.fps << "\n";

    ss << "Position    : "
        << values.playerX << ", "
        << values.playerY << ", "
        << values.playerZ << "\n";

    ss << "Energy      : " << values.energy << "\n";
    ss << "Life        : " << values.life << "\n";
    ss << "Combo       : " << values.combo << "\n";

    ss << "Dash Speed  : " << values.dashSpeed << "\n";
    ss << "Enemy Count : " << values.enemyCount << "\n";
    ss << "Wave        : " << values.wave << "\n";

    m_text->SetText(ss.str());
}

void DebugInfo::Draw()
{
    if (!m_visible || !m_text)
        return;

    m_text->Draw();
}

void DebugInfo::Finalize()
{
    m_text.reset();
    m_fontSystem.Finalize();
}

void DebugInfo::SetVisible(bool visible)
{
    m_visible = visible;
}

void DebugInfo::ToggleVisible()
{
    m_visible = !m_visible;
}

bool DebugInfo::IsVisible() const
{
    return m_visible;
}

void DebugInfo::UpdateFPS()
{
    auto now = Clock::now();

    float deltaTime =
        std::chrono::duration<float>(
            now - m_lastTime
        ).count();

    m_lastTime = now;

    m_elapsedTime += deltaTime;
    m_frameCount++;

    // 0.5秒ごとにFPSを更新
    if (m_elapsedTime >= 0.5f)
    {
        m_fps = m_frameCount / m_elapsedTime;

        m_frameCount = 0;
        m_elapsedTime = 0.0f;
    }
}
