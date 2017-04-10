#pragma once
#include "Transform.h"
#include "ForwardDecls.h"

#include <vector>

namespace Vt {
  namespace Scene {
    class TransformCache {
    public:
      TransformCache(const int init_size);
      virtual ~TransformCache();
      virtual Transform & requestTransform();
      virtual void revokeTransform(const Transform & transform);
      virtual const Transform* data() const;
    private:
      unsigned mCurrentTransform;
      std::vector<Transform> mTransforms;
      std::vector<unsigned> mFreelist;
    };
  }
}
