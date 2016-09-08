TOOLS_BEGIN_PROJECT(app CUI_APP
	DEPENDS
		common
	EXTERNAL_DEPENDS
                X11
                png
                jpeg)

TOOLS_SOURCES(
    ../common/bmpDecoder.cpp
    ../common/pngDecoder.cpp
    pocketmanga_linux.cpp
)


TOOLS_END_PROJECT()
