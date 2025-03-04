#pragma once

#include "common.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Frontend {
    extern int TILE_SIZE;

    class Grid {
public:
    Grid() : 
        tiles(new tile_t[0]()), 
        current_zoom_level(1.0f), 
        target_zoom_level(1.0f),
        current_pan_x(0.0f), 
        current_pan_y(0.0f),
        target_pan_x(0.0f),
        target_pan_y(0.0f),
        zoom_lerp_speed(0.5f),
        pan_lerp_speed(0.35f),
        last_mouse_x(0), 
        last_mouse_y(0),
        is_dragging(false)
    {}

    ~Grid() {
        delete[] tiles;
    }

    void resize(int new_width, int new_height, int offset_x, int offset_y);

    void update_tile(int x, int y, tile_t value);

    tile_t get_tile(int x, int y);

    void zoom(float delta, sf::RenderWindow& window);

    void pan(float dx, float dy);

    void update();

    void draw(sf::RenderWindow& window);

    // Zoom and pan related member variables
    float current_zoom_level;
    float target_zoom_level;
    float current_pan_x;
    float current_pan_y;
    float target_pan_x;
    float target_pan_y;
    float zoom_lerp_speed;
    float pan_lerp_speed;

    int width;
    int height;

    // Variables for mouse dragging
    bool is_dragging;
    int last_mouse_x;
    int last_mouse_y;

private:    
    tile_t* tiles;
};
}