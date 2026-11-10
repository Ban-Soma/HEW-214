#pragma once
//カメラの基本クラス
#include <DirectXMath.h>

class Camera {
protected:
	DirectX::XMFLOAT3 m_pos; //座標
	DirectX::XMFLOAT3 m_look; //注視点
	DirectX::XMFLOAT3 m_up; //上方ベクトル
	float m_fovy; //画角
	float m_aspect; //アスペクト比
	float m_near; //ニアグリップ
	float m_far; //ファーグリップ
public:
	//コンストラクタ
	Camera();
	//デストラクタ
	virtual ~Camera(); //継承を行うならデストラクタにはvirtualをつける
	//更新処理
	virtual void Update() = 0;
	//ビュー変換行列の取得（デフォルトでは転置済みの行列を計算
	DirectX::XMFLOAT4X4 GetViewMatrix(bool transpose = true);
	//プロジェクション変換行列の取得(デフォルトでは転置済みの行列を計算
	DirectX::XMFLOAT4X4 GetProjectionMatrix(bool transpose = true);
	//座標の取得
	DirectX::XMFLOAT3 GetPos();
	//注視点の取得
	DirectX::XMFLOAT3 GetLook();

	//注視点のセット
	void SetLook(DirectX::XMFLOAT3);

};