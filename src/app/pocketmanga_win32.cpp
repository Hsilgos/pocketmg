// pocketmanga.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <functional>

#include "src/filepath.h"
#include "src/filemanager.h"
#include "src/image.h"
#include "src/book.h"
#include "src/mirror.h"
#include "src/scale.h"
#include "src/rotate.h"
#include "src/defines.h"
#include "src/cacheScaler.h"
#include "src/win32/scoped_handle.h"
#include "src/imgDecoderFactory.h"

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

manga::Book book_;
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

/*#include <opencv2/opencv.hpp>

   cv::Mat to_cv(const img::Image &img) {
   img::Image rotated = img::rotate(img, img::Angle_90);

   cv::Mat mat1;
   mat1.create(rotated.height(), rotated.width(), CV_MAKETYPE(CV_8U, rotated.depth()));
   memcpy(mat1.data, rotated.data(), rotated.height() * rotated.width() * rotated.depth());

   return mat1;
   }

   cv::Mat to_cv(manga::CacheScaler::Cache &cache) {
   if( cache.representation == manga::CacheScaler::Parts3 ) {
    img::Image dst;
    utils::Rect bounds = cache.bounds;

    if(1 == cache.currentShowing)
      bounds.x = (cache.image.width() - bounds.width) / 2;
    else if(2 == cache.currentShowing)
      bounds.x = cache.image.width() - bounds.width;

    copyRect(cache.image, dst, bounds);

    cache.currentShowing++;

    return to_cv(dst);
   } else {
    return to_cv(cache.image);
   }
   }
   /*int main() {
   manga::Book book;

   book.setRoot(fs::FilePath("i:\\tmp", false));
   manga::CacheScaler* scaler = new manga::CacheScaler(600, 800);
   book.setCachePrototype(scaler);

   img::Image img;

   img::Image img2;
   img2.enableMinimumReallocations(true);

   img::Image img3;
   img3.enableMinimumReallocations(true);

   while (book.incrementPosition()) {
    //img = book.currentImage();
    //toBgr(img, img);
    //img.copyTo(img2);
    //cv::Mat cv = to_cv(img);
    //toGray(img, img);

    //img::copyRect(img, img, utils::Rect(0, 0, 400, 400));

    //img = img::scale(img, img::HighScaling, 600, 0);

    //img = img::rotate(img, img::Angle_90);

    //cv::imshow("ololo1", to_cv(img));
    /*cv::imshow("ololo1", to_cv(scaler->scaledGrey()));

       book.preload();

       if( scaler->scaledGrey().representation == manga::CacheScaler::Parts3) {
       cv::waitKey(2000);
       cv::imshow("ololo1", to_cv(scaler->scaledGrey()));
       cv::waitKey(3000);
       cv::imshow("ololo1", to_cv(scaler->scaledGrey()));
       }*/
//cv::Mat to_cv(img2)
//cv::imshow("ololo2", mat2);
//cv::imshow("ololo2", mat2);
//cv::imshow("ololo2", mat2);

/*cv::waitKey(3000);*/


/*}


   return 0;


   /* fs::IFileManager *mgr = fs::IFileManager::create();
   std::vector<fs::FilePath> list = mgr->getFileList("i:\\books\\Prison School\\Том 01", fs::IFileManager::Directory, true);

   fs::sort(list, fs::FirstWordThenNumbers);
   //fs::sort(list, fs::JustNumbers);

   std::vector<fs::FilePath>::iterator it = list.begin(), itEnd = list.end();
   for ( ; it != itEnd; ++it )
   {
   std::cout << it->getPath() << std::endl;
   }

   return 0;*/
//}
