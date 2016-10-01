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
#include "common/decoders/imgDecoderFactory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
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
    /*static char *tir="This is red";
    static char *tig="This is green";
    static char *tib="This is blue";*/
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
    img::DecoderFactory::getInstance().setDecodeMode(img::DecodeIntoGray);

    //book.setRoot(fs::FilePath("/home/hsilgos/Dropbox/Projects/pocketmanga/test/resources/valid", false));
    book.setRoot(fs::FilePath("/home/hsilgos/Dropbox/apictures", false));
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
