#pragma once

#include "common.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "frontend_grid.h"

namespace Frontend {   
    void on_close(const std::optional<sf::Event::Closed>& event, sf::RenderWindow& window);

    void on_resize(const std::optional<sf::Event::Resized>& resized, sf::RenderWindow& window);

    void on_key_press(const std::optional<sf::Event::KeyPressed>& keyPressed, sf::RenderWindow& window, DisplayGrid& grid);

    void on_mouse_wheel(const std::optional<sf::Event::MouseWheelScrolled>& mouseScroll, sf::RenderWindow& window, DisplayGrid& grid);

    void on_mouse_drag(const std::optional<sf::Event::MouseMoved>& mouseMoved, sf::RenderWindow& window, DisplayGrid& grid);;
}
