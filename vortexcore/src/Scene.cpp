#include "..\include\VortexCore\Scene.h"
#include "..\include\VortexCore\TransformCache.h"
#include "..\include\VortexCore\SceneManager.h"


//--------------------------------------------------------------------------
//
Vt::Scene::Scene::Scene(SceneManager& sceneManager)
   : mTransformCache(std::make_unique<Vt::Scene::TransformCache>(4096)) 
   , mSceneManager(sceneManager)
   , mRenderContext(sceneManager.renderContext()){
}


//--------------------------------------------------------------------------
//
Vt::Scene::Scene::~Scene() {
}

//--------------------------------------------------------------------------
//
const std::string & Vt::Scene::Scene::name() const {
   return mName;
}

//--------------------------------------------------------------------------
//
Vt::Scene::TransformCache & Vt::Scene::Scene::transformCache() {
   return *mTransformCache.get();
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::load() {

}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::unload() {
    
}


//--------------------------------------------------------------------------
//
std::future<void> Vt::Scene::Scene::loadAsync() {
   return std::future<void>();
}

//--------------------------------------------------------------------------
//
std::future<void> Vt::Scene::Scene::unloadAsync() {
   return std::future<void>();
}

//--------------------------------------------------------------------------
//
float Vt::Scene::Scene::loadProgress() {
   return 0.0f;
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::activate() {
   {
      std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
      if (!mActive) {
         mSceneManager.mActiveScenes.push_back(this);
         mActive = true;
      }
   }
}

//--------------------------------------------------------------------------
//
bool Vt::Scene::Scene::active() const {
   return mActive;
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::deactivate() {
   {
      std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
      if (mActive) {
         auto s = std::find(mSceneManager.mActiveScenes.begin(), mSceneManager.mActiveScenes.end(), this);
         mSceneManager.mActiveScenes.erase(s);
         mActive = false;
      }
   }
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::show() {
   {
      std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
      if (!mVisible) {
         mSceneManager.mVisibleScenes.push_back(this);
         mVisible = true;
      }
   }
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::hide() {
   {
      std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
      if (mActive) {
         auto s = std::find(mSceneManager.mVisibleScenes.begin(), mSceneManager.mVisibleScenes.end(), this);
         mSceneManager.mVisibleScenes.erase(s);
         mVisible = false;
      }
   }
}

//--------------------------------------------------------------------------
//
bool Vt::Scene::Scene::visible() {
   return mVisible;
}

//--------------------------------------------------------------------------
//
Vt::Scene::SceneManager & Vt::Scene::Scene::sceneManager() const {
   return mSceneManager;
}

//--------------------------------------------------------------------------
//
Vt::Gfx::RenderContext & Vt::Scene::Scene::renderContext() const {
   return mRenderContext;
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::beginPlay() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::endPlay() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::beginDraw() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::draw(const std::chrono::high_resolution_clock::duration & delta) {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::endDraw() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::tick(const std::chrono::high_resolution_clock::duration & delta) {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::_load() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::_unload() {
}

