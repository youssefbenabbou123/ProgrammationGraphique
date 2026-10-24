#ifndef TERRAIN_HEIGHT_H
#define TERRAIN_HEIGHT_H
#include <algorithm>

const int TERRAIN_GRID_W = 96;
const int TERRAIN_GRID_D = 60;
const float TERRAIN_WIDTH_X = 160.0f;
const float TERRAIN_DEPTH_Z = 100.0f;
const float TERRAIN_HALF_X = 80.0f;
const float TERRAIN_HALF_Z = 50.0f;

extern const float gTerrainHeightTable[60][96];

inline float getTerrainHeight(float x, float z)
{
    float u = (x + TERRAIN_HALF_X) / TERRAIN_WIDTH_X;
    float v = (z + TERRAIN_HALF_Z) / TERRAIN_DEPTH_Z;
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    float fx = u * (TERRAIN_GRID_W - 1);
    float fz = v * (TERRAIN_GRID_D - 1);
    int x0 = (int)fx;
    int z0 = (int)fz;
    int x1 = std::min(TERRAIN_GRID_W - 1, x0 + 1);
    int z1 = std::min(TERRAIN_GRID_D - 1, z0 + 1);
    float tx = fx - x0;
    float tz = fz - z0;
    float h00 = gTerrainHeightTable[z0][x0];
    float h10 = gTerrainHeightTable[z0][x1];
    float h01 = gTerrainHeightTable[z1][x0];
    float h11 = gTerrainHeightTable[z1][x1];
    float h0 = h00 * (1.0f - tx) + h10 * tx;
    float h1 = h01 * (1.0f - tx) + h11 * tx;
    return h0 * (1.0f - tz) + h1 * tz;
}

#endif
