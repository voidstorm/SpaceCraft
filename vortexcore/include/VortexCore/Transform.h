#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#pragma warning(disable: 4838)
#include "Api.h"


namespace Vt {
  namespace Scene {
    class Transform final {
    public:
      Transform() {
      }

      Transform(const glm::vec4 &position, const glm::quat &rotation = glm::quat(), const glm::vec4 &scale = glm::vec4())
        : mPostion(position)
        , mScale(scale) 
        , mRotation(rotation){
      }

      ~Transform() {
      }



      inline void setPosition(const glm::vec4& position) {
        mPostion = position;
      }

      inline void setRotation(const glm::quat& rotation) {
        mRotation = rotation;
      }

      inline void setScale(const glm::vec4& scale) {
        mScale = scale;
      }

      inline const glm::vec4& position() const {
        return mPostion;
      }

      inline const glm::quat& rotation() const {
        return mRotation;
      }

      inline const glm::vec4& scale() const {
        return mScale;
      }

    private:
      glm::vec4 mPostion;
      glm::vec4 mScale;
      glm::quat mRotation;
    };
  }
}

