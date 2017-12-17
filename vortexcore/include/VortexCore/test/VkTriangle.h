#pragma once
#include "VortexCore/Api.h"
#include "VortexCore/ForwardDecls.h"
#include "VortexCore/Scene.h"
#include <string>
#include <memory>
#include <chrono>
#include <future>
#include <atomic>

//Test scene for vulkan tests

namespace Vt {
namespace Test {
class VORTEX_API VkTriangle : public Vt::Scene::Scene{
public:
   VkTriangle(Vt::Scene::SceneManager& sceneManager);
   virtual ~VkTriangle();

   virtual void load() override;
   virtual void unload() override;

protected:
   //called after level load
   virtual void beginPlay() override;
   //called before level unload
   virtual void endPlay() override;
   //called before drawing the scene
   virtual void beginDraw() override;
   //draws the scene
   virtual void draw(const std::chrono::high_resolution_clock::duration &delta) override;
   //called when drawing is finished
   virtual void endDraw() override;
   //logic/world tick
   virtual void tick(const std::chrono::high_resolution_clock::duration &delta) override;

};
}
}

