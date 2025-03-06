#include "common.h"
#include <vector>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <cmath>
#include "frontend_grid.h"
#include "frontend_events.h"
#include "frontend_gui.h"

namespace Frontend {

DisplayGrid grid = DisplayGrid();

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

        fmt::print("[FRONTEND] Set display grid size to {}x{}.\n", new_width, new_height);

    } else if (tokens[0] == "GRID_RESIZE") {
        int new_width = std::stoi(tokens[1]);
        int new_height = std::stoi(tokens[2]);
        int resize_offset_x = std::stoi(tokens[3]);
        int resize_offset_y = std::stoi(tokens[4]);

        grid.resize(new_width, new_height, resize_offset_x, resize_offset_y);

        fmt::print("[FRONTEND] Resize display grid size to {}x{}.\n", new_width, new_height);

        frontend_message_queue.push({Message::RUN});

    } else if (tokens[0] == "RESET_SIZE") {
        int width = std::stoi(tokens[1]);
        int height = std::stoi(tokens[2]);

        grid.reset(width, height);

        fmt::print("[FRONTEND] Reset display grid.\n");
    } else {
        fmt::print("[FRONTEND] Failed to handle message: {}\n", content);
    }
}

void wasd_input(sf::Window& window, DisplayGrid& grid, bool imgui_consuming_input) {
    if (imgui_consuming_input) return;

    static int PAN_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        PAN_SPEED = 100;
    } else {
        PAN_SPEED = 40;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        grid.smooth_pan(0, PAN_SPEED);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        grid.smooth_pan(PAN_SPEED, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        grid.smooth_pan(0, -PAN_SPEED);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        grid.smooth_pan(-PAN_SPEED, 0);
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

    if (!ImGui::SFML::Init(window, false)) {
        fmt::print(stderr, "Failed to initialize ImGui-SFML\n");
        return;
    }

    set_imgui_theme();

    sf::Clock clock;
    std::optional<sf::Event> event;
    bool imgui_consuming_input = false;

    while (window.isOpen()) {
        auto delta_time = clock.restart();

        // Handle simulator messages
        while (auto msg = simulator_message_queue.try_pop()) {
            switch (msg->type) {
                case Message::MessageType::TILE_UPDATE:
                    handle_tile_update(msg);
                    break;
                case Message::MessageType::RESET:
                    handle_custom_message(msg);
                    break;
                case Message::MessageType::EXIT:
                    window.close();
                    break;
                case Message::MessageType::CUSTOM:
                    handle_custom_message(msg);
                    break;
            }
        }

        // Handle window events
        while (event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            imgui_consuming_input = ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse;

            if (imgui_consuming_input) continue;
            
            if (const auto* closed = event->getIf<sf::Event::Closed>()) {
                on_close(*closed, window);
            } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                on_resize(*resized, window);
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                on_key_press(*keyPressed, window, grid);
            } else if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                on_mouse_wheel(*mouseWheel, window, grid);
            } else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                on_mouse_drag(*mouseMoved, window, grid);
            }
        }      
        
        // Handle input
        wasd_input(window, grid, imgui_consuming_input);    

        grid.update();       
        
        ImGui::SFML::Update(window, delta_time);

        imgui();

        window.clear();
        grid.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
} // namespace Frontend
