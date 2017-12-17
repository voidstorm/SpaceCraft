#include "SceneTest.h"
//Testscene
#include "VortexCore/test/VkTriangle.h"

Sc::SceneTest::SceneTest(Vt::Scene::SceneManager & sceneManager) 
   :
   Vt::Scene::Scene("SceneTest", sceneManager) {
}

Sc::SceneTest::~SceneTest() {
}

void Sc::SceneTest::load() {
}

void Sc::SceneTest::unload() {
}

void Sc::SceneTest::beginPlay() {
}

void Sc::SceneTest::endPlay() {
}

void Sc::SceneTest::beginDraw() {
}

void Sc::SceneTest::draw(const std::chrono::high_resolution_clock::duration & delta) {
}

void Sc::SceneTest::endDraw() {
}

void Sc::SceneTest::tick(const std::chrono::high_resolution_clock::duration & delta) {
}

void Sc::SceneTest::_load() {
}

void Sc::SceneTest::_unload() {
}
