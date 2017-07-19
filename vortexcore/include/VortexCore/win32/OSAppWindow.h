#pragma once

//#ifndef _WIN32_WINNT 
//#define _WIN32_WINNT 0x0500
//#endif
#include <windows.h>
#include <map>
#include <thread>
#include <atomic>
#include <memory>
#include <string>


namespace Vt {
namespace App {
class AppWindow;
namespace Private {
class OSAppWindow {
public:
   enum MouseButton {
      null_ = 0,
      left_ = VK_LBUTTON,
      right_ = VK_RBUTTON,
      middle_ = VK_MBUTTON,
      wheelDelta_ = 120
   };
public:
   OSAppWindow(Vt::App::AppWindow &backptr, std::wstring name = L"evolution engine", int width = 640, int height = 480, unsigned style = WS_OVERLAPPEDWINDOW, bool visible = false);
   virtual ~OSAppWindow(void);
   virtual HWND handle();
   virtual HWND winId(); //for Qt compatibility
   virtual HINSTANCE instance(); //for Qt compatibility
   static int run();
   static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static void registerWndClass(HINSTANCE hinstance, int color);

   virtual int width();
   virtual int height();
   virtual void show();
   virtual void hide();
   virtual void setSize(int width, int height);
   virtual void startTimer(int ms);
   virtual void stopTimer();
   virtual void close();
   //events
   virtual void onCreate();
   virtual void onTimer();
   virtual bool onClose();
   virtual void onPaint();
   virtual void onMove(int x, int y);
   virtual void onRestore();
   virtual void onMinimize();
   virtual void onMaximize();
   virtual void onShow();
   virtual void onHide();
   virtual void onResize(int width, int height);
   virtual void onResized(int width, int height);
   virtual void onKeydown(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   virtual void onKeyup(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   virtual void onCharKey(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   virtual void onMouseButtonDown(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   virtual void onMouseButtonUp(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   virtual void onMouseDblClick(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   virtual void onMouseMove(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   virtual void onMouseWheel(int clicks, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   static bool isKeyDown(uint64_t key);
   virtual bool getCursorPos(int& x, int& y);
   virtual void setCursorPos(int x, int y);
   virtual RECT getWindowRect();
protected:
   void release();
   virtual void timer(void* param);
   virtual LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
   static std::atomic<int> refCount_;
   static std::map<HWND, OSAppWindow*> windows_;
   typedef std::map<HWND, OSAppWindow*>::iterator WindowIter_;
   HINSTANCE	hinstance_;
   HWND		hwnd_;
   int			width_;
   int height_;
   int style_;
   int timerInterval_;
   Vt::App::AppWindow &mBackPtr;
   std::atomic<bool> runTimer_;
   std::unique_ptr<std::thread> timerThread_;
   std::atomic<bool> close_;
   std::wstring name_;
};
}
}
}