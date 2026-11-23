#pragma once
class Combo {
private:
	int m_CountCombo;

public:
	Combo();
	~Combo();

	void UpdateCombo();
	void DrawCombo();

	void AddCombo(int num);
	void ResetCombo();

	int GetCombo() const;

};
