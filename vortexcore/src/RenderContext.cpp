#include "..\include\VortexCore\RenderContext.h"
#include "..\include\VortexCore\private\RenderContextVulkan.h"



Vt::Gfx::RenderContext::RenderContext(const std::shared_ptr<Vt::App::AppWindow> &window,const RenderContextLayout &layout):
	 mWindow(window)
  ,mLayout(layout)
  {
}


Vt::Gfx::RenderContext::~RenderContext() {
}

bool Vt::Gfx::RenderContext::swapBuffers() {
	return false;
}

bool Vt::Gfx::RenderContext::toggleFullscreen() {
	return false;
}

std::weak_ptr<Vt::App::AppWindow> Vt::Gfx::RenderContext::window() const {
	return mWindow;
}

const Vt::Gfx::RenderContextLayout & Vt::Gfx::RenderContext::layout() const {
  return mLayout;
}

bool Vt::Gfx::RenderContext::createSwapChain() {
	return false;
}

bool Vt::Gfx::RenderContext::restoreSwapChain() {
	return false;
}

void Vt::Gfx::RenderContext::resizeSwapChain(unsigned width,unsigned height) {
}
