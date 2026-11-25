#pragma once

#include <DirectXMath.h>
#include "ShockWave.h"

class Bullet
{
private:
    DirectX::XMFLOAT3 m_pos;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_direction;

    float m_speed; // 弾の速度
    int m_life; // 弾の生存時間

    bool m_active;
public:
    // コンストラクタ
    Bullet();
    // 引数付きコンストラクタ
    Bullet(DirectX::XMFLOAT3 pos,
        DirectX::XMFLOAT3 direction);
    // デストラクタ
    ~Bullet();

    void Reset(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 direction);
    // 更新処理
    void Update();
    // 描画処理
    void Draw();
    void SetActive(bool active);
    bool IsActive()const;
    bool IsAlive() const;
};
