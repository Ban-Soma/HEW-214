#pragma once

#include <DirectXMath.h>

class Bullet
{
private:
    DirectX::XMFLOAT3 m_pos;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_direction;

    float m_speed; // 弾の速度
    int m_life; // 弾の生存時間

public:
    // コンストラクタ
    Bullet();
    // 引数付きコンストラクタ
    Bullet(DirectX::XMFLOAT3 pos,
        DirectX::XMFLOAT3 direction);
    // デストラクタ
    ~Bullet();

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

    bool IsAlive() const;
};
