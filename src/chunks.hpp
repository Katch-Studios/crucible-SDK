#include "blocks.hpp"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128

#ifdef PLATFORM_WEB
    #define WORLD_SIZE 8
#else
    #define WORLD_SIZE 16
#endif

typedef struct {
    unsigned char lights;
} BlockProperties;

typedef struct {
    vector<int> blocks;
    vector<BlockProperties> Properties;
    Mesh ChunkMesh;
    Mesh TranslucentChunkMesh;
    bool ChunkUpdated;
    int ChunkX;
    int ChunkZ;
} Chunk;

typedef struct {
    bool hit;
    Vector3 point;
    Vector3 normal;
    int blockX, blockY, blockZ;
    int face;
    float distance;
} RaycastHit;

bool IsFaceCovered(const Chunk& chunk, int x, int y, int z, int face, const vector<Chunk>& chunks) {
    int otherX = x;
    int otherY = y;
    int otherZ = z;

    switch (face) {
        case 1: otherZ++; break;
        case 2: otherZ--; break;
        case 3: otherX++; break;
        case 4: otherX--; break;
        case 5: otherY++; break;
        case 6: otherY--; break;
        default: return false;
    }

    if (otherY < 0 || otherY >= CHUNK_HEIGHT) return false;

    int neighborCX = chunk.ChunkX;
    int neighborCZ = chunk.ChunkZ;

    if (otherX < 0) {
        neighborCX -= 1;
        otherX = CHUNK_WIDTH - 1;
    } else if (otherX >= CHUNK_WIDTH) {
        neighborCX += 1;
        otherX = 0;
    }

    if (otherZ < 0) {
        neighborCZ -= 1;
        otherZ = CHUNK_WIDTH - 1;
    } else if (otherZ >= CHUNK_WIDTH) {
        neighborCZ += 1;
        otherZ = 0;
    }

    const Chunk* targetChunk = &chunk;
    if (neighborCX != chunk.ChunkX || neighborCZ != chunk.ChunkZ) {
        targetChunk = nullptr;
        for (int i = 0; i < (int)chunks.size(); i++) {
            if (chunks[i].ChunkX == neighborCX && chunks[i].ChunkZ == neighborCZ) {
                targetChunk = &chunks[i];
                break;
            }
        }
        if (targetChunk == nullptr) return false;
    }

    int Index = x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_WIDTH;
    int otherIndex = otherX + otherZ * CHUNK_WIDTH + otherY * CHUNK_WIDTH * CHUNK_WIDTH;

    int blockId2 = chunk.blocks[Index];
    int blockId = targetChunk->blocks[otherIndex];

    if (blockId <= 0 || blockId >= (int)BlockEntries.size()) return false;

    BlockEntry& entry = BlockEntries[blockId];

    if (entry.translucent || entry.transparency > 0) {
        if (blockId == blockId2) return true;
        return false;
    }

    return true;
}

bool IsBlockCovered(const Chunk& chunk, int x, int y, int z, const vector<Chunk>& chunks) {
    for (int face = 1; face <= 6; face++) {
        if (!IsFaceCovered(chunk, x, y, z, face, chunks)) {
            return false;
        }
    }
    return true;
}

int GetBlockIndex(int x, int y, int z) {
    return x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_WIDTH;
}

bool IsValidBlockCoord(int x, int y, int z) {
    return x >= 0 && x < CHUNK_WIDTH &&
           y >= 0 && y < CHUNK_HEIGHT &&
           z >= 0 && z < CHUNK_WIDTH;
}

bool IsBlockSolid(const Chunk& chunk, int x, int y, int z) {
    if (!IsValidBlockCoord(x, y, z)) return false;
    return chunk.blocks[GetBlockIndex(x, y, z)] != 0;
}

char GetDefaultLightLevel(int y) {
    if(y>31) return 9;
    if(y<22) return 0;
    return y -22;
}

std::vector<BlockProperties> UpdateLight(Chunk& chunk, const vector<Chunk>& chunks) {
    int size = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH;
    chunk.Properties.assign(size, {0});
    
    auto getIndex = [](int x, int y, int z) {
        return x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_WIDTH;
    };
    
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            bool inShadow = false;
            int currentSky = 9;
            
            for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
                int index = getIndex(x, y, z);
                int blockId = chunk.blocks[index];
                
                int blockLight = BlockEntries[blockId].lightLevel;
                
                int skyLight = inShadow ? 6 : currentSky;
                
                if(inShadow) skyLight = GetDefaultLightLevel(y);
                if(skyLight == 9) skyLight = inShadow ? 6 : currentSky;
                
                if (BlockEntries[blockId].translucent || BlockEntries[blockId].transparency > 0) {
                    currentSky = std::max(currentSky - 1, 0);
                } else if (blockId != 0) {
                    inShadow = true;
                    currentSky = 0;
                }
                
                chunk.Properties[index].lights = std::max((unsigned char)skyLight, (unsigned char)blockLight);
            }
        }
    }

    return chunk.Properties;
}


Chunk InitChunk(int cx, int cz) {
    Chunk chunk;
    chunk.ChunkMesh = {0};
    chunk.TranslucentChunkMesh = {0};
    chunk.ChunkUpdated = false;
    
    chunk.blocks.resize(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH);
    chunk.Properties.resize(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH);
    
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_WIDTH; z++) {

                int index = x +
                            z * CHUNK_WIDTH +
                            y * CHUNK_WIDTH * CHUNK_WIDTH;
                
                float fx = (float)(x + cx * CHUNK_WIDTH);
                float fz = (float)(z + cz * CHUNK_WIDTH);
                
                float ground_height_f = 32.0f 
                    + 3.0f * sin(fx * 0.036) 
                    + 6.0f * sin(fx * 0.0015f) * cos(fz * 0.12f)
                    + 2.0f * sin((fx + fz) * 0.0036f);
                
                int ground_height = (int)ground_height_f;

                if (y <= ground_height)  chunk.blocks[index] = 1;
                
                chunk.Properties[index].lights = GetDefaultLightLevel(y);    
                
            }
        }
    }
    
    chunk.ChunkX = cx;
    chunk.ChunkZ = cz;
    
    return chunk;
}

void InitChunks(vector<Chunk>& chunks) {
    for(int x = 0; x < WORLD_SIZE; x++) {
            for(int z = 0; z < WORLD_SIZE; z++) {
                int index = x + z * WORLD_SIZE;
                chunks.push_back(InitChunk(x, z));
        }
    }
}