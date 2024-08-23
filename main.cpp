//the image of the ship should not have a whole white square
#include <cstdio>
#include <cstdlib>
#include <raylib.h>
#include <string>
#include <vector>
#include <random>
#include <iostream>

std::random_device rd;
std::mt19937 gen(rd());
const int speed = 5;

enum Status{FREE,FIGHT,NEWMAP};
Status status = FREE;

struct Player_info{
    Rectangle player;
    Texture2D texture;
    Vector2 origin;
    float rotation;
};

struct Map_unit{
    std::vector<Rectangle> recs;
    Map_unit* top;
    Map_unit* bot;
    Map_unit* left;
    Map_unit* right;
};

Map_unit* current_map;

int width;
int height;

void drawFight(){
    Image img = LoadImage("enemy.png"); 
    Texture2D texture = LoadTextureFromImage(img);
    img = LoadImage("back.png"); 
    Texture2D texture2 = LoadTextureFromImage(img);

    DrawTexture(texture, 1000,100, WHITE);
    DrawTexture(texture2, 100,500, WHITE);

}

std::vector<Rectangle> generateMap(){
    std::uniform_int_distribution<> distr(5, 20);
    int random_number = distr(gen);
    std::vector<Rectangle> eng;

    for(int i=0;i<random_number;i++){
        std::uniform_int_distribution<> distr(100, 1000);
        int x = distr(gen);
        int y = distr(gen);
        Rectangle rec = {(float)x,(float)y,50,50};
        eng.push_back(rec); 
    }

    return eng;
}

void drawMap(){
    for(const auto& rec: current_map->recs){
        DrawRectangleRec(rec, MAROON);
    }
}

//theres no way thats clean
void switchMap(Rectangle& player){
    if(player.y > height){
        if(!current_map->bot){
            Map_unit* new_map = new Map_unit();
            new_map->recs = generateMap();
            new_map->top = current_map;
            new_map->left = nullptr;
            new_map->right = nullptr;
            new_map->bot = nullptr;        
            current_map->bot = new_map;        
            current_map = new_map;        
        }else{
            current_map = current_map->bot; 
        }
        player.y = 1;
    }else if(player.y < 0){
        if(!current_map->top){
            Map_unit* new_map = new Map_unit();
            new_map->recs = generateMap();
            new_map->top = nullptr;
            new_map->left = nullptr;
            new_map->right = nullptr;
            new_map->bot = current_map;        
            current_map->top = new_map;        
            current_map = new_map;        
        }else{
            current_map = current_map->top; 
        }
        player.y = height + 1;
    }else if(player.x > width){
        if(!current_map->right){
            Map_unit* new_map = new Map_unit();
            new_map->recs = generateMap();
            new_map->top = nullptr;
            new_map->left = current_map;
            new_map->right = nullptr;
            new_map->bot = nullptr;        
            current_map->right = new_map;        
            current_map = new_map;        
        }else{
            current_map = current_map->right; 
        }
        player.x = 1;
    }else if(player.x < 0){
        if(!current_map->left){
            Map_unit* new_map = new Map_unit();
            new_map->recs = generateMap();
            new_map->top = nullptr;
            new_map->left = nullptr;
            new_map->right = current_map;
            new_map->bot = nullptr;        
            current_map->left = new_map;        
            current_map = new_map;        
        }else{
            current_map = current_map->left; 
        }
        player.x = width + 1;
    }
    status = FREE;
}

float update_player(Rectangle& player, float rotation){
    if(IsKeyDown(KEY_A)) {
        rotation = 0.0;
        player.x -= speed;
    }
    if(IsKeyDown(KEY_D)) {
        rotation = 180.0;
        player.x += speed;
    }
    if(IsKeyDown(KEY_W)) {
        rotation = 90.0;
        player.y -= speed;
    }
    if(IsKeyDown(KEY_S)){
        rotation = -90.0;
        player.y += speed;
    }

    if(player.y > height || player.y < 0 || player.x > width || player.x < 0){
        status = NEWMAP;
    }

    return rotation;
}

void roaming(Player_info& player_info){
    player_info.rotation = update_player(player_info.player, player_info.rotation);

    DrawText(std::to_string(player_info.player.x).c_str(), 50, 50, 50, MAROON);
    DrawText(std::to_string(player_info.player.y).c_str(), 50, 100, 50, MAROON);
    DrawTexturePro(player_info.texture,(Rectangle){0,0,(float) player_info.texture.width, (float) player_info.texture.height} ,
            (Rectangle){player_info.player.x,player_info.player.y, (float)player_info.texture.width, (float)player_info.texture.height}, 
            player_info.origin, player_info.rotation, WHITE);
    for(const auto& rec: current_map->recs){
        if(CheckCollisionRecs(player_info.player, rec)){
            status = FIGHT;
            return;
        }
    }
}


void run_loop(){
    InitWindow(0, 0, "Space Freight");

    int screenHeight = GetMonitorHeight(0);
    int screenWidth = GetMonitorWidth(0);

    width = screenWidth;
    height = screenHeight;

    Image img = LoadImage("test.png"); 
    Texture2D texture = LoadTextureFromImage(img);
    struct Player_info player = {
        Rectangle { (float) 300, (float) 300, 25 ,25},
        texture,
        Vector2 { (float)texture.width/2, (float)texture.height/2 },
        16.0f
    };


    std::vector<Rectangle> recs = generateMap();

    Map_unit new_map = {
        recs,nullptr,nullptr,nullptr,nullptr
    };

    current_map = &new_map;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        if(status == FREE){
            roaming(player);
            drawMap();
        }else if(status == NEWMAP){
            switchMap(player.player);    
        }else if(status == FIGHT){
            drawFight();
        }
        EndDrawing();
    }
}

int main(){
    run_loop();
}

