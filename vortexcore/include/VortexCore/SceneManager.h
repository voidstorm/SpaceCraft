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
   friend class Game;
public:
   SceneManager(Vt::Gfx::RenderContext &render_context, Vt::Game &game);
   SceneManager(const SceneManager&) = delete;
   virtual ~SceneManager();
   virtual Vt::Gfx::RenderContext& renderContext() const;
   //adds a scene to the scene manager
   virtual Vt::Scene::Scene & addScene(std::unique_ptr<Vt::Scene::Scene>&& scene);
   //returs a scene by name
   virtual Vt::Scene::Scene& findSceneByName(const std::string &name);
   //loads resources
   virtual void loadResources();
   //unloads resources
   virtual void unloadResources();
   //returns the associated scene graph
   virtual Vt::Scene::SceneGraph& sceneGraph();
   //returns all active scenes
   virtual std::vector<Vt::Scene::Scene*> activeScenes();
   //returns all visible scenes
   virtual std::vector<Vt::Scene::Scene*> visibleScenes();
   //return current game
   virtual Vt::Game &game();
private:
   void _loadResources();
   void _unloadResources();

private:
   Vt::Game &mGame;
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

