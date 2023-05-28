#include "RenderEditorGUISystem.h"

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <filesystem>
#include <algorithm>

#include "../AssetStore/AssetStore.h"
#include "../Game/SceneManager.h"
#include "../Game/LevelLoader.h"

#include "../Utilities/FileDialog.h"

void RenderEditorGUISystem::Update(Registry& registry, SceneManager& sceneManager, SDL_Renderer& renderer, AssetStore& assetStore)
{
    // Draw ImGui objects
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save map")) {
                const std::string fileName = FileDialog::SaveFile();
                if (fileName.empty()) {
                    return;
                }

                LevelLoader::SaveMap(fileName, registry, sceneManager);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Tile set")) {

                const std::string imageFileName = FileDialog::OpenImageFile();
                const std::filesystem::path path(imageFileName);
                const std::string assetId(path.stem().string());

                assetStore.AddTexture(renderer, assetId, imageFileName);

                int tileMapWidth = 0, tileMapHeight = 0;
                if (!SDL_QueryTexture(assetStore.GetTexture(assetId), nullptr, nullptr, &tileMapWidth, &tileMapHeight)) {
                    activeTilesetId = assetId;
                    activeTilesetDimensions = { tileMapWidth, tileMapHeight };
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
	}
    


    bool isImageLoaded = !activeTilesetId.empty();
    if (isImageLoaded) {
        DisplayLoadedTileset(sceneManager, assetStore);
    }


    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void RenderEditorGUISystem::DisplayLoadedTileset(SceneManager& sceneManager, AssetStore& assetStore)
{
    if (ImGui::Begin("Texture", NULL, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::Image(assetStore.GetTexture(activeTilesetId), ImVec2(activeTilesetDimensions.x, activeTilesetDimensions.y));
        const glm::ivec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

        const int tileSize = 32;
        const int offsetX = 8, offsetY = 26;
        const ImVec2 windowPos = ImGui::GetWindowPos();
        const glm::ivec2 tileStartPos(windowPos.x + offsetX, windowPos.y + offsetY);

        const glm::ivec2 relativeMousePos = mousePos - tileStartPos;
        const bool isHoverOverTileMap = (relativeMousePos.x >= 0 && relativeMousePos.x <= activeTilesetDimensions.x &&
                                        relativeMousePos.y >= 0 && relativeMousePos.y <= activeTilesetDimensions.y);

        if (isHoverOverTileMap) {
            const int tileIdxX = relativeMousePos.x / tileSize;
            const int tileIdxY = relativeMousePos.y / tileSize;

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            const ImVec2 rectMin(tileStartPos.x + (tileIdxX * tileSize), tileStartPos.y + (tileIdxY * tileSize));
            const ImVec2 rectMax(rectMin.x + tileSize, rectMin.y + tileSize);
            const ImU32 col = ImColor(10, 255, 10, 255);
            draw_list->AddRect(rectMin, rectMax, col);

            const int tileIdxLinear = tileIdxY * (activeTilesetDimensions.x / tileSize) + tileIdxX;

            if (ImGui::IsMouseClicked(0)) {
                sceneManager.SetActiveTile({ glm::ivec2(0), (tileIdxX * tileSize), (tileIdxY * tileSize), tileSize, tileSize, tileIdxLinear, activeTilesetId });
            }
        }

        ImGui::Checkbox("Snap To Grid", &sceneManager.snapToGrid);

        const GridProperties& grid = sceneManager.GetGridProperties();
        int cellCountX = grid.cellCountX;
        int cellCountY = grid.cellCountY;

        if (ImGui::InputInt("Grid Width", &cellCountX, 1, 5)) {
            sceneManager.SetGridSize(cellCountX, cellCountY);
        }

        if (ImGui::InputInt("Grid Height", &cellCountY, 1, 5)) {
            sceneManager.SetGridSize(cellCountX, cellCountY);
        }
    }
    ImGui::End();

}
