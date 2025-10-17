#pragma once

#include <imgui.h>

class TestUI {
public:
    static void Render() {
        RenderMainMenuBar();
        RenderViewportPanel();
        RenderPropertiesPanel();
        RenderContentBrowser();
        RenderOutliner();
    }

private:
    static void RenderMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New", "Ctrl+N")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window")) {
                if (ImGui::MenuItem("Viewport")) {}
                if (ImGui::MenuItem("Properties")) {}
                if (ImGui::MenuItem("Content Browser")) {}
                if (ImGui::MenuItem("Outliner")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    static void RenderViewportPanel() {
        ImGui::SetNextWindowPos(ImVec2(20, 40), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);
        
        if (ImGui::BeginMenuBar()) {
            if (ImGui::Button("Perspective")) {}
            ImGui::SameLine();
            if (ImGui::Button("Top")) {}
            ImGui::SameLine();
            if (ImGui::Button("Front")) {}
            ImGui::SameLine();
            if (ImGui::Button("Side")) {}
            ImGui::EndMenuBar();
        }
        
        ImGui::Text("3D Viewport Area");
        ImGui::Text("Resolution: 800x600");
        ImGui::Text("FPS: 60");
        
        // Demo content
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + size.x, ImGui::GetCursorScreenPos().y + size.y),
            IM_COL32(40, 40, 45, 255)
        );
        
        ImGui::End();
    }

    static void RenderPropertiesPanel() {
        ImGui::SetNextWindowPos(ImVec2(840, 40), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Properties");
        
        ImGui::Text("Details Panel");
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Location");
            float location[3] = {0.0f, 0.0f, 0.0f};
            ImGui::DragFloat3("##Location", location, 0.1f);
            
            ImGui::Text("Rotation");
            float rotation[3] = {0.0f, 0.0f, 0.0f};
            ImGui::DragFloat3("##Rotation", rotation, 1.0f);
            
            ImGui::Text("Scale");
            float scale[3] = {1.0f, 1.0f, 1.0f};
            ImGui::DragFloat3("##Scale", scale, 0.1f);
        }
        
        if (ImGui::CollapsingHeader("Material")) {
            ImGui::ColorEdit3("Base Color", (float*)&ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            float metallic = 0.5f;
            ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
            float roughness = 0.5f;
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        }
        
        ImGui::End();
    }

    static void RenderContentBrowser() {
        ImGui::SetNextWindowPos(ImVec2(20, 660), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 200), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Content Browser");
        
        ImGui::Text("Assets/");
        ImGui::Separator();
        
        // Simulate asset grid
        for (int i = 0; i < 12; i++) {
            ImGui::Button(("Asset_" + std::to_string(i)).c_str(), ImVec2(80, 80));
            if ((i + 1) % 8 != 0) ImGui::SameLine();
        }
        
        ImGui::End();
    }

    static void RenderOutliner() {
        ImGui::SetNextWindowPos(ImVec2(840, 460), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Outliner");
        
        ImGui::Text("Scene Hierarchy");
        ImGui::Separator();
        
        if (ImGui::TreeNode("Scene")) {
            if (ImGui::TreeNode("Cameras")) {
                ImGui::Selectable("Main Camera");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Lights")) {
                ImGui::Selectable("Directional Light");
                ImGui::Selectable("Point Light 1");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Objects")) {
                ImGui::Selectable("Cube");
                ImGui::Selectable("Sphere");
                ImGui::Selectable("Plane");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        
        ImGui::End();
    }
};
