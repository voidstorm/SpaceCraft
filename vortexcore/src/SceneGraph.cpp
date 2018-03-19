#include "..\include\VortexCore\SceneGraph.h"
#include "..\include\VortexCore\SceneObject.h"


Vt::Scene::SceneGraph::SceneGraph() {
}


Vt::Scene::SceneGraph::~SceneGraph() {
}

void Vt::Scene::SceneGraph::addSceneObject(const std::shared_ptr<Vt::Scene::SceneObject>& object, const std::shared_ptr<Vt::Scene::SceneObject>& parent) {
}

void Vt::Scene::SceneGraph::clear() {
}

void Vt::Scene::SceneGraph::removeSceneObject(const std::shared_ptr<Vt::Scene::SceneObject>& object) {
}

void Vt::Scene::SceneGraph::removeSceneObjectByName(const std::string & name) {
}

std::shared_ptr<Vt::Scene::SceneObject> Vt::Scene::SceneGraph::findSceneObjectByName(const std::string & name) {
   return std::shared_ptr<Vt::Scene::SceneObject>();
}

std::shared_ptr<Vt::Scene::SceneObject> Vt::Scene::SceneGraph::root() {
   return std::shared_ptr<Vt::Scene::SceneObject>();
}
