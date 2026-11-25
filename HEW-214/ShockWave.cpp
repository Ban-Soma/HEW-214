
#include "Shockwave.h"
#include "GameTime.h"
#include "Geometory.h"

#include <cmath>
#include <algorithm>

Shockwave::Shockwave()
    : m_Pos{ 0.0f, 0.2f, 0.0f }
    , m_Timer(0.0f)
    , m_Duration(0.20f)
    , m_MaxRadius(3.0f)
    , m_Active(false)
{
}


void Shockwave::Start(const DirectX::XMFLOAT3& pos)
{
    m_Pos = pos;
    m_Timer = 0.0f;
    m_Active = true;
}

void Shockwave::Update()
{
    if (!m_Active)return;

    m_Timer += GameTime::UnScaleDeltaTime();

    if (m_Timer >= m_Duration)
    {
        m_Timer = m_Duration;
        m_Active = false;
    }
}

void Shockwave::Draw()
{
    if (!m_Active)
        return;

    const float t = (std::min)(m_Timer / m_Duration, 1.0f);

    const float radius = m_MaxRadius * t;

    const float alpha = 1.0f - t;

    const int segments = 48;
    const float y = m_Pos.y + 0.05f;

    for (int i = 0; i < segments; ++i)
    {
        float a0 = DirectX::XM_2PI * i / segments;
        float a1 = DirectX::XM_2PI * (i + 1) / segments;

        float x0 = m_Pos.x + cosf(a0) * radius;
        float z0 = m_Pos.z + sinf(a0) * radius;

        float x1 = m_Pos.x + cosf(a1) * radius;
        float z1 = m_Pos.z + sinf(a1) * radius;


        Geometory::AddLine(
            { x0, y, z0, },
            { x1, y, z1,},
            { 0.3f, 0.9f, 1.0f, alpha }
        );
    }
}

bool Shockwave::IsActive() const
{
    return m_Active;
}

DirectX::XMFLOAT3 Shockwave::GetPos()const
{
    return m_Pos;
}

float Shockwave::GetMaxRadius() const
{
    return m_MaxRadius;
}

float Shockwave::GetProgress() const
{
    if (m_Duration <= 0.0f)return 1.0f;
    return (std::min)(m_Timer/m_Duration,1.0f);
}

