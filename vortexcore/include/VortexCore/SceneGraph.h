#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include "Aggregate.h"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <future>
//Generic scene graph 

namespace Vt {
	namespace Scene {
		class VORTEX_API SceneGraph : public Vt::Aggregate {
		public:
         enum class IterationOrder : unsigned {
            IO_DEPTH_FIRST,
            IO_BREADTH_FIRST
         };

         friend class Vt::Scene::Scene;
			SceneGraph(const std::shared_ptr<Vt::Scene::SceneObject> &root);
			virtual ~SceneGraph();
         virtual std::shared_ptr<Vt::Scene::SceneObject> addSceneObject(const std::shared_ptr<Vt::Scene::SceneObject> &object, const std::shared_ptr<Vt::Scene::SceneObject> &parent);
         virtual void addSceneObjects(const std::vector<std::shared_ptr<Vt::Scene::SceneObject>> &objects, const std::shared_ptr<Vt::Scene::SceneObject> &parent);
         virtual void clear();
         virtual void removeSceneObject(const std::shared_ptr<Vt::Scene::SceneObject> &object);
         virtual void removeSceneObjectByName(const std::string &name);
         virtual std::shared_ptr<Vt::Scene::SceneObject> findSceneObjectByName(const std::string &name);
         virtual std::shared_ptr<Vt::Scene::SceneObject> root();

         std::vector<std::shared_ptr<SceneObject>>::iterator begin() {
            return mObjects.begin();
         }

         std::vector<std::shared_ptr<SceneObject>>::iterator end() {
            return mObjects.end();
         }

      private:
         //draws the scene
         void _draw(const std::chrono::high_resolution_clock::duration &delta);
         //logic/world tick
         void _tick(const std::chrono::high_resolution_clock::duration &delta);
         //reorders the scene graph for depth first or breadth iterating
         std::future<bool> _optimize(IterationOrder order= IterationOrder::IO_DEPTH_FIRST);


         std::vector<std::shared_ptr<SceneObject>> mObjects;
         std::vector<std::shared_ptr<SceneObject>> mObjectsOpt;
         std::vector<std::shared_ptr<SceneObject>> mObjectStack;
         std::shared_ptr<SceneObject> mRoot;
         bool mDirty = false;;
		};
	}
}

