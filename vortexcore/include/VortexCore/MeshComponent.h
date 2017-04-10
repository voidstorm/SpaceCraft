#pragma once
#include "Component.h"

namespace Vt {
  namespace Scene {
    class VORTEX_API MeshComponent : public Component {
    public:
      MeshComponent(Vt::Scene::SceneObject& parent, const std::string& name = "MeshComponent");
      virtual ~MeshComponent();
    };
  }
}

