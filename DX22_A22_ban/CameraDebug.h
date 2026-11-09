#pragma once
#include "Camera.h"

//デバッグ用カメラ
class CameraDebug : public Camera {
private:
	float m_radXZ; //カメラの横回転
	float m_radY; //カメラの縦回転
	float m_radius; //カメラと注視点の距離
public:
	//コンストラクタ
	CameraDebug();
	//デストラクタ
	~CameraDebug();
	//更新処理
	void Update() final;
};