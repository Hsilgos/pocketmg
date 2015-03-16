// pocketmanga.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>

#include "src/filepath.h"
#include "src/filemanager.h"
#include "src/image.h"
#include "src/book.h"
#include "src/scale.h"
#include "src/rotate.h"
#include "src/defines.h"
#include "src/cacheScaler.h"


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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message) {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code here...
    EndPaint(hWnd, &ps);
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
   /*
   INTER_NEAREST=CV_INTER_NN, //!< nearest neighbor interpolation
   INTER_LINEAR=CV_INTER_LINEAR, //!< bilinear interpolation
   INTER_CUBIC=CV_INTER_CUBIC, //!< bicubic interpolation
   INTER_AREA=CV_INTER_AREA, //!< area-based (or super) interpolation
   INTER_LANCZOS4=CV_INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
   INTER_MAX=7,
   WARP_INVERSE_MAP=CV_WARP_INVERSE_MAP
 */
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
