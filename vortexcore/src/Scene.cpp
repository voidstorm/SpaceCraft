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
   return std::async(std::launch::async, [this]()->void {
      _load();
   });
}

//--------------------------------------------------------------------------
//
std::future<void> Vt::Scene::Scene::unloadAsync() {
   return std::async(std::launch::async, [this]()->void {
      _unload();
   });
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::activate() {
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onActivate();
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
      _onDeactivate();
      return 0;
   });
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::show() {
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onShow();
      return 0;
   });

}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::hide() {
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onHide();
      return 0;
   });
}

//--------------------------------------------------------------------------
//
bool Vt::Scene::Scene::visible() {
   return mVisible;
}

bool Vt::Scene::Scene::loaded() {
   return mLoaded.load(std::memory_order_acquire);;
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

void Vt::Scene::Scene::onLoaded() {
}

void Vt::Scene::Scene::onUnload() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::onActivate() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::onDeactivate() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::onShow() {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::draw(const std::chrono::high_resolution_clock::duration & delta) {
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::onHide() {
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
   mLoaded.store(true, std::memory_order_release);
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onLoaded();
      return 0;
   });
}

//--------------------------------------------------------------------------
//
void Vt::Scene::Scene::_unload() {
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onDeactivate();
      return 0;
   }).get();
   mSceneManager.game().renderThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onHide();
      return 0;
   }).get();
   mSceneManager.game().gameThread().GetCommandQueue().Submit([&, this](void*)->Vt::CommandQueue::CMD_RET_TYPE {
      _onUnload();
      return 0;
   }).get();
   unload();
   mLoaded.store(false, std::memory_order::memory_order_release);
   SYSTEM_LOG_INFO("Scene::_unload: %s", mName.c_str());
}

void Vt::Scene::Scene::_draw(const std::chrono::high_resolution_clock::duration & delta) {
   draw(delta);
}

void Vt::Scene::Scene::_tick(const std::chrono::high_resolution_clock::duration & delta) {
   tick(delta);
}

void Vt::Scene::Scene::_onLoaded() {
   onLoaded();
}

void Vt::Scene::Scene::_onUnload() {
   onUnload();
}

void Vt::Scene::Scene::_onActivate() {
   SYSTEM_LOG_INFO("Scene::_onActivate: %s", mName.c_str());
   onActivate();
   //std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
   if (!mActive) {
      mSceneManager.mActiveScenes.push_back(this);
      mActive = true;
   }
}

void Vt::Scene::Scene::_onDeactivate() {
   SYSTEM_LOG_INFO("Scene::_onDeactivate: %s", mName.c_str());
   onDeactivate();
   //std::lock_guard<decltype(mSceneManager.m_act_lock)> l(mSceneManager.m_act_lock);
   if (mActive) {
      auto s = std::find(mSceneManager.mActiveScenes.begin(), mSceneManager.mActiveScenes.end(), this);
      mSceneManager.mActiveScenes.erase(s);
      mActive = false;
   }
}

void Vt::Scene::Scene::_onShow() {
   SYSTEM_LOG_INFO("Scene::_onShow: %s", mName.c_str());
   onShow();
   //std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
   if (!mVisible) {
      mSceneManager.mVisibleScenes.push_back(this);
      mVisible = true;
   }
}

void Vt::Scene::Scene::_onHide() {
   SYSTEM_LOG_INFO("Scene::_onHide: %s", mName.c_str());
   onHide();
   //std::lock_guard<decltype(mSceneManager.m_vis_lock)> l(mSceneManager.m_vis_lock);
   if (mVisible) {
      auto s = std::find(mSceneManager.mVisibleScenes.begin(), mSceneManager.mVisibleScenes.end(), this);
      mSceneManager.mVisibleScenes.erase(s);
      mVisible = false;
   }
}

