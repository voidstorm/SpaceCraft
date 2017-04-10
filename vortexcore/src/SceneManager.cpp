#include "..\include\VortexCore\SceneManager.h"
#include "..\include\VortexCore\RenderContext.h"
#include "..\include\VortexCore\SceneGraph.h"
#include "..\include\VortexCore\Scene.h"


Vt::Scene::SceneManager::SceneManager(Vt::Gfx::RenderContext &render_context):
    mRenderContext(render_context)
{
}


Vt::Scene::SceneManager::~SceneManager() {
}

Vt::Gfx::RenderContext & Vt::Scene::SceneManager::renderContext() {
    return mRenderContext;
}

void Vt::Scene::SceneManager::addScene(std::unique_ptr<Vt::Scene::Scene>&& scene) {
    mScenes.emplace(scene->name(), std::move(scene));
}

Vt::Scene::Scene & Vt::Scene::SceneManager::findSceneByName(const std::string & name) {
    auto scene = mScenes.find(name);
    if ( scene != mScenes.end() ) {
        return std::ref(*(scene->second));
    }
    throw std::exception((std::string("SceneManager::findSceneByName: ") + name).c_str());
}

Vt::Scene::SceneGraph & Vt::Scene::SceneManager::sceneGraph() {
    return *mSceneGraph.get();
}
