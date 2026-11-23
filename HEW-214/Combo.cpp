#include "Combo.h"


//コンボ数をカウントするための変数
int g_CountCombo = 0;
void InitCombo()
{

}

void UpdateCombo()
{
}

void DrawCombo()
{
}

void UninitCombo()
{
}

void AddCombo(int num)
{
	g_CountCombo += num;
	if (g_CountCombo >= 999) {
		g_CountCombo = 999;
	}
}

void ResetCombo()
{
	g_CountCombo = 0;
}
