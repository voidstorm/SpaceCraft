#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <memory>
#include <exception>

//This is the normal rendercontext. Typically there exists one context per window.
//There might be different implementations of the context - e.g. opengl, vuilkan...
//The render contextmanages resources such as swapchains and all window/platform related stuff...

namespace Vt {
namespace Gfx {

class RenderContextException : public std::runtime_error {
public:
   explicit RenderContextException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit RenderContextException(const char* what_arg) :std::runtime_error(what_arg) {};
};


class RenderContextVulkan;



class VORTEX_API RenderContext {
public:
   RenderContext(/*const std::shared_ptr<Vt::App::AppWindow> &window, const RenderContextLayout &layout*/);
   ~RenderContext();
   void init();


private:
   std::unique_ptr<RenderContextVulkan> mVkContext;
   std::shared_ptr<Vt::App::AppWindow> mWindow;
};
}
}

