Texture2D EmptyTexture[6] = {};

#define PATH_BUFFER 256

Texture2D LoadAtlas(char *name) {
    char path[PATH_BUFFER];
    snprintf(path, PATH_BUFFER, "Assets/Packs/%s/terrain.png", name);
    return LoadTexture(path);
}

string packname;

void LoadPack(char *name) {
    char path[PATH_BUFFER];
    packname = name;
    
    Texture2D Atlas = LoadAtlas(name);
    GlobalAtlas = Atlas;
}

Texture2D LoadPackIcon(char *name) {
    char path[PATH_BUFFER];
    snprintf(path, PATH_BUFFER, "Assets/Packs/%s/Icon.png", name);
    return LoadTexture(path);
}

Texture2D LoadTextureFromPack(char *name) {
    char path[PATH_BUFFER];
    snprintf(path, PATH_BUFFER, "Assets/Packs/%s/%s", packname.c_str(), name);
    return LoadTexture(path);
}

Sound LoadSFXFromPack(char *name) {
    char path[PATH_BUFFER];
    snprintf(path, PATH_BUFFER, "Assets/Packs/%s/Audio/%s", packname.c_str(), name);
    return LoadSound(path);
}

Music LoadSongFromPack(char *name) {
    char path[PATH_BUFFER];
    snprintf(path, PATH_BUFFER, "Assets/Packs/%s/Audio/Music/%s", packname.c_str(), name);
    return LoadMusicStream(path);
}