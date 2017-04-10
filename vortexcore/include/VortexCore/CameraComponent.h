#pragma once
#include "Component.h"

namespace Vt {
	namespace Scene {
		class CameraComponent : public Component {
		public:
			CameraComponent(Vt::Scene::SceneObject& parent, const std::string& name = "CameraComponent");
			virtual ~CameraComponent();
		};
	}
}

