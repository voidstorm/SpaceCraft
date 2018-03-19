#pragma once
#include "Api.h"
#include "Transform.h"
#include "Component.h"
#include "Delegate.h"
#include <memory>
#include <vector>
#include <type_traits>
#include <chrono>
#include "Aggregate.h"
#include "ComponentContainer.h"
#include "ForwardDecls.h"

namespace Vt {
  namespace Scene {
    //Scene objects can tick
    class VORTEX_API SceneObject : public Vt::Scene::ComponentContainer, public std::enable_shared_from_this<SceneObject> {
    public:
      SceneObject(Scene &scene, const bool is_static= false, const std::string& name = "SceneObject");
      virtual ~SceneObject();

      //gets the transform
      inline Transform& transform() {
        return mTransform;
      }

      //gets the transform
      inline const Transform& transform() const {
        return mTransform;
      }

      //sets a new transform
      inline void setTransform(const Transform& transform) {
        mTransform = transform;
      }

      virtual void activate();
      virtual void deactivate();
      const std::string& name() const;
      bool canTick() const;
      bool active() const;
      bool visible() const;
      void show();
      void hide();
      bool isStatic() const;
      Vt::Scene::Scene & scene();


      //Delegates
      //delegates always have a sender argument
      Vt::Delegate<void, SceneObject&> OnActivate;
      Vt::Delegate<void, SceneObject&> OnDeactivate;
      Vt::Delegate<void, SceneObject&> OnBeginPlay;
      Vt::Delegate<void, SceneObject&> OnEndPlay;
      Vt::Delegate<void, SceneObject&> OnShow;
      Vt::Delegate<void, SceneObject&> OnHide;
      Vt::Delegate<void, SceneObject&, const std::chrono::high_resolution_clock::duration&> OnTick;

    protected:
      //Overridable callbacks
      virtual void onActivate();
      virtual void onDeactivate();
      virtual void onShow();
      virtual void onHide();

      virtual void onBeginPlay();
      virtual void onEndPlay();
      virtual void onTick(const std::chrono::high_resolution_clock::duration &delta);

    private:
      void _onActivate();
      void _onDeactivate();
      void _tick(const std::chrono::high_resolution_clock::duration &delta);

    protected:
      bool mActive = true;
      bool mStatic = false;
      bool mVisible = true;
      bool mCanTick = false;
      Vt::Scene::Scene &mScene;
      const std::string mName;

    private:
      Transform &mTransform;
      std::weak_ptr<SceneObject> mParent;
      std::vector<std::shared_ptr<SceneObject>> mChildren;
    };
  }
}


