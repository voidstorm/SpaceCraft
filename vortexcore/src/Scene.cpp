#include "..\include\VortexCore\Scene.h"
#include "..\include\VortexCore\TransformCache.h"
#include "..\include\VortexCore\SceneManager.h"
#include "..\include\VortexCore\Game.h"
#include "..\include\VortexCore\ThreadContext.h"
#include "..\include\VortexCore\SystemLogger.h"


//--------------------------------------------------------------------------
//
Vt::Scene::Scene::Scene(const std::string &name, SceneManager& sceneManager)
   : mTransformCache(std::make_unique<Vt::Scene::TransformCache>(4096))
   , mSceneManager(sceneManager)
   , mRenderContext(sceneManager.renderContext())
   , mName(name) {
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
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _beginPlay();
      return 0;
   });
}

//--------------------------------------------------------------------------
//
bool Vt::Scene::Scene::active() const {
   return mActive;
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::deactivate() {
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _endPlay();
      return 0;
   });
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::show() {
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _beginDraw();
      return 0;
   });

}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::hide() {
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _endDraw();
      return 0;
   });
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
   SYSTEM_LOG_INFO("Scene::_load: %s", mName.c_str());
   load();
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::_unload() {
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _endPlay();
      return 0;
   }).get();
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _endDraw();
      return 0;
   }).get();
   unload();
   SYSTEM_LOG_INFO("Scene::_unload: %s", mName.c_str());
}

void Vt::Scene::Scene::_draw(const std::chrono::high_resolution_clock::duration & delta) {
   draw(delta);
}

void Vt::Scene::Scene::_tick(const std::chrono::high_resolution_clock::duration & delta) {
   tick(delta);
}

void Vt::Scene::Scene::_beginPlay() {
   SYSTEM_LOG_INFO("Scene::_beginPlay: %s", mName.c_str());
   beginPlay();
   //std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
   if (!mActive) {
      mSceneManager.mActiveScenes.push_back(this);
      mActive = true;
   }
}

void Vt::Scene::Scene::_endPlay() {
   SYSTEM_LOG_INFO("Scene::_endPlay: %s", mName.c_str());
   endPlay();
   //std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
   if (mActive) {
      auto s = std::find(mSceneManager.mActiveScenes.begin(), mSceneManager.mActiveScenes.end(), this);
      mSceneManager.mActiveScenes.erase(s);
      mActive = false;
   }
}

void Vt::Scene::Scene::_beginDraw() {
   SYSTEM_LOG_INFO("Scene::_beginDraw: %s", mName.c_str());
   beginDraw();
   //std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
   if (!mVisible) {
      mSceneManager.mVisibleScenes.push_back(this);
      mVisible = true;
   }
}

void Vt::Scene::Scene::_endDraw() {
   SYSTEM_LOG_INFO("Scene::_endDraw: %s", mName.c_str());
   endDraw();
   //std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
   if (mVisible) {
      auto s = std::find(mSceneManager.mVisibleScenes.begin(), mSceneManager.mVisibleScenes.end(), this);
      mSceneManager.mVisibleScenes.erase(s);
      mVisible = false;
   }
}

