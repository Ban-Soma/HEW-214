#include "Defines.h"
#include "Camera.h"

Camera::Camera()
	:m_pos{0.0f,0.0f,-2.0f}
	,m_look{ 0.0f,0.0f,0.0f}
	,m_up{0.0f,1.0f,0.0f}
	,m_fovy(DirectX::XMConvertToRadians(60.0f))//角度をラジアン角に変更する関数
	,m_aspect(16.0f / 9.0f)
	,m_near(CMETER(30.0f))
	,m_far(METER(100.0f))
{
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix(bool transpose)
{
	//ビュー変換行列の作成
	DirectX::XMMATRIX view;

	view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(m_pos.x,m_pos.y,m_pos.z,0.0f),								
									 DirectX::XMVectorSet(m_look.x,m_look.y,m_look.z,0.0f),
									 DirectX::XMVectorSet(m_up.x,m_up.y,m_up.z,0.0f));
	if (transpose)
	{
		view = DirectX::XMMatrixTranspose(view);
	}

	//読み取りデータに変換
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat,view);

	//計算結果を取得
	return mat;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix(bool transpose)
{
	//プロジェクション変換行列の作成
	DirectX::XMMATRIX proj;
	proj = DirectX::XMMatrixPerspectiveFovLH(m_fovy,m_aspect,m_near,m_far);
	if (transpose)
	{
		proj = DirectX::XMMatrixTranspose(proj);
	}

	//読み取りデータに変換
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, proj);

	//計算結果を取得
	return mat;
}

DirectX::XMFLOAT3 Camera::GetPos()
{
	return m_pos;
}

DirectX::XMFLOAT3 Camera::GetLook()
{
	return m_look;
}

void Camera::SetLook(DirectX::XMFLOAT3 SetLook)
{
	m_look = SetLook;
}
