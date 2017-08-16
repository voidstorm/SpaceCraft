#include "../include/VortexCore/AppWindow.h"
#ifdef _WIN32
#include "../include/VortexCore/win32/OSAppWindow.h"
template<> HWND VORTEX_API Vt::App::AppWindow::winId() {
   return mImpl->winId();
}

template<> HWND VORTEX_API Vt::App::AppWindow::winId() const {
   return mImpl->winId();
}

template<> HINSTANCE VORTEX_API Vt::App::AppWindow::instance() {
   return mImpl->instance();
}

template<> HINSTANCE VORTEX_API Vt::App::AppWindow::instance() const {
   return mImpl->instance();
}
#endif


Vt::App::AppWindow::AppWindow(std::wstring name, int width, int height, Style style, bool visible) :
   mImpl{ std::make_unique<Vt::App::Private::OSAppWindow>(*this, name, width, height, static_cast<unsigned>(style), visible) } {
}


Vt::App::AppWindow::~AppWindow() {

}

int Vt::App::AppWindow::width() {
   return mImpl->width();
}

int Vt::App::AppWindow::height() {
   return mImpl->height();
}

void Vt::App::AppWindow::show() {
   mImpl->show();
}

void Vt::App::AppWindow::hide() {
   mImpl->hide();
}

void Vt::App::AppWindow::setSize(int width, int height) {
   mImpl->setSize(width, height);
}

void Vt::App::AppWindow::startTimer(int ms) {
   mImpl->startTimer(ms);
}

void Vt::App::AppWindow::stopTimer() {
   mImpl->stopTimer();
}

void Vt::App::AppWindow::close() {
   mImpl->close();
}

void Vt::App::AppWindow::lock()  const {
   mImpl->lock();
}

void Vt::App::AppWindow::unlock() const {
   mImpl->unlock();
}


int Vt::App::AppWindow::exec() {
   return Vt::App::Private::OSAppWindow::run();
}

//delete window content
void Vt::App::AppWindow::deleter() {
   mSelf.reset();
}

void Vt::App::AppWindow::registerDeleter() {
   mSelf = shared_from_this();
}

std::weak_ptr <Vt::App::AppWindow> Vt::App::AppWindow::create(std::wstring name, int width, int height, Style style, bool visible) {
   //auto tmp = std::make_shared<AppWindow>(name, width, height, style, visible);
   auto tmp = std::shared_ptr<AppWindow>(new AppWindow(name, width, height, style, visible));
   tmp->registerDeleter();
   return tmp;
}
