#include "Combo.h"
Combo::Combo()
	:m_CountCombo(0)
{
}

Combo::~Combo()
{
}


void Combo::UpdateCombo()
{
}

void Combo::DrawCombo()
{
}


void Combo::AddCombo(int num)
{
	m_CountCombo += num;
	if (m_CountCombo >= 999) {
		m_CountCombo = 999;
	}
}

void Combo:: ResetCombo()
{
	m_CountCombo = 0;
}

int Combo::GetCombo() const {
	return  m_CountCombo;
}


