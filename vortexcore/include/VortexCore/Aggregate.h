#pragma once
#include "Interface.h"
#include <unordered_map>
#include <memory>
#include <exception>
#include <type_traits>
#include <vector>

namespace Vt {
class VORTEX_API Aggregate {
public:
   template<class T, class... Args>
   void createInterface(Args&&... args) {
      mInterfaces.emplace(typeid(T).hash_code(), std::make_shared<T>(std::forward<Args>(args)...));
   }

   template<class T>
   void addSharedInterface(const std::shared_ptr<typename T> &interface) {
      mInterfaces.emplace(typeid(T).hash_code(), std::dynamic_pointer_cast<Vt::Interface>(interface));
   }

   template<class T>
   T * getInterface() {
      auto item = mInterfaces.find(typeid(T).hash_code());
      if (item != mInterfaces.end()) {
         return static_cast<std::add_pointer<T>::type>(item->second.get());
      }
      return nullptr;
   }

   template<class T>
   std::shared_ptr<typename T> getSharedInterface() {
      auto item = mInterfaces.find(typeid(T).hash_code());
      if (item != mInterfaces.end()) {
         return std::dynamic_pointer_cast<typename T>(item->second);
      }
      return nullptr;
   }

   //range based for-loop support
   std::unordered_map<size_t, std::shared_ptr<Vt::Interface>>::iterator begin() {
      return mInterfaces.begin();
   }

   std::unordered_map<size_t, std::shared_ptr<Vt::Interface>>::iterator end() {
      return mInterfaces.end();
   }

private:
   std::unordered_map<size_t, std::shared_ptr<Vt::Interface>> mInterfaces;
};
}
