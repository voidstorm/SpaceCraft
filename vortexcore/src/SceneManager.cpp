#include "..\include\VortexCore\SceneManager.h"
#include "..\include\VortexCore\RenderContext.h"
#include "..\include\VortexCore\SceneGraph.h"
#include "..\include\VortexCore\Scene.h"


//--------------------------------------------------------------------------
Vt::Scene::SceneManager::SceneManager(Vt::Gfx::RenderContext &render_context):
    mRenderContext(render_context)
{
}

//--------------------------------------------------------------------------
Vt::Scene::SceneManager::~SceneManager() {
}

//--------------------------------------------------------------------------
Vt::Gfx::RenderContext & Vt::Scene::SceneManager::renderContext() const{
    return mRenderContext;
}

//--------------------------------------------------------------------------
void Vt::Scene::SceneManager::addScene(std::unique_ptr<Vt::Scene::Scene>&& scene) {
    mScenes.emplace(scene->name(), std::move(scene));
}

//--------------------------------------------------------------------------
Vt::Scene::Scene & Vt::Scene::SceneManager::findSceneByName(const std::string & name) {
    auto scene = mScenes.find(name);
    if ( scene != mScenes.end() ) {
        return std::ref(*(scene->second));
    }
    throw std::exception((std::string("SceneManager::findSceneByName: ") + name).c_str());
}

//--------------------------------------------------------------------------
void Vt::Scene::SceneManager::loadResources() {
   _loadResources();
}

//--------------------------------------------------------------------------
Vt::Scene::SceneGraph & Vt::Scene::SceneManager::sceneGraph() {
    return *mSceneGraph.get();
}

//--------------------------------------------------------------------------
std::vector<Vt::Scene::Scene*> Vt::Scene::SceneManager::activeScenes() {
   std::vector<Vt::Scene::Scene*> scenes;
   {
      std::lock_guard<decltype(m_act_lock)> l(m_act_lock);
      scenes = mActiveScenes;
   }
   return std::move(scenes);
}

//--------------------------------------------------------------------------
std::vector<Vt::Scene::Scene*> Vt::Scene::SceneManager::visibleScenes() {
   std::vector<Vt::Scene::Scene*> scenes;
   {
      std::lock_guard<decltype(m_vis_lock)> l(m_vis_lock);
      scenes = mVisibleScenes;
   }
   return std::move(scenes);
}

void Vt::Scene::SceneManager::_loadResources() {
}
