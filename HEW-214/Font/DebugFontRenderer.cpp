#include "DebugFontRenderer.h"

#include "TextMesh.h"
#include "TextureAtlas.h"

void DebugFontRenderer::DrawTextMesh(
    const TextMesh& mesh,
    const TextureAtlas& atlas)
{
    m_lastVertexCount =
        mesh.vertices.size();

    m_lastIndexCount =
        mesh.indices.size();

    m_lastAtlasWidth =
        atlas.GetWidth();

    m_lastAtlasHeight =
        atlas.GetHeight();
}