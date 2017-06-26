#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <memory>

namespace Vt {
namespace Gfx {

struct ScreenLayout {
   int width;
   int height;
   int monitorIdx;
   bool fullscreen;
   int depth;
   int stencil;
   int multisample;
};


class VORTEX_API Screen {
public:
   Screen();
   ~Screen();

   bool swapBuffers();
   bool toggleFullscreen();
   std::weak_ptr<Vt::App::AppWindow> window() const;
   const ScreenLayout& layout() const;

private:
   bool createSwapChain();
   bool restoreSwapChain();
   void resizeSwapChain(unsigned width, unsigned height);

private:
   ScreenLayout mLayout;

};
}
}