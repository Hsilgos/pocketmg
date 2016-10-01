#include <boost/test/unit_test.hpp>

#include "common/iArchive.h"

namespace {
struct ArchiverFixture
{
    void CheckSimpleArchive(const std::string& path)
    {
        fs::FilePath file_path(path, true);
        archive::IArchive* opened_archive = archive::recognize(file_path);
        BOOST_REQUIRE(opened_archive);
        std::vector<fs::FilePath> paths = opened_archive->getFileList(true);
        std::sort(paths.begin(), paths.end());

        auto expected = {fs::FilePath("content/file3.txt", true),
                         fs::FilePath("file1.txt", true),
                         fs::FilePath("file2.txt", true)};
        BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), paths.begin(), paths.end());
    }
};

// --log_level=test_suite --run_test=TestArchiver
BOOST_FIXTURE_TEST_SUITE(TestArchiver, ArchiverFixture)

BOOST_AUTO_TEST_CASE(Rar) {
    CheckSimpleArchive("test_data/archives/archive.rar");
}

BOOST_AUTO_TEST_CASE(Zip) {
    CheckSimpleArchive("test_data/archives/archive.zip");
}

BOOST_AUTO_TEST_CASE(Jar) {
    CheckSimpleArchive("test_data/archives/archive.jar");
}

BOOST_AUTO_TEST_CASE(SevenZip) {
    CheckSimpleArchive("test_data/archives/archive.7z");
}

BOOST_AUTO_TEST_CASE(Tar) {
    CheckSimpleArchive("test_data/archives/archive.tar");
}

BOOST_AUTO_TEST_SUITE_END()
}
