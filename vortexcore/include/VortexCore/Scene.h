#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <string>
#include <memory>
#include <chrono>
#include <future>
#include <atomic>

//Has a scene graph and scene objects

namespace Vt {
namespace Scene {
class VORTEX_API Scene {
   friend class Game;
public:
   Scene(SceneManager& sceneManager);
   virtual ~Scene();
   virtual const std::string & name() const;
   virtual Vt::Scene::TransformCache & transformCache();
   virtual void load();
   virtual void unload();
   virtual std::future<void> loadAsync();
   virtual std::future<void> unloadAsync();
   virtual float loadProgress();
   virtual void activate();
   virtual bool active() const;
   virtual void deactivate();
   virtual void show();
   virtual void hide();
   virtual bool visible();
   SceneManager& sceneManager() const;
   Vt::Gfx::RenderContext& renderContext() const;

protected:
   //called after level load
   virtual void beginPlay();
   //called before level unload
   virtual void endPlay();
   //called before drawing the scene
   virtual void beginDraw();
   //draws the scene
   virtual void draw(const std::chrono::high_resolution_clock::duration &delta);
   //called when drawing is finished
   virtual void endDraw();
   //logic/world tick
   virtual void tick(const std::chrono::high_resolution_clock::duration &delta);

private:
   void _load();
   void _unload();

   std::unique_ptr<TransformCache> mTransformCache;
   std::string mName;
   SceneManager &mSceneManager;
   Vt::Gfx::RenderContext &mRenderContext;
   bool mActive{ false };
   bool mVisible{ false };
};
}
}

