#include "VkTriangle.h"

Vt::Test::VkTriangle::VkTriangle(Vt::Scene::SceneManager & sceneManager) 
   :
   Vt::Scene::Scene("VkTriangle", sceneManager) {
}

Vt::Test::VkTriangle::~VkTriangle() {
}

void Vt::Test::VkTriangle::load() {
   Vt::Scene::Scene::load();
}

void Vt::Test::VkTriangle::unload() {
}

void Vt::Test::VkTriangle::onActivate() {
}

void Vt::Test::VkTriangle::onDeactivate() {
}

void Vt::Test::VkTriangle::onShow() {
}

void Vt::Test::VkTriangle::draw(const std::chrono::high_resolution_clock::duration & delta) {
}

void Vt::Test::VkTriangle::onHide() {
}

void Vt::Test::VkTriangle::tick(const std::chrono::high_resolution_clock::duration & delta) {
}
