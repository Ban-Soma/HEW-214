#pragma once

#include <DirectXMath.h>

class Bullet
{
private:
    DirectX::XMFLOAT3 m_pos;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_direction;

    float m_speed;
    int m_life;

public:
    // コンストラクタ
    Bullet();
    // デストラクタ
    ~Bullet();

    // 更新処理
    void Update();
    // 描画処理
    void Draw();


};
