// pocketmanga.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <memory>
#include <functional>

#include "common/filepath.h"
#include "common/filemanager.h"
#include "common/image.h"
#include "common/book.h"
#include "common/mirror.h"
#include "common/scale.h"
#include "common/rotate.h"
#include "common/defines.h"
#include "common/cacheScaler.h"
#include "common/imgDecoderFactory.h"

#if 0
HINSTANCE hInst;                                                                // current instance
const wchar_t szTitle[] = L"PocketManga";                                       // The title bar text
const wchar_t szWindowClass[] = L"PocketMangaWindow";                   // the main window class name

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex = {0};

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = NULL;
  wcex.hCursor = NULL;
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = NULL;

  RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance) {
  HWND hWnd;

  hInst = hInstance;    // Store instance handle in our global variable

  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

  if (!hWnd) {
    return FALSE;
  }

  ShowWindow(hWnd, SW_SHOW);
  UpdateWindow(hWnd);

  return TRUE;
}


img::Image current_image_;
ScopedHandle<HBITMAP> bitmap_;

class ScopedPaint {
public:
  ScopedPaint(HWND hwnd) : hwnd_(hwnd), hdc_(::BeginPaint(hwnd, &ps_)) {}

  ~ScopedPaint() {
    EndPaint(hwnd_, &ps_);
  }

  HDC hdc() {
    return hdc_;
  }

private:
  ScopedPaint& operator=(const ScopedPaint&);
  const HWND hwnd_;
  PAINTSTRUCT ps_;
  const HDC hdc_;
};

BYTE bBits[] = {
  0xBF, 0xA0,
  0x1F, 0x00,
  0x8E, 0x20,
  0xC4, 0x60,
  0xE0, 0xE0,
  0xF1, 0xE0,
  0xE0, 0xE0,
  0xC4, 0x60,
  0x8E, 0x20,
  0x1F, 0x00,
  0xBF, 0xA0
};

void UpdateCurrentImage(HWND hWnd) {
  current_image_ = book_.currentImage();
  //img::Image t = book_.currentImage();
  //img::mirror(t, current_image_, img::LeftToRight);
  //current_image_ = img::scale(current_image_, img::HighScaling, 600, 0);
  //toGray(current_image_, current_image_);
  SetWindowTextA(hWnd, book_.bookmark().currentFile.filePath.getFileName().c_str());

  ScopedHandle<HDC> hdc(GetDC(NULL), std::bind(::ReleaseDC, static_cast<HWND>(NULL), std::placeholders::_1));
  if (hdc.handle()) {
    bitmap_.reset(::CreateCompatibleBitmap(hdc.handle(), current_image_.width(), current_image_.height()), ::DeleteObject);
    if (bitmap_.handle()) {
      ScopedHandle<HDC> hdc_mem(CreateCompatibleDC(hdc.handle()), DeleteDC);
      if (hdc_mem.handle()) {
        ScopedSelectObject obj(hdc_mem.handle(), bitmap_.handle());

        BITMAPINFO bi = {0};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = current_image_.width();
        bi.bmiHeader.biHeight = current_image_.height();
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 8 * current_image_.depth();
        bi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(hdc_mem.handle(), 0, 0, current_image_.width(), current_image_.height(),
                      0, 0, current_image_.width(), current_image_.height(), current_image_.data(),
                      (LPBITMAPINFO)&bi, DIB_RGB_COLORS, SRCCOPY);
      }
    }
  }

  InvalidateRect(hWnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_PAINT: {
    ScopedPaint paint(hWnd);
    if (bitmap_.handle()) {
      ScopedHandle<HDC> mem_dc(CreateCompatibleDC(paint.hdc()), ::DeleteDC);
      ScopedSelectObject obj(mem_dc.handle(), bitmap_.handle());
      BitBlt(paint.hdc(), 0, 0, current_image_.width(), current_image_.height(), mem_dc.handle(), 0, 0, SRCCOPY);
    }
  }
  break;
  case WM_KEYDOWN:
    if (wParam == VK_LEFT) {
      if (book_.decrementPosition())
        UpdateCurrentImage(hWnd);
    } else if (wParam == VK_RIGHT) {
      if (book_.incrementPosition())
        UpdateCurrentImage(hWnd);
    }

    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}


int main() {
  //book_.setRoot(fs::FilePath("i:\\tmp", false));
  img::DecoderFactory::getInstance().setAlignment(sizeof(DWORD));
  //img::DecoderFactory::getInstance().setDesiredBytePerPixel(1);
  book_.setRoot(fs::FilePath("e:/pocketbook/pocketmanga/test/resources", false));
  //manga::CacheScaler* scaler = new manga::CacheScaler(600, 800);
  //book_.setCachePrototype(scaler);
  // TODO: Place code here.
  HINSTANCE hInstance = GetModuleHandle(NULL);

  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance))
    return FALSE;

  // Main message loop:
  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}
#else

#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

inline void DoDestryImage(XImage* image)
{
    XDestroyImage(image);
}

std::unique_ptr<XImage, void(*)(XImage*)> CreateTrueColorImage(img::Image& image, Display *display, Visual *visual)
{
    char* data = reinterpret_cast<char*>(malloc(dataSize(image)));
    memcpy(data, image.data(), dataSize(image));

    XImage* ptr = XCreateImage(display, visual, 24, ZPixmap, 0, data, image.width(), image.height(), 32, image.scanline(true));

    return std::unique_ptr<XImage, void(*)(XImage*)>(ptr, DoDestryImage);
}

img::Image new_image;
img::Image current_image;
std::unique_ptr<XImage, void(*)(XImage*)> current_ximage(nullptr, DoDestryImage);
manga::Book book;

void UpdateCurrentImage(Display *display)
{
    current_image = book.currentImage();
    if (current_image.depth() == 4)
    {
        new_image = current_image;
    }
    else if (current_image.depth() == 3)
    {
        img::rgb2rgba(current_image, new_image);
    }
    else if (current_image.depth() == 1)
    {
        img::grey2rgba(current_image, new_image);
    }
    img::mirror(new_image, new_image, img::UpsideDown);
    current_ximage = CreateTrueColorImage(new_image, display, DefaultVisual(display, 0));
}

void processEvent(Display *display, Window window)
{
    static char *tir="This is red";
    static char *tig="This is green";
    static char *tib="This is blue";
    XEvent ev;
    XNextEvent(display, &ev);
    switch(ev.type)
    {
    case Expose:
        XPutImage(display, window, DefaultGC(display, 0), current_ximage.get(), 0, 0, 0, 0, current_image.width(), current_image.height());
        XSetForeground(display, DefaultGC(display, 0), 0x00ff0000); // red
        XDrawString(display, window, DefaultGC(display, 0), 0, current_image.height() + 20, book.bookmark().currentFile.filePath.getFileName().c_str(), book.bookmark().currentFile.filePath.getFileName().size());
        break;
    case KeyPress:
        KeySym const key_sum = XLookupKeysym(&ev.xkey, 0);
        if (key_sum == XK_Right)//XK_Right  - 114
            book.incrementPosition();
        else if (key_sum == XK_Left)//XK_Left
            book.decrementPosition();

        UpdateCurrentImage(display);
        XWindowAttributes attributes;
        XGetWindowAttributes(display, window, &attributes);
        XClearArea(display, window, 0, 0, attributes.width, attributes.height, false);
        XPutImage(display, window, DefaultGC(display, 0), current_ximage.get(), 0, 0, 0, 0, current_image.width(), current_image.height());

        XDrawString(display, window, DefaultGC(display, 0), 0, current_image.height() + 20, book.bookmark().currentFile.filePath.getFileName().c_str(), book.bookmark().currentFile.filePath.getFileName().size());
    }
}

int main(int argc, char **argv)
{
    img::DecoderFactory::getInstance().setDecodeMode(img::DecodeIntoRgba);

    book.setRoot(fs::FilePath("/home/hsilgos/Dropbox/Projects/pocketmanga/test/resources/valid", false));
    book.incrementPosition();

    current_image = book.currentImage();

    //int width=512, height=512;
    Display *display=XOpenDisplay(NULL);
    Visual *visual=DefaultVisual(display, 0);
    Window window=XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, 800, 600, 1, 0, 0);
    if(visual->c_class!=TrueColor)
    {
        fprintf(stderr, "Cannot handle non true color visual ...\n");
        exit(1);
    }

    UpdateCurrentImage(display);

    XSelectInput(display, window, ButtonPressMask|ExposureMask|KeyPressMask);
    XMapWindow(display, window);
    while(1)
    {
        processEvent(display, window);
    }
}

#endif
