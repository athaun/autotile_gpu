#include "common.h"
#include <vector>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <cmath>

namespace Frontend {


Grid grid = Grid();

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

    // Pan speed for WASD and arrow keys
    const float PAN_SPEED = 100.0f;

    const auto onClose = [&window](const std::optional<sf::Event::Closed>& event) {
        frontend_message_queue.push(Message{Message::MessageType::EXIT});
        ImGui::SFML::Shutdown();
        window.close();
    };

    const auto onResize = [&window](const std::optional<sf::Event::Resized>& resized) {
        // Update the view size and center based on window size
        sf::View view;
        view.setSize(sf::Vector2f(static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)));
        view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
        window.setView(view);
    };

    const auto onKeyPress = [&window, PAN_SPEED](const std::optional<sf::Event::KeyPressed>& keyPressed) {
        // Panning with WASD and arrow keys
        if (keyPressed->code == sf::Keyboard::Key::A) {
            grid.pan(PAN_SPEED, 0);
        }
        if (keyPressed->code == sf::Keyboard::Key::D) {
            grid.pan(-PAN_SPEED, 0);
        }
        if (keyPressed->code == sf::Keyboard::Key::W) {
            grid.pan(0, PAN_SPEED);
        }
        if (keyPressed->code == sf::Keyboard::Key::S) {
            grid.pan(0, -PAN_SPEED);
        }

        // Existing simulation control key handlers
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            frontend_message_queue.push(Message{Message::MessageType::EXIT});
            window.close();
        }
        if (keyPressed->code == sf::Keyboard::Key::Space) {
            frontend_message_queue.push(Message{Message::MessageType::PAUSE});
        }
        if (keyPressed->code == sf::Keyboard::Key::Right) {
            frontend_message_queue.push(Message{Message::MessageType::STEP});
        }
        if (keyPressed->code == sf::Keyboard::Key::R) {
            frontend_message_queue.push(Message{Message::MessageType::RUN});
        }
        if (keyPressed->code == sf::Keyboard::Key::P) {
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
    const auto onMouseWheel = [&window](const std::optional<sf::Event::MouseWheelScrolled>& mouseScroll) {
        // Zoom in/out based on mouse wheel
        float zoom_factor = (mouseScroll->delta > 0) ? 1.1f : 0.9f;
        grid.zoom(zoom_factor, window);
    };

    // New handler for mouse drag
    const auto onMouseDrag = [&window](const std::optional<sf::Event::MouseMoved>& mouseMoved) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (!grid.is_dragging) {
                grid.last_mouse_x = mouseMoved->position.x;
                grid.last_mouse_y = mouseMoved->position.y;
                grid.is_dragging = true;
            }
            float dx = mouseMoved->position.x - grid.last_mouse_x;
            float dy = mouseMoved->position.y - grid.last_mouse_y;
            grid.pan(dx, dy);
            grid.last_mouse_x = mouseMoved->position.x;
            grid.last_mouse_y = mouseMoved->position.y;
        } else {
            grid.is_dragging = false;
        }
    };

    sf::Clock clock;
    while (window.isOpen()) {
        std::optional<sf::Event> event;
        while (event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            ImGuiIO& io = ImGui::GetIO();

            if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
                // Handle the event based on its type
                if (const auto* closed = event->getIf<sf::Event::Closed>()) {
                    onClose(*closed);
                } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                    onResize(*resized);
                } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    onKeyPress(*keyPressed);
                } else if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    onMouseWheel(*mouseWheel);
                } else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                    onMouseDrag(*mouseMoved);
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
