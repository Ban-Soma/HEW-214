#include "GameObject.h"

GameObject::GameObject()
	:m_pos(0.0f,0.0f,0.0f)
	,m_scale(1.0f,1.0f,1.0f)
	,m_rotation(0.0f,0.0f,0.0f)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update()//çXêVèàóù
{
}

void GameObject::Draw()//ï`âÊèàóù
{
}

void GameObject::SetPos(float x, float y, float z)
{
	m_pos = DirectX::XMFLOAT3(x, y, z);
}

void GameObject::SetScale(float x, float y, float z)
{
	m_scale = DirectX::XMFLOAT3(x, y, z);
}

void GameObject::SetRotation(float x, float y, float z)
{
	m_rotation = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 GameObject::GetPos()
{
	return m_pos;
}

DirectX::XMFLOAT3 GameObject::GetScale()
{
	return m_scale;
}

DirectX::XMFLOAT3 GameObject::GetRotation()
{
	return m_rotation;
}