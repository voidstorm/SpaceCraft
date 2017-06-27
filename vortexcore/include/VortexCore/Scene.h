#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <string>
#include <memory>

//Has a scene graph and scene objects

namespace Vt {
  namespace Scene {
    class VORTEX_API Scene {
    public:
      Scene();
      virtual ~Scene();
      virtual const std::string & name() const;
      virtual Vt::Scene::TransformCache & transformCache();
    private:
      std::unique_ptr<TransformCache> mTransformCache;
      std::string mName;
    };
  }
}

