typedef struct {
    Vector3 Position;
    Vector3 Size;
    Vector3 Velocity;
    bool Grounded;
    float Yaw;
    float Pitch;
}Player;

Vector3 DefaultPlayerSize = (Vector3){1,1.5,1};

Player SpawnPlayer(Vector3 Pos) {
    Player NewPlayer;
    NewPlayer.Position = Pos;
    NewPlayer.Size = DefaultPlayerSize;
    NewPlayer.Velocity = (Vector3){0,0,0};
    NewPlayer.Grounded = false;
    NewPlayer.Yaw = 0.0f;
    NewPlayer.Pitch = 0.0f;
    return NewPlayer;
}

Vector2 GetChunk(Player player) {
    return (Vector2){floor(player.Position.x / 16), floor(player.Position.z / 16)};
}

Vector2 GetLocalXZ(Player player) {
    return (Vector2){(float)(((int)floor(player.Position.x) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH), (float)(((int)floor(player.Position.z) % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH)};
}

int BlockAtPos(Vector3 Pos,vector<Chunk>& chunks) {
    Player temp;
    temp.Position = Pos;
    Vector2 GChunk = GetChunk(temp);
    Vector2 Local = GetLocalXZ(temp);
    
    int ChunkIndex = GChunk.y + GChunk.x * WORLD_SIZE;
    int LocalIndex = Local.x + Local.y * CHUNK_WIDTH + (int)floor(Pos.y) * CHUNK_WIDTH * CHUNK_WIDTH;
    
    if(ChunkIndex < 0 || ChunkIndex >= (int)chunks.size()) return 0;
    
    if(Local.x < 0 || Local.x >= CHUNK_WIDTH) return 0;
    if(Local.y < 0 || Local.y >= CHUNK_WIDTH) return 0;
    if(Pos.y < 0 || Pos.y >= CHUNK_HEIGHT) return 0;
    
    Chunk chunk = chunks[ChunkIndex];
    
    return chunk.blocks[LocalIndex];
}

void SetBlockAtPos(Vector3 Pos,vector<Chunk>& chunks,int id) {
    Player temp;
    temp.Position = Pos;
    Vector2 GChunk = GetChunk(temp);
    Vector2 Local = GetLocalXZ(temp);
    
    int ChunkIndex = GChunk.y + GChunk.x * WORLD_SIZE;
    int LocalIndex = Local.x + Local.y * CHUNK_WIDTH + (int)floor(Pos.y) * CHUNK_WIDTH * CHUNK_WIDTH;
    
    if(ChunkIndex < 0 || ChunkIndex >= (int)chunks.size()) return;
    
    if(Local.x < 0 || Local.x >= CHUNK_WIDTH) return;
    if(Local.y < 0 || Local.y >= CHUNK_WIDTH) return;
    if(Pos.y < 0 || Pos.y >= CHUNK_HEIGHT) return;
    
    Chunk& chunk = chunks[ChunkIndex];
    
    chunk.blocks[LocalIndex] = id;
    chunk.Properties[LocalIndex].lights = BlockEntries[id].lightLevel;
    
    BuildChunkMesh(chunk,false,chunks);
    BuildChunkMesh(chunk,true,chunks);
}