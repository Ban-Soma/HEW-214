#include "Energy.h"
#include <algorithm>


void Energy::Update(float deltaTime)
{
	if (!m_isOverheated)return;
	//回復
	const float recoverySpeed = MAX_ENERGY / OVERHEAT_TIME;

	m_energy += recoverySpeed * deltaTime;
	m_energy = (std::min)(m_energy, MAX_ENERGY);
	if (m_energy >= MAX_ENERGY)
	{
		m_energy = MAX_ENERGY;
		m_isOverheated = false;
	}
}

bool Energy::ConsumeDash()
{
	if (m_isOverheated)	return false;
	if (m_energy < DASH_COST)return false;
	m_energy -= DASH_COST;
	return true;
}

void Energy::OnDashEnd()
{
	if (m_isOverheated) return;

	if (m_energy <= 0.0f)
	{
		m_energy = 0.0f;
		m_isOverheated = true;
	}
}

void Energy::OnEnemyKilled(int count)
{
	if (count <= 0)return;

	m_energy += KILL_RECOVERY * count;
	m_energy = (std::min)(m_energy, MAX_ENERGY);

	//オーバーヒート中でも満タンになれば復帰
	if (m_isOverheated && m_energy >= MAX_ENERGY)
	{
		m_isOverheated = false;
	}

	
}

float Energy::GetEnergy() const
{
	return m_energy;
}

float Energy::GetEnergyRatio() const
{
	return 0.0f;
}

bool Energy::IsOverheated() const
{
	return m_isOverheated;
}
