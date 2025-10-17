#include "ImGuiTheme.h"

namespace ImGuiTheme {
    void ApplyOrangeAcrylicTheme() {
        ImGui::StyleColorsDark();
        
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 3.0f;

        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);

        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;
        
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;

        // Acrylic translucent backgrounds
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.12f, 0.13f, 0.70f);  // More transparent
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.16f, 0.17f, 0.60f);   // Translucent child windows
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.12f, 0.13f, 0.85f);   // Slightly less transparent for popups
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.14f, 0.15f, 0.75f); // Translucent menu bar

        // Orange acrylic title bars with transparency
        colors[ImGuiCol_TitleBg] = ImVec4(0.8f, 0.4f, 0.0f, 0.65f);       // Translucent orange
        colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.5f, 0.0f, 0.85f); // More opaque when active
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.8f, 0.4f, 0.0f, 0.45f); // Very transparent when collapsed

        // Orange translucent tabs
        colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.4f, 0.0f, 0.60f);           // Translucent orange tabs
        colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.6f, 0.1f, 0.80f);    // More visible on hover
        colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.5f, 0.0f, 0.90f);     // Nearly opaque when active
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.6f, 0.3f, 0.0f, 0.40f);  // Very transparent when unfocused
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.8f, 0.4f, 0.0f, 0.65f); // Semi-transparent
        
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 0.4f, 0.0f, 0.75f);

        colors[ImGuiCol_Border] = ImVec4(0.6f, 0.3f, 0.0f, 0.5f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        
        // Semi-transparent frame backgrounds for acrylic effect
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 0.80f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.27f, 0.29f, 0.85f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.20f, 0.22f, 0.90f);
        
        // Semi-transparent buttons
        colors[ImGuiCol_Button] = ImVec4(0.24f, 0.27f, 0.30f, 0.75f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.34f, 0.38f, 0.85f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.2f, 0.22f, 0.90f);

        colors[ImGuiCol_Header] = ImVec4(0.8f, 0.4f, 0.0f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.5f, 0.0f, 0.8f);
        colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        
        colors[ImGuiCol_Separator] = ImVec4(0.6f, 0.3f, 0.0f, 0.5f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(1.0f, 0.5f, 0.0f, 0.8f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.12f, 0.13f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 0.8f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.9f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);
        
        colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.5f, 0.0f, 0.78f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.1f, 1.0f);
        
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.8f, 0.4f, 0.0f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.5f, 0.0f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.6f, 0.1f, 0.95f);
        
        colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.5f, 0.0f, 0.67f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.1f, 1.0f);
        
        
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.8f, 0.4f, 0.0f, 0.52f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.6f, 0.3f, 0.0f, 1.0f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.5f, 0.25f, 0.0f, 0.5f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 0.5f, 0.0f, 0.9f);
        
        colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
        
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f ,0.6f);
    }
    void ApplyDarkTheme() {
        ImGui::StyleColorsDark();
    }
}