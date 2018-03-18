#pragma once
#include "Component.h"
#include <unordered_map>
#include <memory>
#include <exception>
#include <type_traits>
#include <vector>

namespace Vt {
namespace Scene {
class VORTEX_API ComponentContainer {
public:
   template<class T, class... Args>
   void createComponent(Args&&... args) {
      mComponents.emplace(typeid(T).hash_code(), std::make_shared<T>(std::forward<Args>(args)...));
   }

   template<class T>
   void addSharedComponent(const std::shared_ptr<typename T> &component) {
      mComponents.emplace(typeid(T).hash_code(), std::dynamic_pointer_cast<Vt::Scene::Component>(component));
   }

   template<class T>
   T * getComponent() {
      auto item = mComponents.find(typeid(T).hash_code());
      if (item != mComponents.end()) {
         return static_cast<std::add_pointer<T>::type>(item->second.get());
      }
      return nullptr;
   }

   template<class T>
   std::shared_ptr<typename T> getSharedComponent() {
      auto item = mComponents.find(typeid(T).hash_code());
      if (item != mComponents.end()) {
         return std::dynamic_pointer_cast<typename T>(item->second);
      }
      return nullptr;
   }

   //range based for-loop support
   std::unordered_map<size_t, std::shared_ptr<Vt::Scene::Component>>::iterator begin() {
      return mComponents.begin();
   }

   std::unordered_map<size_t, std::shared_ptr<Vt::Scene::Component>>::iterator end() {
      return mComponents.end();
   }

private:
   std::unordered_map<size_t, std::shared_ptr<Vt::Scene::Component>> mComponents;
};
}
}

__forceinline std::unordered_map<size_t, std::shared_ptr<Vt::Scene::Component>>::iterator begin(Vt::Scene::ComponentContainer &components) {
   return components.begin();
}

__forceinline std::unordered_map<size_t, std::shared_ptr<Vt::Scene::Component>>::iterator end(Vt::Scene::ComponentContainer &components) {
   return components.end();
}

