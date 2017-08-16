#pragma once
#include <memory>
#include <string>
#include "Api.h"
#include "Delegate.h"
#include "ForwardDecls.h"
//TODO: check closing behaviour -> that is see if window delets when close is pressed



namespace Vt {
namespace App {
using ::Vt::Delegate;
class  AppWindow : public virtual std::enable_shared_from_this<AppWindow> {
public:
   typedef int width_t;
   typedef int pos_t;
   typedef uint64_t key_t;
   typedef uint64_t altstate_t;
   typedef uint64_t ctrlstate_t;
   typedef uint64_t shiftstate_t;
   typedef uint64_t mbutton_t;
   typedef int wheeldelta_t;

   enum class Style : unsigned {
      ws_overlapped = 0x00000000l,
      ws_popup = 0x80000000l,
      ws_child = 0x40000000l,
      ws_minimize = 0x20000000l,
      ws_visible = 0x10000000l,
      ws_disabled = 0x08000000l,
      ws_clipsiblings = 0x04000000l,
      ws_clipchildren = 0x02000000l,
      ws_maximize = 0x01000000l,
      ws_caption = 0x00c00000l,
      ws_border = 0x00800000l,
      ws_dlgframe = 0x00400000l,
      ws_vscroll = 0x00200000l,
      ws_hscroll = 0x00100000l,
      ws_sysmenu = 0x00080000l,
      ws_thickframe = 0x00040000l,
      ws_group = 0x00020000l,
      ws_tabstop = 0x00010000l,
      ws_minimizebox = 0x00020000l,
      ws_maximizebox = 0x00010000l,
      ws_tiled = ws_overlapped,
      ws_iconic = ws_minimize,
      ws_sizebox = ws_thickframe,
      ws_overlappedwindow = ws_overlapped |
      ws_caption |
      ws_sysmenu |
      ws_thickframe |
      ws_minimizebox |
      ws_maximizebox,
      ws_popupwindow = ws_popup |
      ws_border |
      ws_sysmenu,
      ws_childwindow = ws_child
   };
   virtual ~AppWindow();
protected:
   AppWindow(std::wstring name, int width = 640, int height = 480, Style style = Style::ws_overlappedwindow, bool visible = true);
public:
#ifdef _WIN32
   template<typename T = HWND> T winId();
   template<typename T = HWND> T winId() const;

   template<typename T = HINSTANCE> T instance();
   template<typename T = HINSTANCE> T instance() const;
#else
   template<typename T> T winId();
#endif
   virtual int width();
   virtual int height();
   virtual void show();
   virtual void hide();
   virtual void setSize(int width, int height);
   virtual void startTimer(int ms);
   virtual void stopTimer();
   virtual void close();
   virtual void lock() const;
   virtual void unlock() const;
   static VORTEX_API int exec();
   static VORTEX_API std::weak_ptr <AppWindow> create(std::wstring name, int width = 640, int height = 480, Style style = Style::ws_overlappedwindow, bool visible = true);

   //events
public:
   Delegate<void> OnCreate;
   Delegate<void> OnTimer;
   //if one of the attached delegates returns false, close will be aborted
   Delegate<bool> OnClose;
   Delegate<void> OnPaint;
   //x, y
   Delegate<void, pos_t, pos_t> OnMove;
   Delegate<void> OnRestore;
   Delegate<void> OnMinimize;
   Delegate<void> OnMaximize;
   Delegate<void> OnShow;
   Delegate<void> OnHide;
   //width, height
   Delegate<void, width_t, width_t> OnResize;
   Delegate<void, width_t, width_t> OnResized;
   // key, shift, alt, ctrl
   Delegate<void, key_t, shiftstate_t, altstate_t, ctrlstate_t> OnKeydown;
   Delegate<void, key_t, shiftstate_t, altstate_t, ctrlstate_t> OnKeyup;
   Delegate<void, key_t, shiftstate_t, altstate_t, ctrlstate_t> OnCharKey;
   //int x, int y, unsigned buttOn, unsigned shift, unsigned alt, unsigned ctrl
   Delegate<void, pos_t, pos_t, mbutton_t, shiftstate_t, altstate_t, ctrlstate_t> OnMouseButtonDown;
   Delegate<void, pos_t, pos_t, mbutton_t, shiftstate_t, altstate_t, ctrlstate_t> OnMouseButtonUp;
   Delegate<void, pos_t, pos_t, mbutton_t, shiftstate_t, altstate_t, ctrlstate_t> OnMouseDblClick;
   Delegate<void, pos_t, pos_t, mbutton_t, shiftstate_t, altstate_t, ctrlstate_t> OnMouseMove;
   Delegate<void, wheeldelta_t, mbutton_t, shiftstate_t, altstate_t, ctrlstate_t> OnMouseWheel;
private:
   friend class Vt::App::Private::OSAppWindow;
   void deleter();
   void registerDeleter();
   std::shared_ptr <AppWindow> mSelf;
   std::unique_ptr<Vt::App::Private::OSAppWindow> mImpl;
};
}
}

