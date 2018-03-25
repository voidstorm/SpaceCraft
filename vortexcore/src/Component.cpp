#include "..\include\VortexCore\Component.h"
#include "..\include\VortexCore\SceneObject.h"


Vt::Scene::Component::Component(Vt::Scene::SceneObject& parent, const std::string& name):
    mParent(parent)
  , mName(name){
}


Vt::Scene::Component::~Component() {
}

const std::string & Vt::Scene::Component::name() const {
  return mName;
}

bool Vt::Scene::Component::canTick() const {
  return mCanTick;
}

bool Vt::Scene::Component::active() const {
  return mActive;
}

Vt::Scene::SceneObject & Vt::Scene::Component::parent() const {
  return mParent;
}


void Vt::Scene::Component::activate() {
  onActivate();
  OnActivate(shared_from_this());
  mActive = true;
}

void Vt::Scene::Component::deactivate() {
  onDeactivate();
  OnDeactivate(shared_from_this());
  mActive = false;
}

void Vt::Scene::Component::onActivate() {

}

void Vt::Scene::Component::onDeactivate() {

}

void Vt::Scene::Component::onBeginPlay(Vt::Scene::SceneObject& sender) {
  OnBeginPlay(shared_from_this());
}

void Vt::Scene::Component::onEndPlay(Vt::Scene::SceneObject& sender) {
  OnEndPlay(shared_from_this());
}

void Vt::Scene::Component::onTick(Vt::Scene::SceneObject& sender, const std::chrono::high_resolution_clock::duration & delta) {

}

void Vt::Scene::Component::onDraw(Vt::Scene::SceneObject & sender, const std::chrono::high_resolution_clock::duration & delta) {
}

void Vt::Scene::Component::_draw(Vt::Scene::SceneObject& sender, const std::chrono::high_resolution_clock::duration & delta) {
   onDraw(sender, delta);
   OnDraw(shared_from_this(), delta);
}

void Vt::Scene::Component::_tick(Vt::Scene::SceneObject& sender, const std::chrono::high_resolution_clock::duration & delta) {
   onTick(sender, delta);
   OnTick(shared_from_this(), delta);

}


