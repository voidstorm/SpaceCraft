#include "..\include\VortexCore\SceneObject.h"
#include "..\include\VortexCore\Scene.h"
#include "..\include\VortexCore\TransformCache.h"


//-------------------------------------------------------------------------------------------------
Vt::Scene::SceneObject::SceneObject(Scene &scene, const bool is_static, const std::string& name)
   : mStatic(is_static)
   , mScene(scene)
   , mName(name)
   , mTransform(scene.transformCache().requestTransform()) {
}


//-------------------------------------------------------------------------------------------------
Vt::Scene::SceneObject::~SceneObject() {
   mScene.transformCache().revokeTransform(mTransform);
}

void Vt::Scene::SceneObject::addChild(const std::shared_ptr<SceneObject>& child) {
   child->mParent = shared_from_this();
   mChildren.push_back(child);
}

void Vt::Scene::SceneObject::addChildren(const std::vector<std::shared_ptr<SceneObject>>& children) {
   for (auto &c : children) {
      c->mParent = shared_from_this();
   }
   mChildren.insert(mChildren.end(), children.begin(), children.end());
}

std::vector<std::shared_ptr<Vt::Scene::SceneObject>>& Vt::Scene::SceneObject::children() {
   return mChildren;
}

////-------------------------------------------------------------------------------------------------
//bool Vt::Scene::SceneObject::addComponent(std::unique_ptr<Component>&& component) {
//  auto it=std::find_if(mComponents.begin(), mComponents.end(), [&] (auto &p) {
//    if ( p->name() == component->name() )
//      return true;
//    else
//      return false;
//  });
//  if ( it == mComponents.end() ) {
//    mComponents.emplace_back(std::move(component));
//    return true;
//  }
//  return false;
//}
//
////-------------------------------------------------------------------------------------------------
//std::weak_ptr<Vt::Scene::Component> Vt::Scene::SceneObject::getComponentByName(const std::string & name) {
//  for ( const auto &i : mComponents ) {
//    if ( name == i->name() ) {
//      return i;
//    }
//  }
//  throw std::exception("SceneObject::getComponentByName: invalid name");
//}

//-------------------------------------------------------------------------------------------------
const std::string & Vt::Scene::SceneObject::name() const {
   return mName;
}

//-------------------------------------------------------------------------------------------------
bool Vt::Scene::SceneObject::canTick() const {
   return mCanTick;
}

//-------------------------------------------------------------------------------------------------
bool Vt::Scene::SceneObject::active() const {
   return mActive;
}

//-------------------------------------------------------------------------------------------------
bool Vt::Scene::SceneObject::visible() const {
   return mVisible;
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::show() {
   onShow();
   OnShow(*this);
   mVisible = true;
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::hide() {
   onHide();
   OnHide(*this);
   mVisible = false;
}

bool Vt::Scene::SceneObject::isStatic() const {
   return mStatic;
}

Vt::Scene::Scene & Vt::Scene::SceneObject::scene() {
   return  mScene;
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::activate() {
   if (mActive)
      return;
   onActivate();
   OnActivate(*this);
   for (auto &c : *this) {
      c.second->onActivate();
   }
   mActive = true;
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::deactivate() {
   if (!mActive)
      return;
   _onDeactivate();
   OnDeactivate(*this);
   for (auto &c : *this) {
      c.second->deactivate();
   }
   mActive = false;
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::onActivate() {
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::onDeactivate() {
}

void Vt::Scene::SceneObject::onShow() {
}

void Vt::Scene::SceneObject::onHide() {
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::onBeginPlay() {
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::onEndPlay() {
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::onTick(const std::chrono::high_resolution_clock::duration & delta) {
}

void Vt::Scene::SceneObject::onDraw(const std::chrono::high_resolution_clock::duration & delta) {
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::_onActivate() {
   if (!mActive)
      return;
   onBeginPlay();
   OnBeginPlay(*this);
   for (auto &c : *this) {
      c.second->onBeginPlay(*this);
   }
}

//-------------------------------------------------------------------------------------------------
void Vt::Scene::SceneObject::_onDeactivate() {
   if (!mActive)
      return;
   onEndPlay();
   OnEndPlay(*this);
   for (auto &c : *this) {
      c.second.get()->onEndPlay(*this);
   }
}

void Vt::Scene::SceneObject::_tick(const std::chrono::high_resolution_clock::duration &delta) {
   if (!mActive || !mCanTick)
      return;
   onTick(delta);
   OnTick(*this, delta);
   for (auto &c : *this) {
      c.second.get()->_tick(*this, delta);
   }
}

void Vt::Scene::SceneObject::_draw(const std::chrono::high_resolution_clock::duration & delta) {
   if (!mVisible)
      return;
   onDraw(delta);
   OnDraw(*this, delta);
   for (auto &c : *this) {
      c.second.get()->_draw(*this, delta);
   }
}
