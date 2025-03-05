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

    if (!ImGui::SFML::Init(window)) {
        fmt::print(stderr, "Failed to initialize ImGui-SFML\n");
        return;
    }

    sf::Clock clock;
    while (window.isOpen()) {
        std::optional<sf::Event> event;
        while (event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            ImGuiIO& io = ImGui::GetIO();

            if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
                // Handle the event based on its type
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
        }
           

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

        ImGui::SFML::Update(window, clock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        grid.draw(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
} // namespace Frontend
