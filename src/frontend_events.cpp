#include "frontend_events.h"

namespace Frontend {

    const float PAN_SPEED = 100.0f;

    void on_close(const std::optional<sf::Event::Closed>& event, sf::RenderWindow& window) {
        window.close();
        frontend_message_queue.push(Message{Message::MessageType::EXIT});
    }

    void on_resize(const std::optional<sf::Event::Resized>& resized, sf::RenderWindow& window) {
        // Update the view size and center based on window size
        sf::View view;
        view.setSize(sf::Vector2f(static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)));
        view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
        window.setView(view);
    }

    void on_key_press(const std::optional<sf::Event::KeyPressed>& keyPressed, sf::RenderWindow& window, DisplayGrid& grid) {

        if (keyPressed->code == sf::Keyboard::Key::Space) {
            simulation_paused = !simulation_paused;
            frontend_message_queue.push(Message{simulation_paused ? Message::MessageType::PAUSE : Message::MessageType::RUN});
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            frontend_message_queue.push(Message{Message::MessageType::EXIT});
            window.close();
        }
        if (keyPressed->code == sf::Keyboard::Key::Right || keyPressed->code == sf::Keyboard::Key::E) {
            frontend_message_queue.push(Message{Message::MessageType::STEP});
            simulation_paused = true;
        }
        if (keyPressed->code == sf::Keyboard::Key::R) {
            frontend_message_queue.push(Message{Message::MessageType::RESET});
            simulation_paused = true;
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
    }

    void on_mouse_wheel(const std::optional<sf::Event::MouseWheelScrolled>& mouseScroll, sf::RenderWindow& window, DisplayGrid& grid) {
        // Zoom in/out based on mouse wheel
        float zoom_factor = (mouseScroll->delta > 0) ? 1.1f : 0.9f;
        grid.zoom(zoom_factor, window);
    }

    void on_mouse_drag(const std::optional<sf::Event::MouseMoved>& mouseMoved, sf::RenderWindow& window, DisplayGrid& grid) {
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
    }
}