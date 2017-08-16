#include "../../include/VortexCore/AppWindow.h"
#include "../../include/VortexCore/win32/OSAppWindow.h"
#include <iostream>
#include <chrono>
#include <Windows.h>

#define WM_CREATED WM_USER+1

namespace Vt {
namespace App {
namespace Private {

std::atomic<int> OSAppWindow::refCount_ = { 0 };
std::map<HWND, OSAppWindow*> OSAppWindow::windows_ = std::map<HWND, OSAppWindow*>();
std::recursive_mutex OSAppWindow::windowLock_;

//-------------------------------------------------------------------------
OSAppWindow::OSAppWindow(Vt::App::AppWindow &backptr, std::wstring name, int width, int height, unsigned style, bool visible) : width_(width), height_(height), style_(style),
mBackPtr(backptr),
hinstance_(0),
timerInterval_(1),
runTimer_(false),
close_(false),
name_(name) {
   if (!refCount_)
      this->registerWndClass(0, 0);
   RECT rc = { 0, 0, width, height };
   AdjustWindowRect(&rc, style, FALSE);
   hwnd_ = CreateWindow(L"VortexCoreWindowClass", name.c_str(), style,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hinstance_, NULL);
   if (visible)
      ShowWindow(hwnd_, TRUE);
   else
      ShowWindow(hwnd_, FALSE);

   UpdateWindow(hwnd_);
   windows_[hwnd_] = this;
   refCount_++;
   PostMessage((HWND)hwnd_, WM_CREATED, 0, 0);
}

//-------------------------------------------------------------------------
OSAppWindow::~OSAppWindow(void) {
   this->stopTimer();
   WindowIter_ i = windows_.find(hwnd_);
   if (i != windows_.end())
      windows_.erase(i);
   if (refCount_ > 0)
      refCount_--;
   if (refCount_ == 0) {
      UnregisterClass(L"VortexCoreWindowClass", 0);
   }
}

//-------------------------------------------------------------------------
void OSAppWindow::timer(void* param) {
   while (runTimer_) {
      PostMessage((HWND)param, WM_TIMER, (WPARAM)param, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(timerInterval_));
   }
}

LRESULT OSAppWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
      case WM_TIMER:
         if (!close_)
            onTimer();
         break;
      case WM_PAINT:
          if (!close_)
            BeginPaint(hWnd, NULL);
            onPaint();
            EndPaint(hWnd, NULL);
         return TRUE;
         break;
      case WM_ERASEBKGND:
         return TRUE;
      case WM_CREATED:
      {
         onCreate();
      }
      break;
      case WM_CLOSE:
      {
         if (onClose()) {
            DestroyWindow(hWnd);
            return 0;
         } else {
            return 1;
         }
      }
      break;
      case WM_DESTROY:
         stopTimer();
         mBackPtr.deleter();
         if (windows_.empty())
            PostQuitMessage(0);
         break;
      case WM_EXITSIZEMOVE:
         width_ = VT_LOWORD(lParam);
         height_ = VT_HIWORD(lParam);
         onResized(VT_LOWORD(lParam), VT_HIWORD(lParam));
         break;
      case WM_SIZE:
         width_ = VT_LOWORD(lParam);
         height_ = VT_HIWORD(lParam);
         onResize(VT_LOWORD(lParam), VT_HIWORD(lParam));
         //call additional functions depending on the action
         if (wParam == SIZE_MINIMIZED) {
            onMinimize();
         } else if (wParam == SIZE_MAXIMIZED) {
            onMaximize();
         } else if (wParam == SIZE_RESTORED) {
            onRestore();
         }
         break;
         //mouse and kb
      case WM_KEYDOWN:
      {
         onKeydown(wParam, 0, 0, 0);
         break;
      }
      case WM_KEYUP:
      {
         onKeyup(wParam, 0, 0, 0);
         break;
      }
      case WM_CHAR:
      {
         onCharKey(wParam, 0, 0, 0);
         break;
      }
      case WM_MOUSEMOVE:
      {
         onMouseMove(VT_LOWORD(lParam), VT_HIWORD(lParam), 0, 0, 0, 0);
         break;
      }
      case WM_LBUTTONUP:
      {
         onMouseButtonUp(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::left_, 0, 0, 0);
         break;
      }
      case WM_MBUTTONUP:
      {
         onMouseButtonUp(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::middle_, 0, 0, 0);
         break;
      }
      case WM_RBUTTONUP:
      {
         onMouseButtonUp(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::right_, 0, 0, 0);
         break;
      }
      case WM_LBUTTONDOWN:
      {
         onMouseButtonDown(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::left_, 0, 0, 0);
         break;
      }
      case WM_MBUTTONDOWN:
      {
         onMouseButtonDown(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::middle_, 0, 0, 0);
         break;
      }
      case WM_RBUTTONDOWN:
      {
         onMouseButtonDown(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::right_, 0, 0, 0);
         break;
      }
      case WM_LBUTTONDBLCLK:
      {
         onMouseDblClick(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::left_, 0, 0, 0);
         break;
      }
      case WM_MBUTTONDBLCLK:
      {
         onMouseDblClick(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::middle_, 0, 0, 0);
         break;
      }
      case WM_RBUTTONDBLCLK:
      {
         onMouseDblClick(VT_LOWORD(lParam), VT_HIWORD(lParam), MouseButton::right_, 0, 0, 0);
         break;
      }
      case WM_MOUSEWHEEL:
      {
         onMouseWheel(VT_HIWORD(wParam), 0, 0, 0, 0);
         break;
      }
      case WM_MOVE:
      {
         onMove(VT_LOWORD(lParam), VT_HIWORD(lParam));
         break;
      }
      case WM_SHOWWINDOW:
      {
         onShow();
         break;
      }
      case WM_QUERYOPEN:
         onRestore();
      default:
         break;
   }
   return DefWindowProc(hWnd, message, wParam, lParam);
}

//-------------------------------------------------------------------------
void OSAppWindow::release() {
   delete this;
}

//-------------------------------------------------------------------------
void OSAppWindow::show() {
   ShowWindow(hwnd_, TRUE);
}

//-------------------------------------------------------------------------
void OSAppWindow::hide() {
   ShowWindow(hwnd_, FALSE);
}

void OSAppWindow::setSize(int width, int height) {
   RECT r = { 0, 0, width, height };
   AdjustWindowRect(&r, style_, FALSE);
   SetWindowPos(hwnd_, 0, 0, 0, r.right, r.bottom, 0);
}

//-------------------------------------------------------------------------
void OSAppWindow::registerWndClass(HINSTANCE hinstance, int color) {
   WNDCLASSEX wcex;
   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
   wcex.lpfnWndProc = OSAppWindow::wndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hinstance;
   wcex.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_APPLICATION);
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
   wcex.lpszMenuName = NULL;
   wcex.lpszClassName = L"VortexCoreWindowClass";
   wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
   RegisterClassEx(&wcex);
}

//-------------------------------------------------------------------------
bool OSAppWindow::isKeyDown(uint64_t key) {
   return (0 != GetAsyncKeyState((unsigned)key));
}

//-------------------------------------------------------------------------
bool OSAppWindow::getCursorPos(int& x, int& y) {
   POINT p;
   bool result = false;
   result = (0 != GetCursorPos(&p));
   x = p.x;
   y = p.y;
   return result;
}

//-------------------------------------------------------------------------
void OSAppWindow::setCursorPos(int x, int y) {
   SetCursorPos(x, y);
}

//-------------------------------------------------------------------------
RECT OSAppWindow::getWindowRect() {
   RECT rect;
   GetWindowRect(this->hwnd_, &rect);
   return rect;
}

//-------------------------------------------------------------------------
void OSAppWindow::startTimer(int ms) {
   //create timer thread
   if (runTimer_)
      return;
   timerInterval_ = ms;
   runTimer_.store(true);
   if (!timerThread_) {
      timerThread_ = std::make_unique<std::thread>(&Vt::App::Private::OSAppWindow::timer, this, (void*)hwnd_);
   }
}

//-------------------------------------------------------------------------
void OSAppWindow::stopTimer() {
   if (timerThread_ && runTimer_) {
      runTimer_.store(false);
      timerThread_->join();
      timerThread_ = nullptr;
   }
}


//-------------------------------------------------------------------------
LRESULT OSAppWindow::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
   WindowIter_ i = windows_.find(hWnd);
   if (i != windows_.end())
      return i->second->processMessage(hWnd, message, wParam, lParam);
   return DefWindowProc(hWnd, message, wParam, lParam);
}

//-------------------------------------------------------------------------
int OSAppWindow::run() {
   MSG msg = { 0 };
   while (refCount_) {
       if (GetMessage(&msg, NULL, 0, 0)){
           TranslateMessage(&msg);
           {
              //std::lock_guard<std::recursive_mutex> l(windowLock_);
              DispatchMessage(&msg);
           }
       }
   }
   return 0;
}

//-------------------------------------------------------------------------
void OSAppWindow::close() {
   this->stopTimer();
   close_ = true;
   PostMessage(this->hwnd_, WM_CLOSE, 0, 0);
}

//-------------------------------------------------------------------------
void OSAppWindow::lock()  const {
   windowLock_.lock();
}

//-------------------------------------------------------------------------
void OSAppWindow::unlock()  const {
   windowLock_.unlock();
}

//-------------------------------------------------------------------------
int OSAppWindow::width() {
   return width_;
}

//-------------------------------------------------------------------------
int OSAppWindow::height() {
   return height_;
}

//-------------------------------------------------------------------------
HWND OSAppWindow::handle() {
   return hwnd_;
}

//-------------------------------------------------------------------------
HWND OSAppWindow::winId() {
   return hwnd_;
}

HINSTANCE OSAppWindow::instance() {
   return (HINSTANCE)GetWindowLongPtr(hwnd_, GWLP_HINSTANCE);
}

//-------------------------------------------------------------------------
void OSAppWindow::onCreate() {
   mBackPtr.OnCreate();
}

//-------------------------------------------------------------------------
bool OSAppWindow::onClose() {
   auto rs = mBackPtr.OnClose();
   for (auto i : rs) {
      if (i == false)
         return false;
   }
   return true;
}

//-------------------------------------------------------------------------
void OSAppWindow::onPaint() {
   mBackPtr.OnPaint();
}

//-------------------------------------------------------------------------
void OSAppWindow::onTimer() {
   mBackPtr.OnTimer();
}

//-------------------------------------------------------------------------
void OSAppWindow::onResize(int width, int height) {
   mBackPtr.OnResize(width, height);
}

void OSAppWindow::onResized(int width, int height) {
   mBackPtr.OnResized(width, height);
}

//-------------------------------------------------------------------------
void OSAppWindow::onKeydown(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl) {
   mBackPtr.OnKeydown(key, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onKeyup(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl) {
   mBackPtr.OnKeyup(key, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onCharKey(uint64_t key, uint64_t shift, uint64_t alt, uint64_t ctrl) {
   mBackPtr.OnCharKey(key, shift, alt, ctrl);

}

//-------------------------------------------------------------------------
void OSAppWindow::onMouseButtonDown(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl) {
   mBackPtr.OnMouseButtonDown(x, y, button, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onMouseButtonUp(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl) {
   mBackPtr.OnMouseButtonUp(x, y, button, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onMouseDblClick(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl) {
   mBackPtr.OnMouseDblClick(x, y, button, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onMouseMove(int x, int y, unsigned button, unsigned shift, unsigned alt, unsigned ctrl) {
   mBackPtr.OnMouseMove(x, y, button, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onMouseWheel(int clicks, unsigned button, unsigned shift, unsigned alt, unsigned ctrl) {
   mBackPtr.OnMouseWheel(clicks, button, shift, alt, ctrl);
}

//-------------------------------------------------------------------------
void OSAppWindow::onMove(int x, int y) {
   mBackPtr.OnMove(x, y);
}

//-------------------------------------------------------------------------
void OSAppWindow::onRestore() {
   mBackPtr.OnRestore();
}

//-------------------------------------------------------------------------
void OSAppWindow::onMinimize() {
   mBackPtr.OnMinimize();
}

//-------------------------------------------------------------------------
void OSAppWindow::onMaximize() {
   mBackPtr.OnMaximize();
}

//-------------------------------------------------------------------------
void OSAppWindow::onShow() {
   mBackPtr.OnShow();
}

//-------------------------------------------------------------------------
void OSAppWindow::onHide() {
   mBackPtr.OnHide();
}
}
}
}