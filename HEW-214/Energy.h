#pragma once
#include "Defines.h"
class Energy
{
public:
	
	void Update(float deltaTime);

	void ConsumeDash();
	void OnDashEnd();

	void OnEnemyKilled(int count = 1);

	float GetEnergy()const;
	float GetEnergyRatio()const;

	bool IsOverheated()const;
private:
	float m_energy=100.0f;
	bool m_isOverheated=false;
	bool m_DashPending = false;
};