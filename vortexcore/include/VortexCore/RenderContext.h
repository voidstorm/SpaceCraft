#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <memory>

//This is the normal rendercontext. Typically there exists one context per window.
//There might be different implementations of the context - e.g. opengl, vuilkan...
//The render contextmanages resources such as swapchains and all window/platform related stuff...

namespace Vt {
  namespace Gfx {
    class RenderContextVulkan;

    struct RenderContextLayout {
      int width;
      int height;
      int screen;
      bool fullscreen;
      int depth;
      int stencil;
      int multisample;
    };

    class VORTEX_API RenderContext {
    public:
      RenderContext(const std::shared_ptr<Vt::App::AppWindow> &window, const RenderContextLayout &layout);
      virtual ~RenderContext();
      virtual bool swapBuffers();
      virtual bool toggleFullscreen();
      virtual std::weak_ptr<Vt::App::AppWindow> window() const;
      virtual const RenderContextLayout& layout() const;
    protected:
      virtual bool createSwapChain();
      virtual bool restoreSwapChain();
      virtual void resizeSwapChain(unsigned width, unsigned height);

    private:
      std::unique_ptr<RenderContextVulkan> mVkContext;
      std::shared_ptr<Vt::App::AppWindow> mWindow;
      RenderContextLayout mLayout;
    };
  }
}

