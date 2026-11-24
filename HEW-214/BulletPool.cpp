
#include "BulletPool.h"

void BulletPool::Fire(
    DirectX::XMFLOAT3 pos,
    DirectX::XMFLOAT3 direction)
{
    for (auto& bullet : m_Bullets)
    {
        if (bullet.IsActive())
            continue;

        bullet.Reset(pos, direction);
        return;
    }
}

void BulletPool::Update()
{
    for (auto& bullet : m_Bullets)
    {
        if (!bullet.IsActive())
            continue;

        bullet.Update();
    }
}

void BulletPool::Draw()
{
    for (auto& bullet : m_Bullets)
    {
        if (!bullet.IsActive())
            continue;

        bullet.Draw();
    }
}

int BulletPool::GetActiveCount() const
{
    int count = 0;

    for (const auto& bullet : m_Bullets)
    {
        if (bullet.IsActive())
            count++;
    }

    return count;
}
