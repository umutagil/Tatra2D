#include "RenderGUISystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>

void RenderGUISystem::Update(Registry& registry, const SDL_Rect& camera)
{
	// Draw ImGui objects
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Spawn enemies")) {

		// Sprite asset Id

		const char* spriteAssetIds[] = { "tank-texture", "truck-texture" };
		static int item_current_idx = 0; // Here we store our selection data as an index.
		const char* combo_preview_value = spriteAssetIds[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Combo("Texture ID", &item_current_idx, spriteAssetIds, IM_ARRAYSIZE(spriteAssetIds));
		}
		const char* selectedEnemyAssetId = spriteAssetIds[item_current_idx];
		ImGui::Spacing();

		// Transform component values

		static int position[2] = { 0, 0 };
		static float scale[2] = { 1.0f, 1.0f };
		static float rotation = 0.0f;
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::InputInt2("position", position);
			ImGui::InputFloat2("scale", scale);
			ImGui::SliderAngle("rotation", &rotation, 0, 360);
		}
		ImGui::Spacing();

		// Rigidbody component values

		static float velocity[2] = { 0.0f, 0.0f };
		if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::InputFloat2("velocity", velocity);
		}
		ImGui::Spacing();

		// Projectile emitter component values

		static float projectileSpeed = 0.0f;
		static float projectileAngle = 0;
		static float repeatFrequencyS = 0.0f;
		static float projectileDuration = 0.0f;
		static int hitDamage = 0;
		if (ImGui::CollapsingHeader("Projectile", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderFloat("speed (px/sec)", &projectileSpeed, 10.0f, 200.0f);
			ImGui::SliderAngle("angle", &projectileAngle, 0, 360);
			ImGui::InputFloat("repeat frequency (sec)", &repeatFrequencyS);
			ImGui::InputFloat("duration (sec)", &projectileDuration);
			ImGui::InputInt("hit damage", &hitDamage);
		}
		ImGui::Spacing();

		// Health component values

		static int health = 0;
		if (ImGui::CollapsingHeader("Total Health", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderInt("health", &health, 0, 100);
		}
		ImGui::Spacing();

		if (ImGui::Button("Spawn")) {
			EnemyProperties properties;
			properties.assetId = selectedEnemyAssetId;

			properties.position = glm::ivec2(position[0], position[1]);
			properties.scale = glm::vec2(scale[0], scale[1]);
			properties.rotation = glm::degrees(rotation);

			properties.velocity = glm::vec2(velocity[0], velocity[1]);

			properties.projectileVelocity = projectileSpeed * glm::vec2(glm::cos(projectileAngle), glm::sin(projectileAngle));
			properties.repeatFrequencyS = repeatFrequencyS;
			properties.projectileDurationS = projectileDuration;
			properties.hitDamage = hitDamage;
			//properties.isFriendly = false;

			properties.health = health;

			CreateEnemy(registry, properties);
		}
	}
	ImGui::End();

	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
	ImGui::SetNextWindowBgAlpha(0.9f);
	if (ImGui::Begin("Map coordinates", nullptr, windowFlags)){
		ImGui::Text(
			"Map coordinates (x:%.1f, y:%.1f)",
			ImGui::GetIO().MousePos.x + camera.x,
			ImGui::GetIO().MousePos.y + camera.y
		);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

}

void RenderGUISystem::CreateEnemy(Registry& registry, const EnemyProperties& properties)
{
	const int spriteWidth = 32;
	const int spriteHeight = 32;
	const int enemyZIndex = 2;
	const int colliderWidth = spriteWidth;
	const int colliderHeight = spriteHeight;

	Entity enemy = registry.CreateEntity();
	enemy.Group("enemies");
	enemy.AddComponent<TransformComponent>(properties.position, properties.scale, properties.rotation);
	enemy.AddComponent<RigidBodyComponent>(properties.velocity);
	enemy.AddComponent<SpriteComponent>(properties.assetId, spriteWidth, spriteHeight, enemyZIndex);
	enemy.AddComponent<BoxColliderComponent>(colliderWidth, colliderHeight);
	enemy.AddComponent<ProjectileEmitterComponent>(properties.projectileVelocity, properties.repeatFrequencyS, 
												   properties.projectileDurationS, properties.hitDamage, properties.isFriendly);
	enemy.AddComponent<HealthComponent>(properties.health);

}
