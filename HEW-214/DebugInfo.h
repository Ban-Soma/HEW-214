#pragma once

#include "Font/FontSystem.h"
#include "Font/TextObject.h"

#include <d3d11.h>
#include <memory>
#include <string>
#include <chrono>
#include "Player.h"
#include "EnemyManager.h"

class Player;
class EnemyManager;
// デバッグ表示する情報
struct DebugValues
{
    float fps = 0.0f;

    float playerX = 0.0f;
    float playerY = 0.0f;
    float playerZ = 0.0f;

    float energy = 0.0f;
    int life = 0;
    int combo = 0;

    float dashSpeed = 0.0f;
    int enemyCount = 0;
    int wave = 0;
};

class DebugInfo
{
public:
    bool Initialize(ID3D11Device* device,ID3D11DeviceContext* context,const std::string& fontPath);

    void RegisterPlayer(Player* player);
    void RegisterEnemyManager(EnemyManager* manager);

    void Update();
    void Draw();
    void Finalize();

    void SetVisible(bool visible);
    void ToggleVisible();
    bool IsVisible() const;
    void UpdateFPS();
private:
    
    FontSystem m_fontSystem;
    std::unique_ptr<TextObject> m_text;
    bool m_visible = true;
    
    DebugValues values;

    //FPS計測用
    using Clock = std::chrono::steady_clock;
    Clock::time_point m_lastTime = Clock::now();
    int m_frameCount = 0;
    float m_fps = 0.0f;
    float m_elapsedTime = 0.0f;
    
};
