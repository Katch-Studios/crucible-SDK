typedef struct {
    // Textures
    Texture2D Textures[6];
    int TextureNames[6];
    
    // Render Properties
    float transparency;
    bool translucent;
    bool flat;
    
    // Gameplay Properties
    bool solid;
    float hardness;
    unsigned char lightLevel;
    std::string name;
} BlockEntry;

Texture2D GlobalAtlas;

vector<BlockEntry> BlockEntries;

void NewBlock(std::vector<int> Textures, float transparency, bool translucent, bool flat, bool solid, float hardness, uint8_t light, std::string name) {
    BlockEntry newBlock;
    
    // Add textures to the new block entry
    for (int i = 0; i < Textures.size(); ++i) {
        newBlock.TextureNames[i] = Textures[i];
    }

    // Set Render Properties
    newBlock.transparency = transparency;
    newBlock.translucent = translucent;
    if(newBlock.transparency != 0) newBlock.translucent = true;
    newBlock.flat = flat;

    // Set Gameplay Properties
    newBlock.solid = solid;
    newBlock.hardness = hardness;
    newBlock.lightLevel = light;
    newBlock.name = name;

    // Add to BlockEntries
    BlockEntries.push_back(newBlock);
}
