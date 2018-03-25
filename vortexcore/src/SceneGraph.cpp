#include "..\include\VortexCore\SceneGraph.h"
#include "..\include\VortexCore\SceneObject.h"

#define INIT_OBJECT_SIZE 1024

Vt::Scene::SceneGraph::SceneGraph(const std::shared_ptr<Vt::Scene::SceneObject> &root) :
   mRoot(root) {
   mObjects.reserve(INIT_OBJECT_SIZE);
   mObjectsOpt.reserve(INIT_OBJECT_SIZE);
   mObjectStack.reserve(INIT_OBJECT_SIZE);
}


Vt::Scene::SceneGraph::~SceneGraph() {
}

std::shared_ptr<Vt::Scene::SceneObject> Vt::Scene::SceneGraph::addSceneObject(const std::shared_ptr<Vt::Scene::SceneObject>& object, const std::shared_ptr<Vt::Scene::SceneObject>& parent) {
   parent->addChild(object);
   return object;
   mDirty = true;
}

void Vt::Scene::SceneGraph::addSceneObjects(const std::vector<std::shared_ptr<Vt::Scene::SceneObject>>& objects, const std::shared_ptr<Vt::Scene::SceneObject>& parent) {
   parent->addChildren(objects);
   mDirty = true;
}

void Vt::Scene::SceneGraph::clear() {
}

void Vt::Scene::SceneGraph::removeSceneObject(const std::shared_ptr<Vt::Scene::SceneObject>& object) {
   mDirty = true;
}

void Vt::Scene::SceneGraph::removeSceneObjectByName(const std::string & name) {
   mDirty = true;
}

std::shared_ptr<Vt::Scene::SceneObject> Vt::Scene::SceneGraph::findSceneObjectByName(const std::string & name) {
   return std::shared_ptr<Vt::Scene::SceneObject>();
}

std::shared_ptr<Vt::Scene::SceneObject> Vt::Scene::SceneGraph::root() {
   return mRoot;
}

void Vt::Scene::SceneGraph::_draw(const std::chrono::high_resolution_clock::duration & delta) {
   for (auto & obj : *this) {
      obj->_draw(delta);
   }
}

void Vt::Scene::SceneGraph::_tick(const std::chrono::high_resolution_clock::duration & delta) {
   auto opt = _optimize();
   for (auto & obj : *this) {
      obj->_tick(delta);
   }
   if (opt.get()) {
      //need to replace object container
   }
}

std::future<bool> Vt::Scene::SceneGraph::_optimize(IterationOrder order) {
   return std::async(std::launch::async, [this, order]()->bool {
      if (mDirty) {
         mDirty = false;
         //depth first
         if (order == IterationOrder::IO_DEPTH_FIRST) {
            mObjectStack.push_back(mRoot);
            std::shared_ptr<SceneObject> current_node = mRoot;
            while (mObjectStack.size()) {
               
            }
         } else {
            //breadth first

         }
         return true;
      }
      return false;
   });
}
