//Default C++ Libs
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <string>
#include <cstdio>
#include <queue>
#include <iterator>

using namespace std;

//Raylib Stuffs
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

//OutSource Imports

//InHouse Imports
#include "chunks.hpp"
#include "mesh.hpp"
#include "textures.hpp"
#include "player.hpp"
//#include "rendering.hpp" (Deprecated)

int screenWidth = 800;
int screenHeight = 600;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Foundry");
    SetTargetFPS(60);
    SetExitKey(NULL);
    
    NewBlock({}, 0.0f, false, false, false, 0.0f, 0, "Air");
    NewBlock({0,0}, 0.0f, false, false, true, 0.25f, 0, "Template");
    
    LoadPack((char*)"pack");
    
    vector<Chunk> Chunks;
    
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 8.0f, 64.0f, 8.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    Player player;
    player = SpawnPlayer((Vector3){ 32.0f, 64.0f, 32.0f });
    
    Material mat = LoadMaterialDefault();
    Material TranslucentChunkMat = LoadMaterialDefault();
    
    Shader TanslucentShader = LoadShader("Assets/alphacut.vs", "Assets/alphacut.fs");
    
    bool debug = false;
    bool paused = false;
    bool pausedinit = false; 
    bool init = true;
        
    while (!WindowShouldClose()) {
        if(init) {
            BeginDrawing();
            ClearBackground(GRAY);
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            SetWindowSize(GetScreenWidth(), GetScreenHeight());
            DrawText("Loading...", 10,10,20, WHITE);
            EndDrawing();
            InitChunks(Chunks);
            BuildChunkMeshes(Chunks);
            init = false;
        }
        if(!paused) {
            UpdateCamera(&camera, CAMERA_FREE);
            SetMousePosition(screenWidth/2,screenHeight/2);
            DisableCursor();
            HideCursor();
            pausedinit = true;
        } else {
            if(pausedinit) {
                ShowCursor();
                EnableCursor();
                pausedinit = false;
            }
        }
        
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        SetWindowSize(GetScreenWidth(), GetScreenHeight());
        
        if(IsKeyPressed(KEY_F3)) debug = !debug;
        if(IsKeyPressed(KEY_ESCAPE)) paused = !paused;
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        rlEnableDepthTest(); 
        
        mat.maps[MATERIAL_MAP_DIFFUSE].texture = GlobalAtlas;
        TranslucentChunkMat.maps[MATERIAL_MAP_DIFFUSE].texture = GlobalAtlas;
        
        for(int x = 0; x < WORLD_SIZE; x++) {
            for(int z = 0; z < WORLD_SIZE; z++) {
                int index = x + z * WORLD_SIZE;
                Chunk chunk = Chunks[index];
                if (chunk.ChunkMesh.vertexCount > 0) DrawMesh(chunk.ChunkMesh, mat, MatrixTranslate(chunk.ChunkX * CHUNK_WIDTH, 0.0f, chunk.ChunkZ * CHUNK_WIDTH));
                TranslucentChunkMat.shader = TanslucentShader;
                if (chunk.TranslucentChunkMesh.vertexCount > 0) DrawMesh(chunk.TranslucentChunkMesh, TranslucentChunkMat, MatrixTranslate(chunk.ChunkX * CHUNK_WIDTH, 0.0f, chunk.ChunkZ * CHUNK_WIDTH));
            }
        }
        
        EndMode3D();
        
        DrawText("CrucibleSDK Template",5,10,20,WHITE);
        DrawText("(Unfinished Build)",5,40,20,YELLOW);
        
        if(debug) {
            DrawText(TextFormat("XYZ: %.2f, %.2f, %.2f", player.Position.x, player.Position.y, player.Position.z),5,80,20,WHITE);
            DrawText(TextFormat("Local XZ: %.1f, %.1f", fmod(player.Position.x, 16), fmod(player.Position.z, 16)),5,100,20,WHITE);
            DrawText(TextFormat("Chunk XZ: %.0f, %.0f", player.Position.x / 16, player.Position.z / 16),5,120,20,WHITE);
            DrawFPS(5,60);
        }
        
        EndDrawing();
    }   
}