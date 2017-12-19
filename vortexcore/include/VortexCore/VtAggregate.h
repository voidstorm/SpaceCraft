#pragma once
#include "VtInterface.h"
#include <unordered_map>
#include <memory>
#include <exception>

namespace Vt{
    class Aggregate
    {
        template<class T, class... Args>
        void AddInterface(Args&&... args)
        {
            m_interfaces.emplace(typeid(T).hash_code(), std::make_unique<T>(std::forward<Args>(args)...));
        }

        template<class T>
        T * GetInterface()
        {
            auto item = m_interfaces.find(typeid(T).hash_code());
            if (item != m_interfaces.end())
            {
                return static_cast<std::add_pointer<T>::type>(item->second.get());
            }
            throw std::runtime_error("CoffeeMayaBridge::GetInterface, failed to get interface!");
        }

    private:
        std::unordered_map<size_t, std::unique_ptr<Vt::Interface> mInterfaces;
    };
}
