#ifndef __SHADER_LIST_H__
#define __SHADER_LIST_H__

#include "Shader.h"
#include "Model.h"


class ShaderList
{
	// 定義
public:
	enum VSKind
	{
		VS_WORLD, // SetWorld/View/Projection
		VS_ANIME, // SetWorld/View/Projection, SetBones
		VS_KIND_MAX
	};
	enum PSKind
	{
		PS_UNLIT,		// 画像貼り付け		SetMaterial
		PS_LAMBERT,		// 拡散光+環境光		SetMaterial, SetLight
		PS_SPECULAR,	// LAMBERT+鏡面反射	SetMaterial, SetLight, SetCameraPos
		PS_TOON,		// アニメ表現		SetMaterial, SetLight, SetCameraPos
		PS_FOG,			// 霧、遠くの背景		SetMaterial, SetLight, SetFog
		PS_KIND_MAX
	};


public:
	ShaderList();
	~ShaderList();

	static void Init();
	static void Uninit();

	// シェーダー設定
	static VertexShader* GetVS(VSKind vs);
	static PixelShader* GetPS(PSKind ps);

	// 定数バッファへの設定
	static void SetWorld(const DirectX::XMFLOAT4X4& world);
	static void SetView(const DirectX::XMFLOAT4X4& view);
	static void SetProjection(const DirectX::XMFLOAT4X4& proj);
	static void SetBones(DirectX::XMFLOAT4X4* bones200);
	static void SetMaterial(const Model::Material& material);
	static void SetLight(DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 dir);
	static void SetCameraPos(const DirectX::XMFLOAT3 pos);
	static void SetFog(DirectX::XMFLOAT4 color, float start, float range);
	
private:
	static void MakeWorldVS();
	static void MakeAnimeVS();
	static void MakeUnlitPS();
	static void MakeLambertPS();
	static void MakeSpecularPS();
	static void MakeToonPS();
	static void MakeFogPS();

private:
	static VertexShader* m_pVS[VS_KIND_MAX];
	static PixelShader* m_pPS[PS_KIND_MAX];
	static DirectX::XMFLOAT4X4 m_wvp[4];
};

#endif // __SHADER_LIST_H__