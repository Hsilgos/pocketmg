#include <boost/test/unit_test.hpp>

#include "testFileSystem.h"

#include "filepath.h"
#include "defines.h"

namespace test
{
	// --log_level=test_suite --run_test=TestFilePath
	BOOST_AUTO_TEST_SUITE( TestFilePath )

	BOOST_AUTO_TEST_CASE( TestsForTestFileSystem )
	{
		TestFileSystem tfs;
		tfs.addFolder("/abc/def");
		tfs.addFile("/ghi/jkl/nmo.txt", tools::toByteArray("nmo file content"));

		TestArchive arch("archive.testarch");
		arch.addFile("test1.txt");
		arch.addFile("test2.txt");
		tfs.addArchive("/path/to/archive", arch);

		BOOST_CHECK( !tfs.findDirByPath("/not/existing/path", true) );
		BOOST_CHECK( !tfs.findDirByPath("/not/existing/path", false) );

		BOOST_CHECK( tfs.findDirByPath("/abc/def", false) );
		BOOST_CHECK( tfs.findDirByPath("/abc/def", true) );
		BOOST_CHECK( tfs.findDirByPath("/ghi/jkl", false) );
		BOOST_CHECK( tfs.findDirByPath("/ghi/jkl", true) );

		BOOST_CHECK_EQUAL( tfs.findDirByPath("/ghi/jkl", false)->files.size(), 1 );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/ghi/jkl", false)->files[0].name, "nmo.txt" );
		BOOST_CHECK_EQUAL(
			tools::byteArray2string(tfs.readFile(fs::FilePath("/ghi/jkl/nmo.txt", true), 1000)),
			"nmo file content" );

		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive", false)->archives.size(), 1 );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive/archive.testarch", true)->archives.size(), 1 );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive", false)->files.size(), 1 );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive/archive.testarch", true)->files.size(), 1 );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive", false)->files[0].name, "archive.testarch" );
		BOOST_CHECK_EQUAL( tfs.findDirByPath("/path/to/archive/archive.testarch", true)->files[0].name, "archive.testarch" );
	}

	BOOST_AUTO_TEST_CASE( TestsForTestArchive )
	{
		std::auto_ptr<TestFileSystem> tfs(new TestFileSystem);
		TestArchiver archiver(tfs.get());

		TestArchive arch("archive.testarch");
		arch.addFile("test1.txt");
		arch.addFile("test2.txt", tools::toByteArray("archive file 2 content"));
		arch.addFile("test3.txt");

		tfs->addArchive("/path/to/archive", arch);

		BOOST_CHECK( !archiver.open("/some/path/archive.rar") );
		BOOST_CHECK( !archiver.open("/path/to/archive/archive.rar") );
		BOOST_CHECK( archiver.open("/path/to/archive/archive.testarch") );
		std::vector<fs::FilePath> files_in_arch = archiver.getFileList(true);
		BOOST_CHECK_EQUAL(files_in_arch.size(), 3);
		std::sort(files_in_arch.begin(), files_in_arch.end());
		BOOST_CHECK_EQUAL(files_in_arch[0].getPath(), "test1.txt");
		BOOST_CHECK_EQUAL(files_in_arch[1].getPath(), "test2.txt");
		BOOST_CHECK_EQUAL(files_in_arch[2].getPath(), "test3.txt");

		BOOST_CHECK_EQUAL(
			tools::byteArray2string(archiver.getFile(fs::FilePath("test2.txt", true), 1000)), 
			"archive file 2 content");
	}

	void checkPath3(const std::string &path )
	{
		fs::FilePath tfn1(path, true);

		BOOST_CHECK( !tfn1.empty() );
		BOOST_CHECK_EQUAL( tfn1.getLevel(), 3 );
		BOOST_CHECK_EQUAL( tfn1.getPath(), path );

		BOOST_CHECK_EQUAL( tfn1.getName(0), "abc" );
		BOOST_CHECK_EQUAL( tfn1.getName(1), "def" );
		BOOST_CHECK_EQUAL( tfn1.getName(2), "ghi.jpg" );
		BOOST_CHECK_EQUAL( tfn1.getName(4), utils::EmptyString );

		BOOST_CHECK( !tfn1.isDirectory() );
		BOOST_CHECK( tfn1.isDirectory(0) );
		BOOST_CHECK( tfn1.isDirectory(1) );
		BOOST_CHECK( !tfn1.isDirectory(2) );
	}

	BOOST_AUTO_TEST_CASE( Parse1 )
	{
		checkPath3("./abc/def/ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse2 )
	{
		checkPath3(".//abc/def/ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse3 )
	{
		checkPath3("abc/def/ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse4 )
	{
		checkPath3("abc\\def\\ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse5 )
	{
		checkPath3("\\abc\\def\\ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse6 )
	{
		checkPath3(".\\abc\\def\\ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse7 )
	{
		checkPath3("/.//abc/def/ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse8 )
	{
		checkPath3(".//.//abc/def/ghi.jpg");
	}

	BOOST_AUTO_TEST_CASE( Parse9 )
	{
		fs::FilePath tfn1("filename.txt", true);

		BOOST_CHECK_EQUAL( tfn1.getLevel(), 1 );
		BOOST_CHECK_EQUAL( tfn1.getPath(), "filename.txt" );
		BOOST_CHECK_EQUAL( tfn1.getName(0), "filename.txt" );
		BOOST_CHECK( !tfn1.isDirectory() );
	}

	BOOST_AUTO_TEST_CASE( Parse10 )
	{
		fs::FilePath tfn1("/", false);

		BOOST_CHECK( tfn1.empty() );
		BOOST_CHECK_EQUAL( tfn1.getLevel(), 0 );
		BOOST_CHECK( tfn1.isDirectory() );
	}

	BOOST_AUTO_TEST_CASE( Parse11 )
	{
		fs::FilePath tfn1(utils::EmptyString, false);

		BOOST_CHECK( tfn1.empty() );
		BOOST_CHECK_EQUAL( tfn1.getLevel(), 0 );
		BOOST_CHECK( tfn1.isDirectory() );
	}

	BOOST_AUTO_TEST_CASE( Comparing )
	{
		BOOST_CHECK(fs::FilePath("/abc/def/ghi", false) == fs::FilePath("/abc/def/ghi", false) );

		BOOST_CHECK(fs::FilePath("/abc/def/ghi", false) == fs::FilePath("\\abc\\def\\ghi", false) );

		BOOST_CHECK(fs::FilePath("/abc//def/ghi", false) == fs::FilePath("/abc/def/ghi", false) );

		BOOST_CHECK(fs::FilePath("/abc/def", false) < fs::FilePath("/abc/def/ghi", false));

		BOOST_CHECK(fs::FilePath("/abc/def/gha", false) < fs::FilePath("/abc/def/ghi", false));
	}

	BOOST_AUTO_TEST_CASE( SameDirectory )
	{
		BOOST_CHECK(HaveSameDirectory(
			fs::FilePath("/smoke", false),
			fs::FilePath("/smoke", false)));

		BOOST_CHECK(HaveSameDirectory(
			fs::FilePath("/smoke/dir", false),
			fs::FilePath("/smoke/dir", false)));

		BOOST_CHECK(HaveSameDirectory(
			fs::FilePath("/some/dir/file1", true),
			fs::FilePath("/some/dir", false)));

		BOOST_CHECK(HaveSameDirectory(
			fs::FilePath("/some/dir", false),
			fs::FilePath("/some/dir/file2", true)));

		BOOST_CHECK(HaveSameDirectory(
			fs::FilePath("/some/dir/file1.jpg", true),
			fs::FilePath("/some/dir/file2.png", true)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir1", false),
			fs::FilePath("/some/dir2", false)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir1/file1", true),
			fs::FilePath("/some/dir2", false)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir1", false),
			fs::FilePath("/some/dir2/file2", true)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir1/file1", true),
			fs::FilePath("/some/dir2/file2", true)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir/subdir/file1", true),
			fs::FilePath("/some/dir/file2", true)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir/subdir", false),
			fs::FilePath("/some/dir/file2", true)));

		BOOST_CHECK(!HaveSameDirectory(
			fs::FilePath("/some/dir/subdir", false),
			fs::FilePath("/some/dir", false)));
	}

	bool CheckStartsWith(const char *path1, const char *path2, size_t level, bool from_back)
	{
		fs::FilePath tfpath1(path1, false);
		fs::FilePath tfpath2(path2, false);
		return tfpath1.startsWith(tfpath2, level, from_back);
	}

	BOOST_AUTO_TEST_CASE( StartsWith )
	{
		BOOST_CHECK(CheckStartsWith("/abc/def", "/abc/def/gh", 2, false));
		BOOST_CHECK(CheckStartsWith("/abc/def", "/abc/def/gh", 1, false));
		BOOST_CHECK(CheckStartsWith("/abc/def", "/abc/def/gh", 2, true));
		BOOST_CHECK(CheckStartsWith("/abc/def", "/abc/def/gh", 1, true));

		BOOST_CHECK(!CheckStartsWith("/abc/def1", "/abc/def2/gh", 2, false));
		BOOST_CHECK( CheckStartsWith("/abc/def1", "/abc/def2/gh", 1, false));
		BOOST_CHECK(!CheckStartsWith("/abc/def1", "/abc/def2/gh", 2, true));
		BOOST_CHECK( CheckStartsWith("/abc/def1", "/abc/def2/gh", 1, true));
	}

	fs::FilePath CheckCommonPath(
		const char *path1, bool is_file1,
		const char *path2, bool is_file2)
	{
		fs::FilePath tfpath1(path1, is_file1);
		fs::FilePath tfpath2(path2, is_file2);

		return FindCommonPath(tfpath1, tfpath2);
	}

	BOOST_AUTO_TEST_CASE( CommonPath )
	{
		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def", false, "/abc/def", false),
			fs::FilePath("/abc/def", false));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def/file", true, "/abc/def/file", true),
			fs::FilePath("/abc/def/file", true));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def", false, "/abc/def/file", true),
			fs::FilePath("/abc/def", false));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def/file", true, "/abc/def", false),
			fs::FilePath("/abc/def", false));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def/folder", false, "/abc/def", false),
			fs::FilePath("/abc/def", false));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/abc/def/folder1/folder2", false, "/abc/def/folder3/folder4", false),
			fs::FilePath("/abc/def", false));

		BOOST_CHECK_EQUAL(
			CheckCommonPath("/f1/f2", false, "/f3/f4/5", false),
			fs::FilePath());
	}

	BOOST_AUTO_TEST_SUITE_END()
}
