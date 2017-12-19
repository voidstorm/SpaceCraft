#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include "Aggregate.h"
#include <string>
#include <memory>
#include <chrono>
#include <future>
#include <atomic>

//Has a scene graph and scene objects

namespace Vt {
namespace Scene {
class VORTEX_API Scene : public Vt::Aggregate {
   friend class Game;
   friend class SceneManager;
public:
   Scene(const std::string &name, SceneManager& sceneManager);
   virtual ~Scene();
   virtual const std::string & name() const;
   virtual Vt::Scene::TransformCache & transformCache();
   virtual std::future<void> loadAsync();
   virtual std::future<void> unloadAsync();
   virtual void activate();
   virtual bool active() const;
   virtual void deactivate();
   virtual void show();
   virtual void hide();
   virtual bool visible();
   virtual bool loaded();
   SceneManager& sceneManager() const;
   Vt::Gfx::RenderContext& renderContext() const;

protected:
   virtual void load();
   virtual void unload();
   //called after level load
   virtual void onLoaded();
   //called before level unload
   virtual void onUnload();
   //called after level load
   virtual void onActivate();
   //called before level unload
   virtual void onDeactivate();
   //called before drawing the scene
   virtual void onShow();
   //draws the scene
   virtual void draw(const std::chrono::high_resolution_clock::duration &delta);
   //called when drawing is finished
   virtual void onHide();
   //logic/world tick
   virtual void tick(const std::chrono::high_resolution_clock::duration &delta);

private:
   void _load();
   void _unload();
   void _draw(const std::chrono::high_resolution_clock::duration &delta);
   void _tick(const std::chrono::high_resolution_clock::duration &delta);

   void _onLoaded();
   void _onUnload();
   void _onActivate();
   void _onDeactivate();
   void _onShow();
   void _onHide();

   std::unique_ptr<TransformCache> mTransformCache;
   std::string mName;
   SceneManager &mSceneManager;
   Vt::Gfx::RenderContext &mRenderContext;
   bool mActive{ false };
   bool mVisible{ false };
   std::atomic_bool mLoaded{ false };
};
}
}

