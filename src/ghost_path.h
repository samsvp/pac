#ifndef GHOST_PATH_H
#define GHOST_PATH_H

#include <SDL.h>

#include "pac.h"
#include "util_types.h"



// blinky, inky, pinky and clyde
enum ghosts { RED, BLUE, PINK, CLYDE };
typedef enum ghosts ghosts;


/**
 * @brief Calculates Blinky (the red ghost) target
 * Blinkys target is Pacmans current tile or the top
 * right of the screen
 * 
 * @param p the struct containing the cpu data
 * @return vec2 
 */
vec2 red_target(pac* p) 
{
    uint8_t pacman_x = p->cpu.read_byte(p, 0x4D39);
    uint8_t pacman_y = p->cpu.read_byte(p, 0x4D3A);

    vec2 t = { .x = pacman_x, .y = pacman_y };
    return t;
}


/**
 * @brief Calculates Pinky (the pink ghost) target
 * Pinkys target is 4 tiles ahead of Pacmans current 
 * tile or the top left of the screen
 * 
 * @param p the struct containing the cpu data
 * @return vec2 
 */
vec2 pink_target(pac* p)
{
    uint8_t pacman_x = p->cpu.read_byte(p, 0x4D39);
    uint8_t pacman_y = p->cpu.read_byte(p, 0x4D3A);
    uint8_t pacman_dir = p->cpu.read_byte(p, 0x4D3C);
    
    vec2 t = { .x = pacman_x, .y = pacman_y };
    if (pacman_dir == 0) t.y -= 4;
    else if (pacman_dir == 1) t.x += 4;
    else if (pacman_dir == 2) t.y += 4;
    else if (pacman_dir == 3) { t.x -= 4; t.y += 4; }

    return t;
}


/**
 * @brief Calculates Inky (the blue ghost) target
 * Inkys target is 2 tiles ahead of pacmans current 
 * tile added to the difference between Pacmans and
 * Blinkys(the red ghost) position(after applying the offset)
 * or the bottom right of the screen
 * 
 * @param p the struct containing the cpu data
 * @return vec2 
 */
vec2 blue_target(pac* p)
{
    uint8_t blinky_x = p->cpu.read_byte(p, 0x4D0A);
    uint8_t blinky_y = p->cpu.read_byte(p, 0x4D0B);

    uint8_t pacman_x = p->cpu.read_byte(p, 0x4D39);
    uint8_t pacman_y = p->cpu.read_byte(p, 0x4D3A);
    uint8_t pacman_dir = p->cpu.read_byte(p, 0x4D3C);

    vec2 t = { .x = pacman_x, .y = pacman_y };
    // offset pacmans location
    if (pacman_dir == 0) t.y -= 2;
    else if (pacman_dir == 1) t.x += 2;
    else if (pacman_dir == 2) t.y += 2;
    else if (pacman_dir == 3) { t.x -= 2; t.y += 2; }

    t.x += (t.x - blinky_x);
    t.y += (t.y - blinky_y);

    return t;
}


/**
 * @brief Calculates Clyde (the orange ghost) target
 * Clydes target is Pacman if the squared distance between 
 * Clyde and Pacman is more than 0x40, else it is the bottom
 * left corner of the screen
 * 
 * @param p the struct containing the cpu data
 * @return vec2 
 */
vec2 clyde_target(pac* p)
{
    uint8_t clyde_x = p->cpu.read_byte(p, 0x4D10);
    uint8_t clyde_y = p->cpu.read_byte(p, 0x4D11);
    uint8_t pacman_x = p->cpu.read_byte(p, 0x4D39);
    uint8_t pacman_y = p->cpu.read_byte(p, 0x4D3A);

    vec2 pacman = { .x = pacman_x, .y = pacman_y };
    vec2 clyde = { .x = clyde_x, .y = clyde_y };

    vec2 t = {.x = 0x40, .y = 0x3B};
    if (dist2(clyde, pacman) >= 0x40) t = pacman;

    return t;
}


/**
 * @brief Converts the position tile as stored in the
 * cpu into screen coordinates
 * 
 * @param tile the cpu tile
 * @return vec2 
 */
vec2 tile_to_screen(vec2 tile) 
{
    int y = 8 * (tile.x - 30) + 4;
    int x = 8 * (tile.y - 32) + 4;

    vec2 s = { 
        .x = 224 - x,
        .y = y
    };
    return s;
}


/**
 * @brief Returns true if the target tile is empty
 * 
 * @param t 
 * @param p 
 * @return true 
 * @return false 
 */
bool is_tile_empty(vec2 t, pac* p)
{
    if (t.y >= 0x3C || t.y <= 0x1F) return t.x == 0x2F;
    
    uint8_t uv = p->cpu.read_byte(p, 0x4040 + 0x20 * (t.y - 0x20) + (t.x - 0x20));
    int8_t iv = (int8_t) uv;
    return iv < -0x70 || iv > 0;
}


/**
 * @brief Returns the next tile that the ghost will be and its
 * direction
 * 
 * @param current_tile the current ghost tile
 * @param d index of the ghost direction as stored in the cpu registers
 * @param target the target tile
 * @param p 
 * @return pose 
 */
pose next_dir(vec2 current_tile, int d, vec2 target, pac* p)
{
    // Right, down, left, up
    const vec2 mdirs[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    const vec2 dirs[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

    int next_d = 0;

    int min_dist = 1000000;

    // here we are ignoring the two tiles where the ghosts can't turn up
    int nd = 0;
    for (int i = 0; i < 4; i++)
    {
        vec2 next_dir = mdirs[i];

        // ghosts can't turn around
        if (is_equal(add(next_dir, dirs[d]), vec2_zero))
            continue;
        
        vec2 next_tile = add(current_tile, next_dir);
        if (!is_tile_empty(next_tile, p)) continue;

        int dist = dist2(next_tile, target);
        if (dist < min_dist)
        {
            min_dist = dist;
            nd = i + 1;
        }
    }
    next_d = 4 - nd;

    vec2 next_tile = add(current_tile, dirs[next_d]);
    if (next_tile.y == 0x3E) next_tile.y = 0x1E;
    else if (next_tile.y == 0x1D) next_tile.y = 0x3D;

    pose mpose = { .pos = next_tile, .dir = next_d };
    return mpose;
}


/**
 * @brief Draws the path that the red ghost to its target
 * 
 * @param p 
 * @param renderer 
 */
void draw_red_path(pac* p, SDL_Renderer* renderer)
{
    uint8_t blinky_x = p->cpu.read_byte(p, 0x4D0A);
    uint8_t blinky_y = p->cpu.read_byte(p, 0x4D0B);
    uint8_t blinky_dir = p->cpu.read_byte(p, 0x4D2C);

    vec2 tile = { .x=blinky_x, .y=blinky_y };
    const vec2 mdirs[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

    int current_dir = blinky_dir;
    vec2 current_tile = tile;

    for (int i = 0; i < 200; i++)
    {
        vec2 target = red_target(p);
        pose mpose = next_dir(current_tile, current_dir, target, p);
        current_tile = mpose.pos;
        current_dir = mpose.dir;

        if (is_equal(current_tile, target)) break;

        vec2 screen_tile = tile_to_screen(current_tile);
        draw_target(renderer, screen_tile, 255, 0, 0);
    }
}


void draw_target(SDL_Renderer* renderer, vec2 screen_rt, int r, int g, int b) 
{
    SDL_Rect box = {.x = screen_rt.x - 4, .y = screen_rt.y - 4, .w = 8, .h = 8};
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawRect(renderer, &box);
}


#endif