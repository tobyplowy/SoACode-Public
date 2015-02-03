///
/// SphericalTerrainGenerator.h
/// Seed of Andromeda
///
/// Created by Benjamin Arnold on 17 Dec 2014
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
/// Generates spherical terrain and meshes for a planet. 
/// Each planet should own one.
///

#pragma once

#ifndef SphericalTerrainGenerator_h__
#define SphericalTerrainGenerator_h__

#include <Vorb/graphics/GLProgram.h>
#include <Vorb/graphics/FullQuadVBO.h>
#include <Vorb/graphics/GBuffer.h>
#include <Vorb/RPC.h>

#include "SphericalTerrainPatch.h"
#include "TerrainGenTextures.h"

class TerrainGenDelegate;
class RawGenDelegate;
class PlanetGenData;
namespace vorb {
    namespace core {
        namespace graphics {
            class TextureRecycler;
        }
    }
}

// Coordinate mapping for rotating 2d grid to quadcube positions
const i32v3 CubeCoordinateMappings[6] = {
    i32v3(0, 1, 2), //TOP
    i32v3(1, 0, 2), //LEFT
    i32v3(1, 0, 2), //RIGHT
    i32v3(0, 2, 1), //FRONT
    i32v3(0, 2, 1), //BACK
    i32v3(0, 1, 2) //BOTTOM
};

// Vertex Winding
// True when CCW
const bool CubeWindings[6] = {
    true,
    true,
    false,
    false,
    true,
    false
};

// Multipliers for coordinate mappings
const f32v3 CubeCoordinateMults[6] = {
    f32v3(1.0f, 1.0f, -1.0f), //TOP
    f32v3(1.0f, -1.0f, -1.0f), //LEFT
    f32v3(1.0f, 1.0f, -1.0f), //RIGHT
    f32v3(1.0f, 1.0f, -1.0f), //FRONT
    f32v3(1.0f, -1.0f, -1.0f), //BACK
    f32v3(1.0f, -1.0f, -1.0f) //BOTTOM
};

class TerrainVertex {
public:
    f32v3 position; //12
    f32v3 tangent; //24
    f32v2 texCoords; //32
    ColorRGB8 color; //35
    ui8 padding; //36
    ui8v2 normTexCoords; //38
    ui8 temperature; //39
    ui8 humidity; //40
};

class WaterVertex {
public:
    f32v3 position; //12
    f32v3 tangent; //24
    ColorRGB8 color; //27
    ui8 temperature; //28
    f32v2 texCoords; //36
    float depth; //40
};

class SphericalTerrainGenerator {
public:
    SphericalTerrainGenerator(float radius,
                              SphericalTerrainMeshManager* meshManager,
                              PlanetGenData* planetGenData,
                              vg::GLProgram* normalProgram,
                              vg::TextureRecycler* normalMapRecycler);
    ~SphericalTerrainGenerator();

    // Do this on the openGL thread
    void update();

    void generateTerrain(TerrainGenDelegate* data);

    void generateRawHeightmap(RawGenDelegate* data);

    void invokeRawGen(vcore::RPC* so) {
        // TODO(Ben): Change second param to false
        m_rawRpcManager.invoke(so, false);
    }

    void invokePatchTerrainGen(vcore::RPC* so) {
        m_patchRpcManager.invoke(so, false);
    }
private:

    void updatePatchGeneration();

    void updateRawGeneration();

    /// Generates mesh using heightmap
    void buildMesh(TerrainGenDelegate* data);

    ui8 calculateTemperature(float range, float angle, float baseTemp);

    ui8 calculateHumidity(float range, float angle, float baseHum);

    float computeAngleFromNormal(const f32v3& normal);

    void buildSkirts();

    void addWater(int z, int x);

    void tryAddWaterVertex(int z, int x);

    void tryAddWaterQuad(int z, int x);

    /// TODO: THIS IS REUSABLE
    void generateIndices(VGIndexBuffer& ibo, bool ccw);

    static const int PATCHES_PER_FRAME = 8;
    static const int RAW_PER_FRAME = 3;

    // PATCH_WIDTH * 4 is for skirts
    static const int VERTS_SIZE = PATCH_SIZE + PATCH_WIDTH * 4;
    static TerrainVertex verts[VERTS_SIZE];
    static WaterVertex waterVerts[VERTS_SIZE];
    static ui16 waterIndexGrid[PATCH_WIDTH][PATCH_WIDTH];
    static ui16 waterIndices[SphericalTerrainPatch::INDICES_PER_PATCH];
    static bool waterQuads[PATCH_WIDTH - 1][PATCH_WIDTH - 1];

    /// Meshing vars
    int m_index;
    int m_waterIndex;
    int m_waterIndexCount;
    float m_vertWidth;
    float m_radius;
    i32v3 m_coordMapping;
    f32v3 m_startPos;
    bool m_ccw;

    int m_dBufferIndex = 0; ///< Index for double buffering

    int m_patchCounter[2];
    TerrainGenTextures m_patchTextures[2][PATCHES_PER_FRAME];
    TerrainGenDelegate* m_patchDelegates[2][PATCHES_PER_FRAME];
    VGBuffer m_patchPbos[2][PATCHES_PER_FRAME];

    int m_rawCounter[2];
    TerrainGenTextures m_rawTextures[2][RAW_PER_FRAME];
    RawGenDelegate* m_rawDelegates[2][RAW_PER_FRAME];
    VGBuffer m_rawPbos[2][RAW_PER_FRAME];

    VGFramebuffer m_normalFbo = 0;
    ui32v2 m_heightMapDims;

    SphericalTerrainMeshManager* m_meshManager = nullptr;

    vcore::RPCManager m_patchRpcManager; /// RPC manager for mesh height maps
    vcore::RPCManager m_rawRpcManager; /// RPC manager for raw height data requests

    PlanetGenData* m_planetGenData = nullptr; ///< Planetary data
    vg::GLProgram* m_genProgram = nullptr;
    vg::GLProgram* m_normalProgram = nullptr;

    vg::TextureRecycler* m_normalMapRecycler = nullptr;

    static VGIndexBuffer m_cwIbo; ///< Reusable CW IBO
    static VGIndexBuffer m_ccwIbo; ///< Reusable CCW IBO

    VGUniform unCornerPos;
    VGUniform unCoordMapping;
    VGUniform unPatchWidth;
    VGUniform unHeightMap;
    VGUniform unWidth;
    VGUniform unTexelWidth;

    vg::FullQuadVBO m_quad;

    static float m_heightData[PATCH_HEIGHTMAP_WIDTH][PATCH_HEIGHTMAP_WIDTH][4];
};

#endif // SphericalTerrainGenerator_h__