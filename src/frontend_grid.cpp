#include "frontend_grid.h"
#include <vector>

namespace Frontend {
    int TILE_SIZE = 100;

    void DisplayGrid::resize(int new_width, int new_height, int offset_x = 0, int offset_y = 0) {
        tile_t* new_tiles = new tile_t[new_width * new_height]();
        std::fill(new_tiles, new_tiles + new_width * new_height, Rules::EMPTY_TILE);

        for (int y = 0; y < std::min(height, new_height); ++y) {
            for (int x = 0; x < std::min(width, new_width); ++x) {
                new_tiles[(x + offset_x) + (y + offset_y) * new_width] = tiles[x + y * width];
            }
        }
        delete[] tiles;
        width = new_width;
        height = new_height;
        tiles = new_tiles;

        // Snap the grid position to keep the same grid cell in view
        target_pan_x = current_pan_x - offset_x * TILE_SIZE * current_zoom_level;
        target_pan_y = current_pan_y - offset_y * TILE_SIZE * current_zoom_level;
        current_pan_x = target_pan_x;
        current_pan_y = target_pan_y;
    }

    void DisplayGrid::update_tile(int x, int y, tile_t value) {
        tiles[x + y * width] = value;
    }

    tile_t DisplayGrid::get_tile(int x, int y) {
        return tiles[x + y * width];
    }

    void DisplayGrid::zoom(float delta, sf::RenderWindow& window) {
        // Store the previous zoom level
        float old_zoom = target_zoom_level;
        
        // Update target zoom level with constraints
        target_zoom_level = std::max(0.1f, std::min(10.0f, target_zoom_level * delta));
        
        // Get the current mouse position
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        
        // Calculate the mouse position relative to the grid before zoom
        float pre_zoom_mouse_x = (mouse_pos.x - current_pan_x) / (TILE_SIZE * old_zoom);
        float pre_zoom_mouse_y = (mouse_pos.y - current_pan_y) / (TILE_SIZE * old_zoom);
        
        // Calculate new pan to keep the mouse point fixed during zoom
        target_pan_x = mouse_pos.x - (pre_zoom_mouse_x * TILE_SIZE * target_zoom_level);
        target_pan_y = mouse_pos.y - (pre_zoom_mouse_y * TILE_SIZE * target_zoom_level);
        current_pan_x = target_pan_x;
        current_pan_y = target_pan_y;
    }

    void DisplayGrid::pan(float dx, float dy) {
        target_pan_x += dx;
        target_pan_y += dy;
        current_pan_x = target_pan_x;
        current_pan_y = target_pan_y;
    }

    void DisplayGrid::smooth_pan(float dx, float dy) {
        target_pan_x += dx;
        target_pan_y += dy;
    }

    void DisplayGrid::update() {
        // Smooth zoom interpolation
        float zoom_diff = target_zoom_level - current_zoom_level;
        if (std::abs(zoom_diff) > 0.00001f) {
            current_zoom_level += zoom_diff * zoom_lerp_speed * 2.0f;
        } else {
            current_zoom_level = target_zoom_level;
        }
        
        // Smooth pan interpolation
        float pan_x_diff = target_pan_x - current_pan_x;
        float pan_y_diff = target_pan_y - current_pan_y;
        
        // Use a more aggressive pan interpolation during zoom
        float effective_pan_lerp = (std::abs(zoom_diff) > 0.001f) 
            ? pan_lerp_speed * 2.0f 
            : pan_lerp_speed;
        
        // Apply pan interpolation
        if (std::abs(pan_x_diff) > 0.1f) {
            current_pan_x += pan_x_diff * effective_pan_lerp;
        } else {
            current_pan_x = target_pan_x;
        }
        
        if (std::abs(pan_y_diff) > 0.1f) {
            current_pan_y += pan_y_diff * effective_pan_lerp;
        } else {
            current_pan_y = target_pan_y;
        }
    }

    void DisplayGrid::draw(sf::RenderWindow& window) {
        static std::unordered_map<tile_t, sf::Color> colorMap;

        int tile_view_size = TILE_SIZE * current_zoom_level;

        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                tile_t tile = get_tile(x, y);

                if (tile == Rules::EMPTY_TILE) {
                    continue;
                }

                // Ensure each tile type has a consistent color
                if (colorMap.find(tile) == colorMap.end()) {
                    colorMap[tile] = sf::Color(rand() % 256, rand() % 256, rand() % 256);
                }

                sf::RectangleShape tileShape(sf::Vector2f(tile_view_size, tile_view_size));
                tileShape.setPosition(sf::Vector2f(
                    current_pan_x + x * tile_view_size, 
                    current_pan_y + (height - 1 - y) * tile_view_size
                ));
                tileShape.setFillColor(colorMap[tile]);

                window.draw(tileShape);
            }
        }
    }
}