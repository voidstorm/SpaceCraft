#pragma once

//#ifndef _WIN32_WINNT 
//#define _WIN32_WINNT 0x0500
//#endif
#include <windows.h>
#include <map>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>


namespace Vt {
namespace App {
class AppWindow;
namespace Private {
class OSAppWindow final {
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
   ~OSAppWindow(void);
   HWND handle();
   HWND winId(); //for Qt compatibility
   HINSTANCE instance(); //for Qt compatibility
   static int run();
   static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static void registerWndClass(HINSTANCE hinstance, int color);

   int width();
   int height();
   void show();
   void hide();
   void setSize(int width, int height);
   void startTimer(int ms);
   void stopTimer();
   void close();
   void lock() const;
   void unlock() const;

   //events
   void onCreate();
   void onTimer();
   bool onClose();
   void onPaint();
   void onMove(int x, int y);
   void onRestore();
   void onMinimize();
   void onMaximize();
   void onShow();
   void onHide();
   void onResize(int width, int height);
   void onResized(int width, int height);
   void onKeydown(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   void onKeyup(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   void onCharKey(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl);
   void onMouseButtonDown(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   void onMouseButtonUp(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   void onMouseDblClick(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   void onMouseMove(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   void onMouseWheel(int clicks, unsigned button, unsigned shift, unsigned alt, unsigned ctrl);
   static bool isKeyDown(uint64_t key);
   bool getCursorPos(int& x, int& y);
   void setCursorPos(int x, int y);
   RECT getWindowRect();
private:
   void release();
   void timer(void* param);
   LRESULT processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
   static std::recursive_mutex windowLock_;
};
}
}
}