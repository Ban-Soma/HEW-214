#pragma once
#include <DirectXMath.h>
class Shockwave
{
public:
    Shockwave();

    void Start(const DirectX::XMFLOAT3& pos);
    void Update();
    void Draw();
    bool IsActive() const;

    DirectX::XMFLOAT3 GetPos()const;
    float GetMaxRadius()const;
    float GetProgress() const;
private:
    DirectX::XMFLOAT3 m_Pos;

    float m_Timer;
    float m_Duration;
    float m_MaxRadius;

    bool m_Active;
};
