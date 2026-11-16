#pragma once
#include <DirectXMath.h>

class  GameObject
{
protected:
	DirectX::XMFLOAT3 m_pos;//座標
	DirectX::XMFLOAT3 m_scale;//サイズ
	DirectX::XMFLOAT3 m_rotation;//回転
public:
	GameObject();//コンストラクタ
	virtual ~GameObject();//デストラクタ
	virtual void Update();//更新処理
	virtual void Draw();//描画処理
	
	//ゲッター、セッター
	void SetPos(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	DirectX::XMFLOAT3 GetPos();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRotation();
};