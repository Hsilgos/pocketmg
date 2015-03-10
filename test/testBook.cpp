#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "book.h"
#include "image.h"

#include "testFileSystem.h"
#include "testImageDecoder.h"

namespace test {
// --log_level=test_suite --run_test=TestBook
BOOST_AUTO_TEST_SUITE( TestBook )

// --log_level=test_suite --run_test=TestBook/BookExplorer

std::auto_ptr<TestFileSystem> ConstructTestFS() {
  std::auto_ptr<TestFileSystem> tfs(new TestFileSystem);

  tfs->addFile("/abc/def/folder1/file1.jpg");
  tfs->addFile("/abc/def/folder1/file2.jpg");
  tfs->addFile("/abc/def/folder1/file3.jpg");
  tfs->addFile("/abc/def/folder2/file4.jpg");
  tfs->addFile("/abc/def/folder2/file5.jpg");
  tfs->addFile("/abc/def/folder3/file6.jpg");
  tfs->addFolder("/abc/def/folder4");
  tfs->addFile("/abc/def/folder5/subfolder1/file7.jpg");
  tfs->addFile("/abc/def/folder5/subfolder1/file8.jpg");
  tfs->addFile("/abc/def/file7.jpg");

  return tfs;
}

bool CheckPathInArcEmpty(const std::vector<manga::PathToFile> &collect) {
  BOOST_FOREACH(const manga::PathToFile &path, collect) {
    if(!path.pathInArchive.empty())
      return false;
  }
  return true;
}

BOOST_AUTO_TEST_CASE( Explorer ) {
  manga::BookExplorer explorer(ConstructTestFS().release());
  BOOST_CHECK(explorer.setRoot(fs::FilePath("/abc/def/", false)));

  std::vector<manga::PathToFile> root_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(root_list.size(), 6);
  BOOST_CHECK_EQUAL(root_list[0].filePath, fs::FilePath("/abc/def/folder1", false));
  BOOST_CHECK_EQUAL(root_list[1].filePath, fs::FilePath("/abc/def/folder2", false));
  BOOST_CHECK_EQUAL(root_list[2].filePath, fs::FilePath("/abc/def/folder3", false));
  BOOST_CHECK_EQUAL(root_list[3].filePath, fs::FilePath("/abc/def/folder4", false));
  BOOST_CHECK_EQUAL(root_list[4].filePath, fs::FilePath("/abc/def/folder5", false));
  BOOST_CHECK_EQUAL(root_list[5].filePath, fs::FilePath("/abc/def/file7.jpg", true));
  BOOST_CHECK(CheckPathInArcEmpty(root_list));

  // folder 1
  BOOST_REQUIRE(explorer.enter(root_list[0]));
  std::vector<manga::PathToFile> folder1_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(folder1_list.size(), 3);
  BOOST_CHECK_EQUAL(folder1_list[0].filePath, fs::FilePath("/abc/def/folder1/file1.jpg", true));
  BOOST_CHECK_EQUAL(folder1_list[1].filePath, fs::FilePath("/abc/def/folder1/file2.jpg", true));
  BOOST_CHECK_EQUAL(folder1_list[2].filePath, fs::FilePath("/abc/def/folder1/file3.jpg", true));
  BOOST_CHECK(CheckPathInArcEmpty(folder1_list));

  // back to root
  BOOST_CHECK(explorer.back());
  std::vector<manga::PathToFile> root_list2 = explorer.fileList();
  BOOST_CHECK_EQUAL_COLLECTIONS(
    root_list.begin(), root_list.end(),
    root_list2.begin(), root_list2.end());

  // folder 4
  BOOST_REQUIRE(explorer.enter(root_list[3]));
  std::vector<manga::PathToFile> folder4_list = explorer.fileList();
  BOOST_CHECK(folder4_list.empty());
  BOOST_CHECK(CheckPathInArcEmpty(folder4_list));

  // folder 5
  BOOST_REQUIRE(explorer.enter(root_list[4]));
  std::vector<manga::PathToFile> folder5_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(folder5_list.size(), 1);
  BOOST_CHECK_EQUAL(folder5_list[0].filePath, fs::FilePath("/abc/def/folder5/subfolder1", false));
  BOOST_CHECK(CheckPathInArcEmpty(folder5_list));

  BOOST_CHECK(!explorer.enter(fs::FilePath("/abcd", false)));
  BOOST_CHECK(!explorer.enter(fs::FilePath("/abc/defg", false)));
}

void CheckArchPath(const std::vector<manga::PathToFile> &collect) {
  BOOST_FOREACH(const manga::PathToFile &path, collect)
  BOOST_CHECK_EQUAL(path.filePath, fs::FilePath("/path/to/archive/archive1.testarch", true));
}

BOOST_AUTO_TEST_CASE( ExplorerWithArchive ) {
  std::auto_ptr<TestFileSystem> tfs(new TestFileSystem);
  TestArchiver archiver(tfs.get());

  TestArchive archive1("archive1.testarch");
  archive1.addFile("file1.jpg");
  archive1.addFile("folder1/file1.jpg");
  archive1.addFile("folder1/file2.jpg");
  archive1.addFile("folder1/file3.jpg");
  archive1.addFile("folder2/subfolder/file4.jpg");
  archive1.addFile("folder2/subfolder/file5.jpg");
  archive1.addFolder("folder3");

  tfs->addArchive("/path/to/archive", archive1);
  manga::BookExplorer explorer(tfs.release());
  BOOST_CHECK(explorer.setRoot(fs::FilePath("/path/to/", false)));
  BOOST_CHECK(explorer.enter(
                manga::PathToFile(
                  fs::FilePath("/path/to/archive/archive1.testarch", true))));

  std::vector<manga::PathToFile> root_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(root_list.size(), 4);
  BOOST_CHECK_EQUAL(root_list[0].pathInArchive, fs::FilePath("/folder1", false));
  BOOST_CHECK_EQUAL(root_list[1].pathInArchive, fs::FilePath("/folder2", false));
  BOOST_CHECK_EQUAL(root_list[2].pathInArchive, fs::FilePath("/folder3", false));
  BOOST_CHECK_EQUAL(root_list[3].pathInArchive, fs::FilePath("file1.jpg", true));
  CheckArchPath(root_list);

  // folder1
  BOOST_CHECK(explorer.enter(root_list[0]));
  std::vector<manga::PathToFile> folder1_list = explorer.fileList();
  BOOST_CHECK_EQUAL(folder1_list[0].pathInArchive, fs::FilePath("/folder1/file1.jpg", true));
  BOOST_CHECK_EQUAL(folder1_list[1].pathInArchive, fs::FilePath("/folder1/file2.jpg", true));
  BOOST_CHECK_EQUAL(folder1_list[2].pathInArchive, fs::FilePath("/folder1/file3.jpg", true));
  CheckArchPath(folder1_list);

  // back
  BOOST_CHECK(explorer.back());
  std::vector<manga::PathToFile> root_list2 = explorer.fileList();
  BOOST_CHECK_EQUAL_COLLECTIONS(
    root_list.begin(), root_list.end(),
    root_list2.begin(), root_list2.end());

  // folder2
  BOOST_CHECK(explorer.enter(root_list[1]));
  std::vector<manga::PathToFile> folder2_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(folder2_list.size(), 1);
  BOOST_CHECK_EQUAL(folder2_list[0].pathInArchive, fs::FilePath("folder2/subfolder", false));
  CheckArchPath(folder2_list);

  // back
  BOOST_CHECK(explorer.back());
  // back and out of archive
  BOOST_CHECK(explorer.back());
  std::vector<manga::PathToFile> arch_list = explorer.fileList();
  BOOST_REQUIRE_EQUAL(arch_list.size(), 1);
  BOOST_CHECK_EQUAL(arch_list[0].filePath, fs::FilePath("/path/to/archive/archive1.testarch", true));
  BOOST_CHECK(CheckPathInArcEmpty(arch_list));
}

class ExplorerTestFixture {
public:
  ExplorerTestFixture()
    :	file_system_(new TestFileSystem),
      archiver_(file_system_.get()) {
  }

  void addFile(const std::string &path, const std::string &image_data) {
    commitArchive();
    file_system_->addFile(path, CreateTestImage(image_data));
    iter_files_.push_back(manga::PathToFile(fs::FilePath(path, true)));

    if( !image_data.empty() )
      iter_images_.push_back(image_data);
  }

  void addFolder(const std::string &path, bool is_first_file = false) {
    commitArchive();
    file_system_->addFolder(path);
  }


  void addArchive(const std::string &path) {
    commitArchive();
    curr_archive_path_ = fs::FilePath(path, true);
    current_archive_ = TestArchive(curr_archive_path_.getFileName());
  }

  void addFileToArchive(const std::string &path, const std::string &image_data) {
    BOOST_REQUIRE(!current_archive_.empty());
    current_archive_.addFile(path, CreateTestImage(image_data));
    iter_files_.push_back(manga::PathToFile(curr_archive_path_, fs::FilePath(path, true)));

    if( !image_data.empty() )
      iter_images_.push_back(image_data);
  }

  void addFolderToArchive(const std::string &path, bool is_first_file = false) {
    BOOST_REQUIRE(!current_archive_.empty());
    current_archive_.addFolder("folder3");
  }

  TestFileSystem *releaseFileSystem() {
    return file_system_.release();
  }

  void DoNextIterationTest(manga::Book &book, bool with_preload) {
    BOOST_REQUIRE(book.toFirstFile());
    size_t i = 0;

    do {
      if( with_preload ) {
        book.preload();
      }

      BOOST_REQUIRE_LT(i, iter_images_.size());
      BOOST_CHECK_EQUAL(iter_images_[i++], DataFromTestImage(book.currentImage()));
    } while (book.incrementPosition());

    BOOST_CHECK_EQUAL(i, iter_images_.size());
  }

  void DoNextIterationTest(manga::BookExplorer &explorer) {
    BOOST_REQUIRE(explorer.toFirstFile());
    size_t i = 0;
    do {
      BOOST_REQUIRE_LT(i, iter_files_.size());
      BOOST_CHECK_EQUAL(iter_files_[i++], explorer.getCurrentPos());
    } while ( explorer.toNextFile() );
    BOOST_CHECK_EQUAL(i, iter_files_.size());
  }

  void DoPreviousIterationTest(manga::BookExplorer &explorer) {
    BOOST_REQUIRE(explorer.toLastFile());
    size_t i = iter_files_.size();
    do {
      BOOST_REQUIRE_GT(i, 0U);
      BOOST_CHECK_EQUAL(iter_files_[--i], explorer.getCurrentPos());
    } while (explorer.toPreviousFile());
    BOOST_CHECK_EQUAL(i, 0);
  }

  void Construct(bool with_archive, bool with_files) {
    addFolder("/path/to/dir_00/");
    addFolder("/path/to/dir_1/subdir 1");
    addFolder("/path/to/dir_1/subdir 2");
    addFolder("/path/to/dir_1/subdir 3/sub sub dir");

    if( with_files ) {
      addFile("/path/to/dir_2/file1.testimg", "Image File 1");
      addFile("/path/to/dir_5/file2.testimg", "Image File 2");
      addFile("/path/to/dir_10/file3.testimg", "Image File 3");
    }

    if( with_archive ) {
      addArchive("/path/to/dir_11/archive1.testarch");
      addFolderToArchive("/arc/path 00");
      addFileToArchive("/arc/path 1/file1.testimg", "Image File 4");
      addFileToArchive("/arc/path 1/file2.testimg", "Image File 5");
      addFileToArchive("/arc/path 5/file3.testimg", "Image File 6");
      addFolderToArchive("/arc/path 6");
      addFolderToArchive("/arc/path 7/subpath 1");
      addFolderToArchive("/arc/path 7/subpath 2");
      addFileToArchive("/arc/path 10/subpath 1/file4.testimg", "Image File 7");
      addFileToArchive("/arc/path 10/subpath 1/file5.testimg", "Image File 8");
      addFolderToArchive("/arc/path 15");
      addFolderToArchive("/arc/path 100/subpath 1");
      addFolderToArchive("/arc/path 100/subpath 2");
    }

    addFolder("/path/to/dir_20");
    addFolder("/path/to/dir_22/subdir 1");

    if( with_files ) {
      addFile("/path/to/dir_100/subdir 001/file1.testimg", "Image File 10");
      addFile("/path/to/dir_100/subdir 02/file2.testimg", "Image File 11");
      addFile("/path/to/dir_100/subdir 100/file3.testimg", "Image File 12");
      addFile("/path/to/dir_100/subdir 100/file4.testimg", "Image File 13");
    }

    if( with_archive ) {
      addArchive("/path/to/dir_100/subdir 101/archive1.testarch");
      addFileToArchive("file1.testimg", "Image File 14");
      addFileToArchive("file02.testimg", "Image File 15");
      addFileToArchive("file3.testimg", "Image File 16");
      addFileToArchive("file33.testimg", "Image File 17");
      addFileToArchive("file100.testimg", "Image File 18");
      addFileToArchive("file101.testimg", "Image File 19");

      addArchive("/path/to/dir_100/subdir 101/archive10.testarch");
      addFileToArchive("file01.testimg", "Image File 20");
      addFileToArchive("file2.testimg", "Image File 21");
      addFileToArchive("file03.testimg", "Image File 22");
      addFileToArchive("file33.testimg", "Image File 23");
      addFileToArchive("file100.testimg", "Image File 24");
      addFileToArchive("file101.testimg", "Image File 25");
    }

    addFolder("/path/to/dir_1000/");
    addFolder("/path/to/dir_1000/sundir 1");
    addFolder("/path/to/dir_1000/sundir 2");
  }
protected:
  void commitArchive() {
    if(!current_archive_.empty()) {
      file_system_->addArchive(curr_archive_path_.copy(curr_archive_path_.getDirLevel() - 1).getPath(), current_archive_);
      curr_archive_path_ = fs::FilePath();
      current_archive_ = TestArchive();
    }
  }
  std::auto_ptr<TestFileSystem> file_system_;

  TestArchiver archiver_;
  TestArchive current_archive_;
  fs::FilePath curr_archive_path_;
  TestImageDecoder image_decoder_;

  std::vector<manga::PathToFile> iter_files_;
  std::vector<std::string> iter_images_;
};

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir_NoArchives, ExplorerTestFixture ) {
  Construct(false, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir_NoArchives, ExplorerTestFixture ) {
  Construct(false, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir_ArchivesOnly, ExplorerTestFixture ) {
  Construct(true, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir_ArchivesOnly, ExplorerTestFixture ) {
  Construct(true, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir, ExplorerTestFixture ) {
  Construct(true, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir, ExplorerTestFixture ) {
  Construct(true, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}


BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File_NoArchives, ExplorerTestFixture ) {
  Construct(false, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File_NoArchives, ExplorerTestFixture ) {
  Construct(false, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File_ArchivesOnly, ExplorerTestFixture ) {
  Construct(true, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File_ArchivesOnly, ExplorerTestFixture ) {
  Construct(true, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File, ExplorerTestFixture ) {
  Construct(true, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File, ExplorerTestFixture ) {
  Construct(true, true);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  DoPreviousIterationTest(explorer);
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNothing_FileAndDir, ExplorerTestFixture ) {
  Construct(false, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  BOOST_CHECK(!explorer.toFirstFile());
  BOOST_CHECK(!explorer.toLastFile());
}

BOOST_FIXTURE_TEST_CASE( ExplorerIterateNothing_File, ExplorerTestFixture ) {
  Construct(false, false);

  manga::BookExplorer explorer(releaseFileSystem(), fs::IFileManager::File);

  explorer.setRoot(fs::FilePath("/path/to/", false));

  BOOST_CHECK(!explorer.toFirstFile());
  BOOST_CHECK(!explorer.toLastFile());
}


//////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( SmokeToolsTest ) {
  TestImageDecoder decoder;
  tools::ByteArray array = CreateTestImage("Test data");
  img::Image image;
  BOOST_CHECK(decoder.decode(array, image));
  BOOST_CHECK_EQUAL(DataFromTestImage(image), "Test data");
  BOOST_CHECK(!decoder.decode(tools::toByteArray("Wrong image data|Wrong image data|Wrong image data"), image));
}

std::vector<fs::FilePath> CallFixUp(
  const char *root,
  const char *path1, bool is_file1,
  const char *path2, bool is_file2) {
  std::vector<fs::FilePath> paths;

  if(path1)
    paths.push_back(fs::FilePath(path1, is_file1));

  if(path2)
    paths.push_back(fs::FilePath(path2, is_file2));

  manga::FixUpFileTreeForTest(paths, fs::FilePath(root, false));
  return paths;
}

void DoCheckFixUp(
  const std::vector<fs::FilePath> &files,
  const char *path1 = 0,
  const char *path2 = 0,
  const char *path3 = 0,
  const char *path4 = 0,
  const char *path5 = 0) {
  size_t count = 0;
  if( path1 )
    BOOST_CHECK_EQUAL(files[count++].getPath(), path1);
  if( path2 )
    BOOST_CHECK_EQUAL(files[count++].getPath(), path2);
  if( path3 )
    BOOST_CHECK_EQUAL(files[count++].getPath(), path3);
  if( path4 )
    BOOST_CHECK_EQUAL(files[count++].getPath(), path4);
  if( path5 )
    BOOST_CHECK_EQUAL(files[count++].getPath(), path5);

  BOOST_CHECK_EQUAL(count, files.size());
}

// --log_level=test_suite --run_test=TestBook/TestFixingUp
BOOST_AUTO_TEST_CASE( TestFixingUp ) {
  fs::FilePath::setGlobalSelector('/');
  std::vector<fs::FilePath> list1 = CallFixUp("/abc/def", "/abc/def/folder1", false, "/abc/def/folder1/folder2/file1", true);
  DoCheckFixUp(list1, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/folder2", "/abc/def/folder1/folder2/file1");

  std::vector<fs::FilePath> list2 = CallFixUp("/abc/def", "/abc/def/folder1/file1", true, "/abc/def/folder2/file2", true);
  DoCheckFixUp(list2, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/file1", "/abc/def/folder2", "/abc/def/folder2/file2");

  std::vector<fs::FilePath> list3 = CallFixUp("/abc/def", "/abc/def/folder1/folder2/file1", true, "/abc/def/folder1/file2", true);
  DoCheckFixUp(list3, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/folder2", "/abc/def/folder1/folder2/file1", "/abc/def/folder1/file2");

  std::vector<fs::FilePath> list4 = CallFixUp("/abc/def", "/abc/def/folder1", false, "/abc/def/folder2/folder3/file2", true);
  DoCheckFixUp(list4, "/abc/def", "/abc/def/folder1", "/abc/def/folder2", "/abc/def/folder2/folder3", "/abc/def/folder2/folder3/file2");
}

//////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_ArchivesFiles, ExplorerTestFixture ) {
  Construct(true, true);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, false);
}

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Archives, ExplorerTestFixture ) {
  Construct(true, false);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, false);
}

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Files, ExplorerTestFixture ) {
  Construct(false, true);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, false);
}

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_ArchivesFiles, ExplorerTestFixture ) {
  Construct(true, true);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, true);
}

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_Archives, ExplorerTestFixture ) {
  Construct(true, false);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, true);
}

BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_Files, ExplorerTestFixture ) {
  Construct(false, true);

  manga::Book book(releaseFileSystem());
  book.setRoot(fs::FilePath("/path/to/", false));

  DoNextIterationTest(book, true);
}


BOOST_AUTO_TEST_SUITE_END()
}
