TOOLS_BEGIN_PROJECT(test CUI_APP
    DEPENDS
        common
    EXTERNAL_DEPENDS
        X11
        png
        jpeg
        boost_system)

TOOLS_SOURCES(
    ../../src/common/bmpDecoder.cpp
    ../../src/common/jpegDecoder.cpp
    ../../src/common/pngDecoder.cpp
    clone.h
    globalTest.cpp
    globalTest.h
    report.cpp
    report.h
    singleton.h
    smartDelete.h
    tesRect.cpp
    testArchive.cpp
    testArchive.h
    testBenchmark.cpp
    testBenchmark.h
    testBmp.cpp
    testBook.cpp
    testColor.cpp
    testFileList.cpp
    testFilePath.cpp
    testFileSystem.cpp
    testFileSystem.h
    testFormatter.cpp
    testFormatter.h
    testImageDecoder.cpp
    testImageDecoder.h
    testJpg_jpg.cpp
    testJpg_jpg.h
    testName.h
    testRotate.cpp
    testScale.cpp
    testUtils.cpp
    testUtils.h
    timeInfo.h
)

file(GLOB VALID_BMP "resources/valid/*.bmp")
TOOLS_RESOURCE_TO_CODE(bmp/valid ${VALID_BMP})

file(GLOB CORRUPT_BMP "resources/corrupt/*.bmp")
TOOLS_RESOURCE_TO_CODE(bmp/corrupt ${CORRUPT_BMP})

file(GLOB QUESTIONABLE_BMP "resources/questionable/*.bmp")
TOOLS_RESOURCE_TO_CODE(bmp/questionable ${QUESTIONABLE_BMP})

TOOLS_END_PROJECT()
