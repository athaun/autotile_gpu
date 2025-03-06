#include "frontend_gui.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include "common.h"

namespace Frontend {
    void set_imgui_theme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Dark background
        colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Window background
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Child window background
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.92f); // Popup background

        // Light Grey accents
        colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Border color
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // Border shadow
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Frame background (e.g., input fields)
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Frame background hovered
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Frame background active

        // Button and Active Button
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Button color
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f); // Hovered button color
        colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Active button color

        // Text colors
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f); // Text color
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Disabled text color

        // Highlight colors
        colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Separator color
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Hovered separator color
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Active separator color

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Scrollbar background
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Scrollbar grab color
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Hovered scrollbar grab
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f); // Active scrollbar grab

        // Title Bar
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f); // Title bar background
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f); // Collapsed title bar background
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Active title bar background

        // Menu bar
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f); // Menu bar background

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->ClearFonts();
        io.Fonts->AddFontFromFileTTF("assets/font.ttf", 20.0f);
        ImGui::SFML::UpdateFontTexture();

        style.WindowRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.GrabRounding = 5.0f;
        style.ChildRounding = 5.0f;
        style.PopupRounding = 5.0f;
        style.FrameBorderSize = 1.0f;
        style.ScrollbarSize = 15.0f;
        style.GrabMinSize = 10.0f;

        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(5, 5);
        style.ItemSpacing = ImVec2(5, 5);
    }

    void top_bar() {
        // Set window flags to make it full-width and hide the collapsible bar
        ImGui::Begin("Top Bar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
        
        // Make window full-width by setting size to the SFML window size
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 50));
        ImGui::SetWindowPos(ImVec2(0, 0));

        ImGui::SameLine();

        // Step Button
        if (ImGui::Button("Exit")) {
            frontend_message_queue.push({Message::MessageType::EXIT});
        }

        ImGui::SameLine();

        // Step Button
        if (ImGui::Button("Reset")) {
            simulation_paused = true;
            frontend_message_queue.push({Message::MessageType::RESET});
        }

        ImGui::SameLine();

        // Step Button
        if (ImGui::Button("Step")) {
            frontend_message_queue.push({Message::MessageType::STEP});
            simulation_paused = true;
        }

        ImGui::SameLine();

        // Play/Pause Button
        if (ImGui::Button(simulation_paused ? "Play" : "Pause")) {
            simulation_paused = !simulation_paused;
            if (simulation_paused) {
                frontend_message_queue.push({Message::MessageType::PAUSE});
            } else {
                frontend_message_queue.push({Message::MessageType::RUN});
            }
        }
        ImGui::SameLine();

        ImGui::End();
    }

    void imgui() {
        top_bar();
    }
}