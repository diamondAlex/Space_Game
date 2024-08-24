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

int cursor_pos = 560;
int skill_index = 0;

std::vector<std::string> skills = {
    "lazer",
    "rail gun",
    "rockets"
};

int animating = 0;
void animation(){
    std::string name = skills[skill_index];
    if(name == "lazer"){
        DrawRectanglePro({310,460,600,(float)animating/2}, {0,0}, -27.0, BLACK);
    }
    else if(name == "rail gun"){
        DrawRectanglePro({(float)(310+(50-animating)*10),(float)460-(50-animating)*6,50,10}, {0,0}, -27.0, BLACK);
    }
    else if(name == "rockets"){
        DrawRectanglePro({(float)(310+(50-animating)*10),(float)460-(50-animating)*6,50,10}, {0,0}, -27.0, BLACK);
        DrawRectanglePro({(float)(310+(50-animating)*10),(float)450-(50-animating)*6,10,30}, {0,0}, -27.0, BLACK);
    }
    animating--;
    
}

void drawFight(){
    Image img = LoadImage("enemy.png"); 
    Texture2D texture = LoadTextureFromImage(img);
    img = LoadImage("back.png"); 
    Texture2D texture2 = LoadTextureFromImage(img);

    DrawTexture(texture, 850,75, WHITE);
    DrawTexture(texture2, 120,370, WHITE);

    if(IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)){
        if(skill_index >= 2){
            skill_index = 0;
        }
        else{
            skill_index++;
        }
    }
    if(IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)){
        if(skill_index < 1){
            skill_index = 2;
        }
        else{
            skill_index--;
        }
    }
    if(IsKeyReleased(KEY_ENTER)){
        animating = 50;
    }
    DrawRectangle(875, cursor_pos + skill_index * 50, 20,5, MAROON);
    DrawText(skills[0].c_str(), 950, 550, 30, MAROON);
    DrawText(skills[1].c_str(), 950, 600, 30, MAROON);
    DrawText(skills[2].c_str(), 950, 650, 30, MAROON);
    DrawText("esc to quit", 300, 300, 30, MAROON);
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

    DrawText("esc to quit", 50, 50, 50, MAROON);
    DrawText("wasd to control", 50, 100, 50, MAROON);
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
            if(animating > 1){
                animation();
            }
        }
        EndDrawing();
    }
}

int main(){
    run_loop();
}

