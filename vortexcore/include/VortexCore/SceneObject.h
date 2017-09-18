#pragma once
#include "Api.h"
#include "Transform.h"
#include "Component.h"
#include "Delegate.h"
#include <memory>
#include <vector>
#include <type_traits>
#include <chrono>
#include "ForwardDecls.h"

namespace Vt {
  namespace Scene {
    //Scene objects can tick
    class VORTEX_API SceneObject : public std::enable_shared_from_this<SceneObject> {
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

      //add a component to the object. Note that there can be only one component per type.
      bool addComponent(std::unique_ptr<Component> &&component);

      //returns a component by name
      std::weak_ptr<Component> getComponentByName(const std::string &name);

      //returns a component by type
      template<typename T>
      std::weak_ptr<T> getComponentByType() {
        for ( const auto &i : mComponents ) {
          auto target = dynamic_cast<std::add_pointer<T>::type>( i.get() );
          if ( target == nullptr ) {
            continue;
          } else {
            return std::dynamic_pointer_cast<T>(i);
          }
        }
        throw std::exception("SceneObject::getComponentByType: invalid type");
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
      void _beginPlay();
      void _endPlay();
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
      std::vector<std::shared_ptr<Component>> mComponents;
    };
  }
}


