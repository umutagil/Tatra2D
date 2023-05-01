#include "RenderEditorGUISystem.h"

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <filesystem>

#include "../AssetStore/AssetStore.h"
#include "../Game/SceneManager.h"

#include "../Utilities/FileDialog.h"

void RenderEditorGUISystem::Update(SceneManager& sceneManager, SDL_Renderer& renderer, AssetStore& assetStore, const SDL_Rect& camera)
{
    // Draw ImGui objects
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            
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

                    // TODO: Should be done when grid from the texture is selected with mouse
                    sceneManager.SetActiveTile({ 0, 0, 32, 32, assetId });
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
	}
    
    bool isImageLoaded = !activeTilesetId.empty();
    if (isImageLoaded) {
        DisplayLoadedTileset(assetStore);
    }


    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void RenderEditorGUISystem::DisplayLoadedTileset(AssetStore& assetStore)
{
    if (ImGui::Begin("Texture", NULL, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::Image(assetStore.GetTexture(activeTilesetId), ImVec2(activeTilesetDimensions.x, activeTilesetDimensions.y));


    }
    ImGui::End();

}
