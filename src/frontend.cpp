#include "common.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Frontend {
int TILE_SIZE = 100;

class Grid {
public:
    Grid() {
        tiles = new tile_t[0]();
    }

    ~Grid() {
        delete[] tiles;
    }

    void resize(int new_width, int new_height) {
        tile_t* new_tiles = new tile_t[new_width * new_height]();
        std::fill(new_tiles, new_tiles + new_width * new_height, Rules::EMPTY_TILE);

        for (int y = 0; y < std::min(height, new_height); ++y) {
            for (int x = 0; x < std::min(width, new_width); ++x) {
                new_tiles[x + y * new_width] = tiles[x + y * width];
            }
        }
        delete[] tiles;
        width = new_width;
        height = new_height;
        tiles = new_tiles;
    }

    void update_tile(int x, int y, tile_t value) {
        tiles[x + y * width] = value;
    }

    tile_t get_tile(int x, int y) {
        return tiles[x + y * width];
    }

    int width;
    int height;

private:    
    tile_t* tiles;
};

Grid grid = Grid();

void draw_grid(sf::RenderWindow& window) {
    static std::unordered_map<tile_t, sf::Color> colorMap;

    int offsetX = (window.getSize().x - grid.width * TILE_SIZE) / 2;
    int offsetY = (window.getSize().y - grid.height * TILE_SIZE) / 2;

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

            sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tileShape.setPosition(sf::Vector2f(offsetX + x * TILE_SIZE, offsetY + (grid.height - 1 - y) * TILE_SIZE));
            tileShape.setFillColor(colorMap[tile]);

            window.draw(tileShape);
        }
    }

    sf::RectangleShape gridShape(sf::Vector2f(grid.width * TILE_SIZE, grid.height * TILE_SIZE));
    gridShape.setPosition(sf::Vector2f(offsetX, offsetY));
    gridShape.setFillColor(sf::Color::Transparent);
    gridShape.setOutlineThickness(2.0f);
    gridShape.setOutlineColor(sf::Color::White);
    window.draw(gridShape);
}

void handle_tile_update(std::optional<Message> message) {
    int x = message->location.x;
    int y = message->location.y;
    tile_t value = message->value;

    // Update the grid
    grid.update_tile(x, y, value);
}

void handle_custom_message(std::optional<Message> message) {
    std::string content = message->content;
    std::vector<std::string> tokens;
    std::stringstream ss(content);
    std::string token;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
 
    if (tokens[0] == "GRID_SIZE" || tokens[0] == "GRID_RESIZE") {
        int new_width = std::stoi(tokens[1]);
        int new_height = std::stoi(tokens[2]);

        grid.resize(new_width, new_height);

        std::cout << "[FRONTEND] Resized grid to " << new_width << "x" << new_height << std::endl;
    } else {
        std::cerr << "[FRONTEND] Unknown custom message: " << content << std::endl;
    }
}

void run() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Tile Automata Simulator", sf::Style::Resize);
    window.setFramerateLimit(60);
    

    // const sf::Font font("arial.ttf");
    // sf::Text text(font, "Hello SFML", 50);

    // Set up event handlers
    const auto onClose = [&window](const sf::Event::Closed&) {
        frontend_message_queue.push(Message{Message::MessageType::EXIT});
        window.close();
    };

    const auto onResize = [&window](const sf::Event::Resized& resized) {
        sf::View view;
        view.setSize(sf::Vector2f(static_cast<float>(resized.size.x), static_cast<float>(resized.size.y)));
        view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
        window.setView(view);
    };

    const auto onKeyPress = [&window](const sf::Event::KeyPressed& keyPressed) {
        // Escape - Exit
        if (keyPressed.code == sf::Keyboard::Key::Escape) {
            frontend_message_queue.push(Message{Message::MessageType::EXIT});
            window.close();
        }

        // Space - toggle pause
        if (keyPressed.code == sf::Keyboard::Key::Space) {
            frontend_message_queue.push(Message{Message::MessageType::PAUSE});
        }

        // Right arrow - step
        if (keyPressed.code == sf::Keyboard::Key::Right) {
            frontend_message_queue.push(Message{Message::MessageType::STEP});
        }

        // R - run
        if (keyPressed.code == sf::Keyboard::Key::R) {
            frontend_message_queue.push(Message{Message::MessageType::RUN});
        }

        // P - print grid
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

    while (window.isOpen()) {
        // Handle all events with the visitor pattern
        window.handleEvents(onClose, onResize, onKeyPress);

        // Rest of your code remains the same
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
        // window.draw(text);
        window.display();
    }

}
} // namespace Frontend