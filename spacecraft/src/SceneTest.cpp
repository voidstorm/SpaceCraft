#include "SceneTest.h"
//Testscene
#include "VortexCore/test/VkTriangle.h"
#include "VortexCore/SceneObject.h"
#include "VortexCore/SceneGraph.h"
#include "VortexCore/Component.h"


Sc::SceneTest::SceneTest(Vt::Scene::SceneManager & sceneManager) 
   :
   Vt::Scene::Scene("SceneTest", sceneManager) {
}

Sc::SceneTest::~SceneTest() {
}

void Sc::SceneTest::load() {
   _load();
}

void Sc::SceneTest::unload() {
   _unload();
}

void Sc::SceneTest::onActivate() {
}

void Sc::SceneTest::onDeactivate() {
}

void Sc::SceneTest::onShow() {
}

void Sc::SceneTest::draw(const std::chrono::high_resolution_clock::duration & delta) {
}

void Sc::SceneTest::onHide() {
}

void Sc::SceneTest::tick(const std::chrono::high_resolution_clock::duration & delta) {
}

void Sc::SceneTest::_load() {
   //add some game objects to the scene
   sceneGraph().addSceneObject(std::make_shared<Vt::Scene::SceneObject>(*this, false, "Obj1"), sceneGraph().root());
   sceneGraph().addSceneObject(std::make_shared<Vt::Scene::SceneObject>(*this, false, "Obj2"), sceneGraph().root());
   sceneGraph().addSceneObject(std::make_shared<Vt::Scene::SceneObject>(*this, false, "Obj3"), sceneGraph().root());
}

void Sc::SceneTest::_unload() {
}
