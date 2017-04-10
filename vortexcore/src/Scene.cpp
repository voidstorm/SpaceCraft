#include "..\include\VortexCore\Scene.h"
#include "..\include\VortexCore\TransformCache.h"



Vt::Scene::Scene::Scene()
  : mTransformCache(std::make_unique<Vt::Scene::TransformCache>(4096)){
}


Vt::Scene::Scene::~Scene() {
}

const std::string & Vt::Scene::Scene::name() const {
  return mName;
}

Vt::Scene::TransformCache & Vt::Scene::Scene::transformCache() {
  return *mTransformCache.get();
}

