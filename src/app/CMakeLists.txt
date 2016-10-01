TOOLS_BEGIN_PROJECT(app CUI_APP
    DEPENDS
        common
    EXTERNAL_DEPENDS
        X11
        png
        jpeg
        pthread)

TOOLS_SOURCES(
    ../common/decoders/bmpDecoder.cpp
    ../common/decoders/jpegDecoder.cpp
    ../common/decoders/pngDecoder.cpp
    pocketmanga_linux.cpp
)


TOOLS_END_PROJECT()
