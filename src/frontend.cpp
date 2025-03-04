// #include "common.h"
// #include <vector>
// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>

// namespace Frontend {
// int TILE_SIZE = 100;

// class Grid {
// public:
//     Grid() {
//         tiles = new tile_t[0]();
//     }

//     ~Grid() {
//         delete[] tiles;
//     }

//     void resize(int new_width, int new_height, int offset_x = 0, int offset_y = 0) {
//         tile_t* new_tiles = new tile_t[new_width * new_height]();
//         std::fill(new_tiles, new_tiles + new_width * new_height, Rules::EMPTY_TILE);

//         for (int y = 0; y < std::min(height, new_height); ++y) {
//             for (int x = 0; x < std::min(width, new_width); ++x) {
//                 new_tiles[(x + offset_x) + (y + offset_y) * new_width] = tiles[x + y * width];
//             }
//         }
//         delete[] tiles;
//         width = new_width;
//         height = new_height;
//         tiles = new_tiles;
//     }

//     void update_tile(int x, int y, tile_t value) {
//         tiles[x + y * width] = value;
//     }

//     tile_t get_tile(int x, int y) {
//         return tiles[x + y * width];
//     }

//     int width;
//     int height;

// private:    
//     tile_t* tiles;
// };

// Grid grid = Grid();

// void draw_grid(sf::RenderWindow& window) {
//     static std::unordered_map<tile_t, sf::Color> colorMap;

//     // Scale down tile size if needed to keep it in view
//     while (grid.width * TILE_SIZE > window.getSize().x || grid.height * TILE_SIZE > window.getSize().y) {
//         TILE_SIZE--;
//     }

//     int offsetX = std::max((uint)0, (window.getSize().x - grid.width * TILE_SIZE) / 2);
//     int offsetY = std::max((uint)0, (window.getSize().y - grid.height * TILE_SIZE) / 2);

//     for (int y = grid.height - 1; y >= 0; --y) {
//         for (int x = 0; x < grid.width; ++x) {
//             tile_t tile = grid.get_tile(x, y);

//             if (tile == Rules::EMPTY_TILE) {
//                 continue;
//             }

//             // Ensure each tile type has a consistent color
//             if (colorMap.find(tile) == colorMap.end()) {
//                 colorMap[tile] = sf::Color(rand() % 256, rand() % 256, rand() % 256);
//             }

//             sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
//             tileShape.setPosition(sf::Vector2f(offsetX + x * TILE_SIZE, offsetY + (grid.height - 1 - y) * TILE_SIZE));
//             tileShape.setFillColor(colorMap[tile]);

//             window.draw(tileShape);
//         }
//     }

//     sf::RectangleShape gridShape(sf::Vector2f(grid.width * TILE_SIZE, grid.height * TILE_SIZE));
//     gridShape.setPosition(sf::Vector2f(offsetX, offsetY));
//     gridShape.setFillColor(sf::Color::Transparent);
//     gridShape.setOutlineThickness(2.0f);
//     gridShape.setOutlineColor(sf::Color::White);
//     window.draw(gridShape);
// }

// void handle_tile_update(std::optional<Message> message) {
//     grid.update_tile(message->location.x, message->location.y, message->value);
// }

// void handle_custom_message(std::optional<Message> message) {
//     std::string content = message->content;
//     std::vector<std::string> tokens;
//     std::stringstream ss(content);
//     std::string token;

//     while (std::getline(ss, token, ',')) {
//         tokens.push_back(token);
//     }
 
//     if (tokens[0] == "GRID_SIZE") {
//         int new_width = std::stoi(tokens[1]);
//         int new_height = std::stoi(tokens[2]);
//         int resize_offset_x = std::stoi(tokens[3]);
//         int resize_offset_y = std::stoi(tokens[4]);

//         grid.resize(new_width, new_height, resize_offset_x, resize_offset_y);

//         std::cout << "[FRONTEND] Set grid size to " << new_width << "x" << new_height << std::endl;
//     } else {
//         std::cerr << "[FRONTEND] Failed to handle message: " << content << std::endl;
//     }
// }

// void run() {

//     sf::ContextSettings settings;
//     settings.depthBits = 24;
//     settings.stencilBits = 8;
//     settings.majorVersion = 3;
//     settings.minorVersion = 0;

//     sf::RenderWindow window;
//     window.create(sf::VideoMode::getDesktopMode(), "Tile Automata Simulator", sf::Style::Resize, sf::State::Windowed, settings);

//     window.setFramerateLimit(60);

//     const sf::Font font("arial.ttf");
//     sf::Text text(font, "AutoTile GPU\nRight Arrow Key -> Step simulation\nR -> Run simulation\nSpace -> Pause simulation\nP -> Print grid\nEsc -> Exit", 15);

//     // Set up event handlers
//     const auto onClose = [&window](const sf::Event::Closed&) {
//         frontend_message_queue.push(Message{Message::MessageType::EXIT});
//         window.close();
//     };

//     const auto onResize = [&window](const sf::Event::Resized& resized) {
//         sf::View view;
//         view.setSize(sf::Vector2f(static_cast<float>(resized.size.x), static_cast<float>(resized.size.y)));
//         view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
//         window.setView(view);
//     };

//     const auto onKeyPress = [&window](const sf::Event::KeyPressed& keyPressed) {
//         // Escape - Exit
//         if (keyPressed.code == sf::Keyboard::Key::Escape) {
//             frontend_message_queue.push(Message{Message::MessageType::EXIT});
//             window.close();
//         }

//         // Space - toggle pause
//         if (keyPressed.code == sf::Keyboard::Key::Space) {
//             frontend_message_queue.push(Message{Message::MessageType::PAUSE});
//         }

//         // Right arrow - step
//         if (keyPressed.code == sf::Keyboard::Key::Right) {
//             frontend_message_queue.push(Message{Message::MessageType::STEP});
//         }

//         // R - run
//         if (keyPressed.code == sf::Keyboard::Key::R) {
//             frontend_message_queue.push(Message{Message::MessageType::RUN});
//         }

//         // P - print grid
//         if (keyPressed.code == sf::Keyboard::Key::P) {
//             for (int y = grid.height - 1; y >= 0; --y) {
//                 for (int x = 0; x < grid.width; ++x) {
//                     std::cout << Tile::decode(grid.get_tile(x, y)) << " ";
//                 }
//                 std::cout << std::endl;
//             }
//             std::cout << std::endl;
//             for (int y = grid.height - 1; y >= 0; --y) {
//                 for (int x = 0; x < grid.width; ++x) {
//                     std::cout << std::setw(13) << Tile::name(grid.get_tile(x, y)) << " ";
//                 }
//                 std::cout << std::endl;
//             }
//         }        
//     };

//     while (window.isOpen()) {
//         // Handle all events with the visitor pattern
//         window.handleEvents(onClose, onResize, onKeyPress);

//         // Rest of your code remains the same
//         while (auto msg = simulator_message_queue.try_pop()) {
//             switch (msg->type) {
//                 case Message::MessageType::TILE_UPDATE:
//                     handle_tile_update(msg);
//                     break;
//                 case Message::MessageType::EXIT:
//                     window.close();
//                     break;
//                 case Message::MessageType::CUSTOM:
//                     handle_custom_message(msg);
//                     break;
//             }
//         }

//         window.clear();
//         draw_grid(window);
//         window.draw(text);
//         window.display();
//     }

// }

// } // namespace Frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// #include "common.h"
// #include <vector>
// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>

// namespace Frontend {
// // Initial tile size remains the same
// int TILE_SIZE = 100;

// class Grid {
// public:
//     Grid() : 
//         tiles(new tile_t[0]()), 
//         zoom_level(1.0f), 
//         pan_offset_x(0.0f), 
//         pan_offset_y(0.0f) 
//     {}

//     ~Grid() {
//         delete[] tiles;
//     }

//     void resize(int new_width, int new_height, int offset_x = 0, int offset_y = 0) {
//         tile_t* new_tiles = new tile_t[new_width * new_height]();
//         std::fill(new_tiles, new_tiles + new_width * new_height, Rules::EMPTY_TILE);

//         for (int y = 0; y < std::min(height, new_height); ++y) {
//             for (int x = 0; x < std::min(width, new_width); ++x) {
//                 new_tiles[(x + offset_x) + (y + offset_y) * new_width] = tiles[x + y * width];
//             }
//         }
//         delete[] tiles;
//         width = new_width;
//         height = new_height;
//         tiles = new_tiles;
//     }

//     void update_tile(int x, int y, tile_t value) {
//         tiles[x + y * width] = value;
//     }

//     tile_t get_tile(int x, int y) {
//         return tiles[x + y * width];
//     }

//     // New methods for zoom and pan
//     void zoom(float delta, const sf::Vector2f& mouse_pos, const sf::Vector2u& window_size) {
//         // Limit zoom between 0.1 and 5
//         zoom_level = std::max(0.1f, std::min(5.0f, zoom_level * delta));
//     }

//     void pan(float dx, float dy) {
//         pan_offset_x += dx;
//         pan_offset_y += dy;
//     }

//     // Zoom and pan related member variables
//     float zoom_level;
//     float pan_offset_x;
//     float pan_offset_y;

//     int width;
//     int height;

// private:    
//     tile_t* tiles;
// };

// Grid grid = Grid();

// void draw_grid(sf::RenderWindow& window) {
//     static std::unordered_map<tile_t, sf::Color> colorMap;

//     // Calculate effective tile size based on zoom
//     int effective_tile_size = TILE_SIZE * grid.zoom_level;

//     // Adjust grid drawing to account for pan and zoom
//     int offsetX = std::max((uint)0, (window.getSize().x - grid.width * effective_tile_size) / 2) + grid.pan_offset_x;
//     int offsetY = std::max((uint)0, (window.getSize().y - grid.height * effective_tile_size) / 2) + grid.pan_offset_y;

//     for (int y = grid.height - 1; y >= 0; --y) {
//         for (int x = 0; x < grid.width; ++x) {
//             tile_t tile = grid.get_tile(x, y);

//             if (tile == Rules::EMPTY_TILE) {
//                 continue;
//             }

//             // Ensure each tile type has a consistent color
//             if (colorMap.find(tile) == colorMap.end()) {
//                 colorMap[tile] = sf::Color(rand() % 256, rand() % 256, rand() % 256);
//             }

//             sf::RectangleShape tileShape(sf::Vector2f(effective_tile_size, effective_tile_size));
//             tileShape.setPosition(sf::Vector2f(
//                 offsetX + x * effective_tile_size, 
//                 offsetY + (grid.height - 1 - y) * effective_tile_size
//             ));
//             tileShape.setFillColor(colorMap[tile]);

//             window.draw(tileShape);
//         }
//     }

//     // Draw grid outline with zoom consideration
//     sf::RectangleShape gridShape(sf::Vector2f(grid.width * effective_tile_size, grid.height * effective_tile_size));
//     gridShape.setPosition(sf::Vector2f(offsetX, offsetY));
//     gridShape.setFillColor(sf::Color::Transparent);
//     gridShape.setOutlineThickness(2.0f);
//     gridShape.setOutlineColor(sf::Color::White);
//     window.draw(gridShape);
// }



// void run() {
//     sf::ContextSettings settings;
//     settings.depthBits = 24;
//     settings.stencilBits = 8;
//     settings.majorVersion = 3;
//     settings.minorVersion = 0;

//     sf::RenderWindow window;
//     window.create(sf::VideoMode::getDesktopMode(), "Tile Automata Simulator", sf::Style::Resize, sf::State::Windowed, settings);

//     window.setFramerateLimit(60);

//     const sf::Font font("arial.ttf");
//     sf::Text text(font, "AutoTile GPU\nRight Arrow Key -> Step simulation\nR -> Run simulation\nSpace -> Pause simulation\nP -> Print grid\nEsc -> Exit\nScroll -> Zoom\nArrow Keys -> Pan", 15);

//     // Existing event handlers with modifications
//     const auto onClose = [&window](const sf::Event::Closed&) {
//         frontend_message_queue.push(Message{Message::MessageType::EXIT});
//         window.close();
//     };

//     const auto onResize = [&window](const sf::Event::Resized& resized) {
//         sf::View view;
//         view.setSize(sf::Vector2f(static_cast<float>(resized.size.x), static_cast<float>(resized.size.y)));
//         view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
//         window.setView(view);
//     };

//     const auto onKeyPress = [&window](const sf::Event::KeyPressed& keyPressed) {
//         // Existing key handlers
//         if (keyPressed.code == sf::Keyboard::Key::Escape) {
//             frontend_message_queue.push(Message{Message::MessageType::EXIT});
//             window.close();
//         }

//         // Panning with arrow keys
//         float pan_speed = 20.0f;
//         if (keyPressed.code == sf::Keyboard::Key::A) {
//             grid.pan(pan_speed, 0);
//         }
//         if (keyPressed.code == sf::Keyboard::Key::D) {
//             grid.pan(-pan_speed, 0);
//         }
//         if (keyPressed.code == sf::Keyboard::Key::W) {
//             grid.pan(0, pan_speed);
//         }
//         if (keyPressed.code == sf::Keyboard::Key::S) {
//             grid.pan(0, -pan_speed);
//         }


//         if (keyPressed.code == sf::Keyboard::Key::Space) {
//             frontend_message_queue.push(Message{Message::MessageType::PAUSE});
//         }
//         if (keyPressed.code == sf::Keyboard::Key::Right) {
//             frontend_message_queue.push(Message{Message::MessageType::STEP});
//         }
//         if (keyPressed.code == sf::Keyboard::Key::R) {
//             frontend_message_queue.push(Message{Message::MessageType::RUN});
//         }
//         if (keyPressed.code == sf::Keyboard::Key::P) {
//             for (int y = grid.height - 1; y >= 0; --y) {
//                 for (int x = 0; x < grid.width; ++x) {
//                     std::cout << Tile::decode(grid.get_tile(x, y)) << " ";
//                 }
//                 std::cout << std::endl;
//             }
//             std::cout << std::endl;
//             for (int y = grid.height - 1; y >= 0; --y) {
//                 for (int x = 0; x < grid.width; ++x) {
//                     std::cout << std::setw(13) << Tile::name(grid.get_tile(x, y)) << " ";
//                 }
//                 std::cout << std::endl;
//             }
//         }        
//     };

//     // New handler for mouse wheel zoom
//     const auto onMouseWheel = [&window](const sf::Event::MouseWheelScrolled& mouseScroll) {
//         // Zoom in/out based on mouse wheel
//         float zoom_factor = (mouseScroll.delta > 0) ? 1.1f : 0.9f;
//         grid.zoom(zoom_factor, 
//             sf::Vector2f(mouseScroll.position.x, mouseScroll.position.y), 
//             window.getSize()
//         );
//     };

//     while (window.isOpen()) {
//         // Handle all events with the visitor pattern, add mouse wheel handler
//         window.handleEvents(onClose, onResize, onKeyPress, onMouseWheel);

//         // Existing message handling
//         while (auto msg = simulator_message_queue.try_pop()) {
//             switch (msg->type) {
//                 case Message::MessageType::TILE_UPDATE:
//                     handle_tile_update(msg);
//                     break;
//                 case Message::MessageType::EXIT:
//                     window.close();
//                     break;
//                 case Message::MessageType::CUSTOM:
//                     handle_custom_message(msg);
//                     break;
//             }
//         }

//         window.clear();
//         draw_grid(window);
//         window.draw(text);
//         window.display();
//     }
// }

// } // namespace Frontend



#include "common.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>

namespace Frontend {
int TILE_SIZE = 100;

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

    void resize(int new_width, int new_height, int offset_x = 0, int offset_y = 0) {
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

    void update_tile(int x, int y, tile_t value) {
        tiles[x + y * width] = value;
    }

    tile_t get_tile(int x, int y) {
        return tiles[x + y * width];
    }

    void zoom(float delta, sf::RenderWindow& window) {
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

    void pan(float dx, float dy) {
        // Directly adjust pan targets
        target_pan_x += dx;
        target_pan_y += dy;
    }

    void update() {
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

Grid grid = Grid();

void draw_grid(sf::RenderWindow& window) {
    static std::unordered_map<tile_t, sf::Color> colorMap;

    int effective_tile_size = TILE_SIZE * grid.current_zoom_level;

    // Simplified offset calculation
    int offsetX = grid.current_pan_x;
    int offsetY = grid.current_pan_y;

    for (int y = grid.height - 1; y >= 0; --y) {
        for (int x = 0; x < grid.width; ++x) {
            tile_t tile = grid.get_tile(x, y);

            if (tile == Rules::EMPTY_TILE) {
                continue;
            }

            // Ensure each tile type has a consistent color
            if (colorMap.find(tile) == colorMap.end()) {
                colorMap[tile] = sf::Color(rand() % 256, rand() % 256, rand() % 256);
            }

            sf::RectangleShape tileShape(sf::Vector2f(effective_tile_size, effective_tile_size));
            tileShape.setPosition(sf::Vector2f(
                offsetX + x * effective_tile_size, 
                offsetY + (grid.height - 1 - y) * effective_tile_size
            ));
            tileShape.setFillColor(colorMap[tile]);

            window.draw(tileShape);
        }
    }

    // Draw grid outline with smooth zoom consideration
    sf::RectangleShape gridShape(sf::Vector2f(grid.width * effective_tile_size, grid.height * effective_tile_size));
    gridShape.setPosition(sf::Vector2f(offsetX, offsetY));
    gridShape.setFillColor(sf::Color::Transparent);
    gridShape.setOutlineThickness(2.0f);
    gridShape.setOutlineColor(sf::Color::White);
    window.draw(gridShape);
}

void handle_tile_update(std::optional<Message> message) {
    grid.update_tile(message->location.x, message->location.y, message->value);
}

void handle_custom_message(std::optional<Message> message) {
    std::string content = message->content;
    std::vector<std::string> tokens;
    std::stringstream ss(content);
    std::string token;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
 
    if (tokens[0] == "GRID_SIZE") {
        int new_width = std::stoi(tokens[1]);
        int new_height = std::stoi(tokens[2]);
        int resize_offset_x = std::stoi(tokens[3]);
        int resize_offset_y = std::stoi(tokens[4]);

        grid.resize(new_width, new_height, resize_offset_x, resize_offset_y);

        std::cout << "[FRONTEND] Set grid size to " << new_width << "x" << new_height << std::endl;
    } else {
        std::cerr << "[FRONTEND] Failed to handle message: " << content << std::endl;
    }
}

void run() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    sf::RenderWindow window;
    window.create(sf::VideoMode::getDesktopMode(), "Tile Automata Simulator", sf::Style::Resize, sf::State::Windowed, settings);

    window.setFramerateLimit(60);

    const sf::Font font("arial.ttf");
    sf::Text text(font, "AutoTile GPU\nWASD/Arrow Keys -> Pan\nScroll -> Zoom\nRight Arrow -> Step simulation\nR -> Run simulation\nSpace -> Pause simulation\nP -> Print grid\nEsc -> Exit", 15);

    // Pan speed for WASD and arrow keys
    const float PAN_SPEED = 100.0f;

    const auto onClose = [&window](const sf::Event::Closed&) {
        frontend_message_queue.push(Message{Message::MessageType::EXIT});
        window.close();
    };

    const auto onResize = [&window](const sf::Event::Resized& resized) {
        // Update the view size and center based on window size
        sf::View view;
        view.setSize(sf::Vector2f(static_cast<float>(resized.size.x), static_cast<float>(resized.size.y)));
        view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
        window.setView(view);
    };

    const auto onKeyPress = [&window, PAN_SPEED](const sf::Event::KeyPressed& keyPressed) {
        // Panning with WASD and arrow keys
        if (keyPressed.code == sf::Keyboard::Key::A) {
            grid.pan(PAN_SPEED, 0);
        }
        if (keyPressed.code == sf::Keyboard::Key::D) {
            grid.pan(-PAN_SPEED, 0);
        }
        if (keyPressed.code == sf::Keyboard::Key::W) {
            grid.pan(0, PAN_SPEED);
        }
        if (keyPressed.code == sf::Keyboard::Key::S) {
            grid.pan(0, -PAN_SPEED);
        }

        // Existing simulation control key handlers
        if (keyPressed.code == sf::Keyboard::Key::Escape) {
            frontend_message_queue.push(Message{Message::MessageType::EXIT});
            window.close();
        }
        if (keyPressed.code == sf::Keyboard::Key::Space) {
            frontend_message_queue.push(Message{Message::MessageType::PAUSE});
        }
        if (keyPressed.code == sf::Keyboard::Key::Right) {
            frontend_message_queue.push(Message{Message::MessageType::STEP});
        }
        if (keyPressed.code == sf::Keyboard::Key::R) {
            frontend_message_queue.push(Message{Message::MessageType::RUN});
        }
        if (keyPressed.code == sf::Keyboard::Key::P) {
            for (int y = grid.height - 1; y >= 0; --y) {
                for (int x = 0; x < grid.width; ++x) {
                    std::cout << Tile::decode(grid.get_tile(x, y)) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for (int y = grid.height - 1; y >= 0; --y) {
                for (int x = 0; x < grid.width; ++x) {
                    std::cout << std::setw(13) << Tile::name(grid.get_tile(x, y)) << " ";
                }
                std::cout << std::endl;
            }
        }        
    };

    // New handler for mouse wheel zoom
    const auto onMouseWheel = [&window](const sf::Event::MouseWheelScrolled& mouseScroll) {
        // Zoom in/out based on mouse wheel
        float zoom_factor = (mouseScroll.delta > 0) ? 1.1f : 0.9f;
        grid.zoom(zoom_factor, window);
    };

    // New handler for mouse drag
    const auto onMouseDrag = [&window](const sf::Event::MouseMoved& mouseMoved) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (!grid.is_dragging) {
                grid.last_mouse_x = mouseMoved.position.x;
                grid.last_mouse_y = mouseMoved.position.y;
                grid.is_dragging = true;
            }
            float dx = mouseMoved.position.x - grid.last_mouse_x;
            float dy = mouseMoved.position.y - grid.last_mouse_y;
            grid.pan(dx, dy);
            grid.last_mouse_x = mouseMoved.position.x;
            grid.last_mouse_y = mouseMoved.position.y;
        } else {
            grid.is_dragging = false;
        }
    };

    while (window.isOpen()) {
        // Handle all events
        window.handleEvents(onClose, onResize, onKeyPress, onMouseWheel, onMouseDrag);

        // Update smooth zoom and pan
        grid.update();

        // Existing message handling
        while (auto msg = simulator_message_queue.try_pop()) {
            switch (msg->type) {
                case Message::MessageType::TILE_UPDATE:
                    handle_tile_update(msg);
                    break;
                case Message::MessageType::EXIT:
                    window.close();
                    break;
                case Message::MessageType::CUSTOM:
                    handle_custom_message(msg);
                    break;
            }
        }

        window.clear();
        draw_grid(window);
        window.draw(text);
        window.display();
    }
}
} // namespace Frontend
