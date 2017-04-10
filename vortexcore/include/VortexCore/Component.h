#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <chrono>
#include <string>
#include <memory>
#include "Delegate.h"

//Components are attached to scene objects and cannot exist by themselves.

namespace Vt {
  namespace Scene {
    //Scene components can also tick

    class VORTEX_API Component : public std::enable_shared_from_this<Component>{
    public:
      Component(Vt::Scene::SceneObject& parent, const std::string& name="Component");
      virtual ~Component();
      const std::string& name() const;
      bool canTick() const;
      bool active() const;
      Vt::Scene::SceneObject& parent() const;

      virtual void activate();
      virtual void deactivate();
      
      //delegates always have a sender argument
      Vt::Delegate<void, std::shared_ptr<Component>&> OnActivate;
      Vt::Delegate<void, std::shared_ptr<Component>&> OnDeactivate;
      Vt::Delegate<void, std::shared_ptr<Component>&> OnBeginPlay;
      Vt::Delegate<void, std::shared_ptr<Component>&> OnEndPlay;
      Vt::Delegate<void, std::shared_ptr<Component>&, const std::chrono::high_resolution_clock::duration&> OnTick;

    protected:
      virtual void onActivate();
      virtual void onDeactivate();

      virtual void onBeginPlay(Vt::Scene::SceneObject& sender);
      virtual void onEndPlay(Vt::Scene::SceneObject& sender);
      virtual void onTick(Vt::Scene::SceneObject& sender, const std::chrono::high_resolution_clock::duration &delta);

    protected:
      Vt::Scene::SceneObject& mParent;
      bool mActive = true;
      bool mCanTick = true;
      const std::string mName;
      friend class SceneObject;
    };
  }
}

