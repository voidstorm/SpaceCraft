#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include "Aggregate.h"
#include <vector>
#include <string>
#include <memory>
//Generic scene graph 

namespace Vt {
	namespace Scene {
		class VORTEX_API SceneGraph : public Vt::Aggregate {
		public:
			SceneGraph();
			virtual ~SceneGraph();
         virtual void addSceneObject(const std::shared_ptr<Vt::Scene::SceneObject> &object, const std::shared_ptr<Vt::Scene::SceneObject> &parent);
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
         std::vector<std::shared_ptr<SceneObject>> mObjects;
         std::shared_ptr<SceneObject> mRoot;
		};
	}
}

