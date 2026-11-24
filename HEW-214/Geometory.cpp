#include "Geometory.h"

MeshBuffer* Geometory::m_pBox;
MeshBuffer* Geometory::m_pCylinder;
MeshBuffer* Geometory::m_pSphere;
MeshBuffer* Geometory::m_pLines;
Shader* Geometory::m_pVS;
Shader* Geometory::m_pPS;
Shader* Geometory::m_pLineShader[2];
DirectX::XMFLOAT4X4 Geometory::m_WVP[3];
void* Geometory::m_pLineVtx;
int Geometory::m_lineCnt = 0;

Shader* Geometory::m_pTexturePS = nullptr;
ID3D11SamplerState* Geometory::m_pSampler = nullptr;
ID3D11ShaderResourceView* Geometory::m_pCurrentTexture = nullptr;

void Geometory::Init()
{
	for (int i = 0; i < 3; ++i)
		DirectX::XMStoreFloat4x4(&m_WVP[i], DirectX::XMMatrixIdentity());
	MakeBox();
	MakeCylinder();
	MakeSphere();
	MakeVS();
	MakePS();
	MakeTexturePS();
	MakeLineShader();
	MakeLine();
}
void Geometory::Uninit()
{
	SAFE_DELETE_ARRAY(m_pLineVtx);
	SAFE_DELETE(m_pLineShader[1]);
	SAFE_DELETE(m_pLineShader[0]);
	SAFE_DELETE(m_pPS);
	SAFE_DELETE(m_pVS);
	SAFE_DELETE(m_pLines);
	SAFE_DELETE(m_pSphere);
	SAFE_DELETE(m_pCylinder);
	SAFE_DELETE(m_pBox);
	SAFE_DELETE(m_pTexturePS);
	SAFE_RELEASE(m_pSampler);
	m_pCurrentTexture = nullptr;
}

void Geometory::SetWorld(DirectX::XMFLOAT4X4 world)
{
	m_WVP[0] = world;
}
void Geometory::SetView(DirectX::XMFLOAT4X4 view)
{
	m_WVP[1] = view;
}
void Geometory::SetProjection(DirectX::XMFLOAT4X4 proj)
{
	m_WVP[2] = proj;
}

void Geometory::AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
	if (m_lineCnt < MAX_LINE_NUM)
	{
		LineVertex* pVtx = reinterpret_cast<LineVertex*>(m_pLineVtx);
		pVtx[m_lineCnt * 2 + 0] = { start.x, start.y, start.z, color.x, color.y, color.z, color.w};
		pVtx[m_lineCnt * 2 + 1] = { end.x, end.y, end.z, color.x, color.y, color.z, color.w };
		++m_lineCnt;
	}
}
void Geometory::DrawLines()
{
	m_pLineShader[0]->WriteBuffer(0, m_WVP);
	m_pLineShader[0]->Bind();
	m_pLineShader[1]->Bind();
	m_pLines->Write(m_pLineVtx);
	m_pLines->Draw(m_lineCnt * 2);
	m_lineCnt = 0;
}

void Geometory::DrawBox()
{
	if (m_pBox == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	if (m_pCurrentTexture != nullptr && m_pTexturePS != nullptr && m_pSampler != nullptr)
	{
		m_pTexturePS->Bind();
		ID3D11ShaderResourceView* srv = m_pCurrentTexture;
		GetContext()->PSSetShaderResources(0, 1, &srv);
		GetContext()->PSSetSamplers(0,1,&m_pSampler);
	}
	else
	{
		m_pPS->Bind();

		ID3D11ShaderResourceView* nullSRV = nullptr;
		GetContext()->PSSetShaderResources(0, 1, &nullSRV);
	}
	m_pBox->Draw();
}
void Geometory::DrawCylinder()
{
	if (m_pCylinder == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pCylinder->Draw();
}
void Geometory::DrawSphere()
{
	if (m_pSphere == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pSphere->Draw();
}

void Geometory::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pCurrentTexture = texture;
}

void Geometory::MakeVS()
{
const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.uv = vin.uv;
	return vout;
})EOT";

	m_pVS = new VertexShader();
	m_pVS->Compile(VSCode);
}

void Geometory::MakePS()
{
#ifdef _DEBUG
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	float4 color = float4(1,1,1,1);
	float2 halfGrid = floor(abs(pin.uv) * 2.0f);
	float2 quatGrid = floor(abs(pin.uv) * 8.0f);

	float half = fmod(halfGrid.x + halfGrid.y, 2.0f);
	float quat = fmod(quatGrid.x + quatGrid.y, 2.0f);

	color.rgb = ((half * 0.1f) * quat + 0.45f) + (1 - quat) * 0.05f;
	return color;
})EOT";
#else
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
})EOT";
#endif

	m_pPS = new PixelShader();
	m_pPS->Compile(PSCode);
}
void Geometory::MakeLineShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float4 color : COLOR0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.color = vin.color;
	return vout;
})EOT";

	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return pin.color;
})EOT";

	m_pLineShader[0] = new VertexShader();
	m_pLineShader[0]->Compile(VSCode);
	m_pLineShader[1] = new PixelShader();
	m_pLineShader[1]->Compile(PSCode);
}
void Geometory::MakeLine()
{
	m_pLineVtx = new LineVertex[MAX_LINE_NUM * 2];
	m_lineCnt = 0;
	MeshBuffer::Description desc = {};
	desc.pVtx = m_pLineVtx;
	desc.vtxCount = MAX_LINE_NUM * 2;
	desc.vtxSize = sizeof(LineVertex);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	desc.isWrite = true;
	m_pLines = new MeshBuffer();
	m_pLines->Create(desc);
}

void Geometory::MakeTexturePS()
{
	const char* PSCode=R"EOT(
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_IN pin) : SV_TARGET0
{
    return g_Texture.Sample(g_Sampler, pin.uv);
}

)EOT";

	m_pTexturePS = new PixelShader();

	HRESULT hr = m_pTexturePS->Compile(PSCode);

	if (FAILED(hr))
	{
		delete m_pTexturePS;
		m_pTexturePS = nullptr;
		return;
	}
	D3D11_SAMPLER_DESC desc = {};

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = GetDevice()->CreateSamplerState(
		&desc,
		&m_pSampler
	);

	if (FAILED(hr))
	{
		delete m_pTexturePS;
		m_pTexturePS = nullptr;
	}
}
