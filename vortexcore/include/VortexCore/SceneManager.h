#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include "Scene.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <vector>

//The scene manager is responsible loads and manages scenes.

namespace Vt {
    namespace Scene {
        class VORTEX_API SceneManager {
           friend class Scene;
        public:
            SceneManager(Vt::Gfx::RenderContext &render_context);
            SceneManager(const SceneManager&)=delete;
            virtual ~SceneManager();
            virtual Vt::Gfx::RenderContext& renderContext() const;
            //adds a scene to the scene manager
            virtual void addScene(std::unique_ptr<Vt::Scene::Scene>&& scene);
            //returs a scene by name
            virtual Vt::Scene::Scene& findSceneByName(const std::string &name);
            //loads resources
            virtual void loadResources();
            //returns the associated scene graph
            virtual Vt::Scene::SceneGraph& sceneGraph();
            //returns all active scenes
            virtual std::vector<Vt::Scene::Scene*> activeScenes();
            //returns all visible scenes
            virtual std::vector<Vt::Scene::Scene*> visibleScenes();

        private:
           void _loadResources();

        private:
            Vt::Gfx::RenderContext &mRenderContext;
            std::unique_ptr<Vt::Scene::SceneGraph> mSceneGraph;
            std::unordered_map<std::string, std::unique_ptr<Vt::Scene::Scene>> mScenes;
            std::vector<Vt::Scene::Scene*> mActiveScenes;
            std::vector<Vt::Scene::Scene*> mVisibleScenes;
            std::recursive_mutex m_act_lock;
            std::recursive_mutex m_vis_lock;
        };
    }
}

