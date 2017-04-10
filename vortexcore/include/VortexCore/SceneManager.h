#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include "Scene.h"

#include <string>
#include <memory>
#include <unordered_map>

//The scene manager is responsible loads and manages scenes.

namespace Vt {
    namespace Scene {
        class VORTEX_API SceneManager {
        public:
            SceneManager(Vt::Gfx::RenderContext &render_context);
            SceneManager(const SceneManager&)=delete;
            virtual ~SceneManager();
            virtual Vt::Gfx::RenderContext& renderContext();
            //adds a scene to the scene manager
            virtual void addScene(std::unique_ptr<Vt::Scene::Scene>&& scene);
            //returs a scene by name
            virtual Vt::Scene::Scene& findSceneByName(const std::string &name);
            //returns the associated scene graph
            virtual Vt::Scene::SceneGraph& sceneGraph();
        private:
            Vt::Gfx::RenderContext &mRenderContext;
            std::unique_ptr<Vt::Scene::SceneGraph> mSceneGraph;
            std::unordered_map<std::string,std::unique_ptr<Vt::Scene::Scene>> mScenes;
        };
    }
}

