#include <boost/test/unit_test.hpp>

#include "filepath.h"
#include "filemanager.h"

#include "time.h"
//#include "filepath.h"

namespace {
int inc(int& index) {
  return index++;
}
}

namespace test {
// --log_level=test_suite --run_test=TestFileList
BOOST_AUTO_TEST_SUITE(TestFileList)

void doComparing(const fs::FilePath& first, const fs::FilePath& second) {
  // Test checks that sort algorithm swaps entries
  std::vector<fs::FilePath> paths;

  paths.push_back(first);
  paths.push_back(second);

  std::sort(paths.begin(), paths.end(), fs::WordNumberOrder());

  BOOST_CHECK_EQUAL(paths[0].getPath(), second.getPath());
  BOOST_CHECK_EQUAL(paths[1].getPath(), first.getPath());
}

BOOST_AUTO_TEST_CASE(Smoke) {
  doComparing(
    fs::FilePath("/abc/def/file2.jpg", true),
    fs::FilePath("/abc/def/file1.jpg", true));

  doComparing(
    fs::FilePath("/abc/def/folder2", false),
    fs::FilePath("/abc/def/folder1", false));

  doComparing(
    fs::FilePath("/abc/def/folder2/file1", true),
    fs::FilePath("/abc/def/folder1", false));

  doComparing(
    fs::FilePath("/abc/def/folder1/subfolder1", false),
    fs::FilePath("/abc/def/folder1", false));

  doComparing(
    fs::FilePath("/abc/def/folder2/subfolder1", false),
    fs::FilePath("/abc/def/folder1", false));

  doComparing(
    fs::FilePath("/abc/def/folder2/subfolder1/file1", true),
    fs::FilePath("/abc/def/folder1", false));

  doComparing(
    fs::FilePath("/abc/def/file1", true),
    fs::FilePath("/abc/def/folder2/subfolder1/file1", true));

  doComparing(
    fs::FilePath("/abc/def/file7.jpg", true),
    fs::FilePath("/abc/def/folder1/file2.jpg", true));

  doComparing(
    fs::FilePath("/abc/def/folder5/subfolder1", false),
    fs::FilePath("/abc/def/folder1/file1.jpg", true));

  doComparing(
    fs::FilePath("/path/to/dir_11", false),
    fs::FilePath("/path/to/dir_1", false));

  doComparing(
    fs::FilePath("/path/to/dir_11", false),
    fs::FilePath("/path/to/dir_10", false));

  doComparing(
    fs::FilePath("/path/to/dir_100", false),
    fs::FilePath("/path/to/dir_11", false));

  doComparing(
    fs::FilePath("/path/to/dir_001_2", false),
    fs::FilePath("/path/to/dir_1_1", false));
}

BOOST_AUTO_TEST_CASE(Sorting_FirstWordThenNumbers) {
  std::vector<fs::FilePath> paths;

  paths.push_back(fs::FilePath("/aaab20cdf/aaaa04.zip", true));
  paths.push_back(fs::FilePath("/aaab020cdf/aaaa03.zip", true));
  paths.push_back(fs::FilePath("/aaab20cde/aaaa03.zip", true));
  paths.push_back(fs::FilePath("/aaab20/aaaa03.zip", true));
  paths.push_back(fs::FilePath("/aaab20/aaaa03.zip", true));
  paths.push_back(fs::FilePath("/aaab20/aaaA02.zip", true));
  paths.push_back(fs::FilePath("/aaab20", false));
  paths.push_back(fs::FilePath("/aaab20/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab12/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab11/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab10/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab03/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab02/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaab01/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa12/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa11/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa10/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa9/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa4/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa3/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa2/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/", false));
  paths.push_back(fs::FilePath("/aaaa1/aaaa20.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa12.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa11.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa10.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa03.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa02.zip", true));
  paths.push_back(fs::FilePath("/aaaa1/aaaa01.zip", true));

  //TODO: random shuffle

  srand(static_cast<unsigned int>(time(0)));
  std::random_shuffle(paths.begin(), paths.end());
  std::sort(paths.begin(), paths.end(), fs::WordNumberOrder());

  int i = 0;

  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa02.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa10.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa11.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa12.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa1/aaaa20.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa2/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa3/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa4/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa9/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa10/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa11/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa12/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab01/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab02/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab03/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab10/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab11/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab12/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20/aaaA02.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20cde/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab020cdf/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab20cdf/aaaa04.zip");
}

BOOST_AUTO_TEST_CASE(Sorting_SortOnlyNames) {
  std::vector<fs::FilePath> paths;

  paths.push_back(fs::FilePath("aaa 21 q", false));
  paths.push_back(fs::FilePath("aaa 20 q", false));
  paths.push_back(fs::FilePath("aaa 11 q", false));
  paths.push_back(fs::FilePath("aaa 10 q", false));
  paths.push_back(fs::FilePath("aaa 2  q", false));
  paths.push_back(fs::FilePath("aaa 1  q", false));

  srand(static_cast<unsigned int>(time(0)));
  std::random_shuffle(paths.begin(), paths.end());
  std::sort(paths.begin(), paths.end(), fs::WordNumberOrder());

  int i = 0;
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 1  q");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 2  q");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 10 q");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 11 q");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 20 q");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "aaa 21 q");
}

BOOST_AUTO_TEST_CASE(Sorting_JustNumbers) {
  std::vector<fs::FilePath> paths;

  paths.push_back(fs::FilePath("/aaa3a/aaaa03s.zip", true));
  paths.push_back(fs::FilePath("/aaa3a/aaab02s.zip", true));
  paths.push_back(fs::FilePath("/aaa3a/aaac01s.zip", true));
  paths.push_back(fs::FilePath("/aaa3a/aaaa01.zip", true));
  paths.push_back(fs::FilePath("/aaaa2a/aaaa02.zip", true));
  paths.push_back(fs::FilePath("/aaab1a/aaaa03.zip", true));

  srand(static_cast<unsigned int>(time(0)));
  std::random_shuffle(paths.begin(), paths.end());
  std::sort(paths.begin(), paths.end(), fs::NumberOrder());
  int i = 0;

  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaab1a/aaaa03.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaaa2a/aaaa02.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaa3a/aaaa01.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaa3a/aaac01s.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaa3a/aaab02s.zip");
  BOOST_CHECK_EQUAL(paths[inc(i)].getPath(), "/aaa3a/aaaa03s.zip");
}

BOOST_AUTO_TEST_SUITE_END()
}
