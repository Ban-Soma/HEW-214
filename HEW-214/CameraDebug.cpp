#include "CameraDebug.h"
#include "Input.h"
#include "Defines.h"
#include "Player.h"

CameraDebug::CameraDebug()
	:m_radXZ(0.0f)
	,m_radY(0.0f)
	,m_radius(METER(4.0f))
{
	m_radY = 19.8f;
}

CameraDebug::~CameraDebug()
{
}

void CameraDebug::Update()
{
	//直接数字で一道速度を設定するより、
	//一秒間の移動距離から１フレームの速度が計算できるとよい
	const float speed = METER(2.0f) / FPS;//一秒間で5メートル進む
	
	//カメラ位置の移動
	//回り込みの処理
	if (IsKeyPress(VK_LEFT)) { m_radXZ += speed; }
	if (IsKeyPress(VK_RIGHT)) { m_radXZ -= speed; }
	/*if (IsKeyPress(VK_UP)) { m_radY += speed; }
	if (IsKeyPress(VK_DOWN)) { m_radY -= speed; }*/

	//カメラの距離を替える処理
	if (IsKeyPress('Q')) { m_radius += speed; }
	if (IsKeyPress('E')) { m_radius -= speed; }
	
	//カメラ位置の計算
	m_pos.x = cos(m_radY) * -sin(m_radXZ) * m_radius + m_look.x;
	m_pos.y = sin(m_radY) * m_radius + m_look.y;
	m_pos.z = cos(m_radY) * -cos(m_radXZ) * m_radius + m_look.z;

}
