#include "..\include\VortexCore\TransformCache.h"



Vt::Scene::TransformCache::TransformCache(const int init_size) {
  mFreelist.reserve(init_size);
  mTransforms.reserve(init_size);
}


Vt::Scene::TransformCache::~TransformCache() {
}


//we keep a free list. When returing a transform it goes to the free list.
//additionally we need to keep information of segmentation, in order to be
//able to update the uniform buffers correctly.

//We could also keep track of all the transforms that actually changed... to keep
//buffer updates as low as possible.

Vt::Scene::Transform & Vt::Scene::TransformCache::requestTransform() {
  if ( !mFreelist.empty() ) {
    auto tranform = mFreelist.back();
    mFreelist.pop_back();
    return mTransforms[tranform];
  }

  //if we dont find a transform, generate a new one
  mTransforms.emplace_back();
  return mTransforms[mCurrentTransform++];
}

//we could optimize by calculating the index, which would just be something like
//(transform_address - startaddress) / (aligned)elementsize
void Vt::Scene::TransformCache::revokeTransform(const Transform & transform) {
  for ( int i = 0; i < mFreelist.size(); ++i ) {
    if ( &( mTransforms[i] ) == &transform ) {
      mFreelist.push_back(i);
    }
  }
}

const Vt::Scene::Transform * Vt::Scene::TransformCache::data() const {
  return mTransforms.data();
}
