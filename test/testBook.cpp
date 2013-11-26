#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "book.h"
#include "image.h"

#include "testFileSystem.h"
#include "testImageDecoder.h"

namespace test
{
	// --log_level=test_suite --run_test=TestBook
	BOOST_AUTO_TEST_SUITE( TestBook )

	// --log_level=test_suite --run_test=TestBook/BookExplorer

	std::auto_ptr<TestFileSystem> ConstructTestFS()
	{
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

	bool CheckPathInArcEmpty(const std::vector<manga::PathToFile> &aCollect)
	{
		BOOST_FOREACH(const manga::PathToFile &aPath, aCollect)
		{
			if(!aPath.pathInArchive.empty())
				return false;
		}
		return true;
	}

	BOOST_AUTO_TEST_CASE( Explorer )
	{
		manga::BookExplorer tExplorer(ConstructTestFS().release());
		BOOST_CHECK(tExplorer.setRoot(fs::FilePath("/abc/def/", false)));

		std::vector<manga::PathToFile> tRootList = tExplorer.fileList();
		BOOST_REQUIRE_EQUAL(tRootList.size(), 6);
		BOOST_CHECK_EQUAL(tRootList[0].filePath, fs::FilePath("/abc/def/folder1", false));
		BOOST_CHECK_EQUAL(tRootList[1].filePath, fs::FilePath("/abc/def/folder2", false));
		BOOST_CHECK_EQUAL(tRootList[2].filePath, fs::FilePath("/abc/def/folder3", false));
		BOOST_CHECK_EQUAL(tRootList[3].filePath, fs::FilePath("/abc/def/folder4", false));
		BOOST_CHECK_EQUAL(tRootList[4].filePath, fs::FilePath("/abc/def/folder5", false));
		BOOST_CHECK_EQUAL(tRootList[5].filePath, fs::FilePath("/abc/def/file7.jpg", true));
		BOOST_CHECK(CheckPathInArcEmpty(tRootList));

		// folder 1
		BOOST_REQUIRE(tExplorer.enter(tRootList[0]));
		std::vector<manga::PathToFile> tFolder1List = tExplorer.fileList();
		BOOST_REQUIRE_EQUAL(tFolder1List.size(), 3);
		BOOST_CHECK_EQUAL(tFolder1List[0].filePath, fs::FilePath("/abc/def/folder1/file1.jpg", true));
		BOOST_CHECK_EQUAL(tFolder1List[1].filePath, fs::FilePath("/abc/def/folder1/file2.jpg", true));
		BOOST_CHECK_EQUAL(tFolder1List[2].filePath, fs::FilePath("/abc/def/folder1/file3.jpg", true));
		BOOST_CHECK(CheckPathInArcEmpty(tFolder1List));

		// back to root
		BOOST_CHECK(tExplorer.back());
		std::vector<manga::PathToFile> tRootList2 = tExplorer.fileList();
		BOOST_CHECK_EQUAL_COLLECTIONS(
			tRootList.begin(), tRootList.end(),
			tRootList2.begin(), tRootList2.end());

		// folder 4
		BOOST_REQUIRE(tExplorer.enter(tRootList[3]));
		std::vector<manga::PathToFile> tFolder4List = tExplorer.fileList();
		BOOST_CHECK(tFolder4List.empty());
		BOOST_CHECK(CheckPathInArcEmpty(tFolder4List));

		// folder 5
		BOOST_REQUIRE(tExplorer.enter(tRootList[4]));
		std::vector<manga::PathToFile> tFolder5List = tExplorer.fileList();
		BOOST_REQUIRE_EQUAL(tFolder5List.size(), 1);
		BOOST_CHECK_EQUAL(tFolder5List[0].filePath, fs::FilePath("/abc/def/folder5/subfolder1", false));
		BOOST_CHECK(CheckPathInArcEmpty(tFolder5List));

		BOOST_CHECK(!tExplorer.enter(fs::FilePath("/abcd", false)));
		BOOST_CHECK(!tExplorer.enter(fs::FilePath("/abc/defg", false)));
	}

	void CheckArchPath(const std::vector<manga::PathToFile> &aCollect)
	{
		BOOST_FOREACH(const manga::PathToFile &aPath, aCollect)
			BOOST_CHECK_EQUAL(aPath.filePath, fs::FilePath("/path/to/archive/archive1.testarch", true));
	}

	BOOST_AUTO_TEST_CASE( ExplorerWithArchive )
	{
		std::auto_ptr<TestFileSystem> tfs(new TestFileSystem);
		TestArchiver tArchiver(tfs.get());

		TestArchive tArchive1("archive1.testarch");
		tArchive1.addFile("file1.jpg");
		tArchive1.addFile("folder1/file1.jpg");
		tArchive1.addFile("folder1/file2.jpg");
		tArchive1.addFile("folder1/file3.jpg");
		tArchive1.addFile("folder2/subfolder/file4.jpg");
		tArchive1.addFile("folder2/subfolder/file5.jpg");
		tArchive1.addFolder("folder3");

		tfs->addArchive("/path/to/archive", tArchive1);
		manga::BookExplorer tExplorer(tfs.release());
		BOOST_CHECK(tExplorer.setRoot(fs::FilePath("/path/to/", false)));
		BOOST_CHECK(tExplorer.enter(
			manga::PathToFile(
				fs::FilePath("/path/to/archive/archive1.testarch", true))));

		std::vector<manga::PathToFile> tRootList = tExplorer.fileList();
		BOOST_REQUIRE_EQUAL(tRootList.size(), 4);
		BOOST_CHECK_EQUAL(tRootList[0].pathInArchive, fs::FilePath("/folder1", false));
		BOOST_CHECK_EQUAL(tRootList[1].pathInArchive, fs::FilePath("/folder2", false));
		BOOST_CHECK_EQUAL(tRootList[2].pathInArchive, fs::FilePath("/folder3", false));
		BOOST_CHECK_EQUAL(tRootList[3].pathInArchive, fs::FilePath("file1.jpg", true));
		CheckArchPath(tRootList);

		// folder1
		BOOST_CHECK(tExplorer.enter(tRootList[0]));
		std::vector<manga::PathToFile> tFolder1List = tExplorer.fileList();
		BOOST_CHECK_EQUAL(tFolder1List[0].pathInArchive, fs::FilePath("/folder1/file1.jpg", true));
		BOOST_CHECK_EQUAL(tFolder1List[1].pathInArchive, fs::FilePath("/folder1/file2.jpg", true));
		BOOST_CHECK_EQUAL(tFolder1List[2].pathInArchive, fs::FilePath("/folder1/file3.jpg", true));
		CheckArchPath(tFolder1List);

		// back
		BOOST_CHECK(tExplorer.back());
		std::vector<manga::PathToFile> tRootList2 = tExplorer.fileList();
		BOOST_CHECK_EQUAL_COLLECTIONS(
			tRootList.begin(), tRootList.end(),
			tRootList2.begin(), tRootList2.end());

		// folder2
		BOOST_CHECK(tExplorer.enter(tRootList[1]));
		std::vector<manga::PathToFile> tFolder2List = tExplorer.fileList();	
		BOOST_REQUIRE_EQUAL(tFolder2List.size(), 1);
		BOOST_CHECK_EQUAL(tFolder2List[0].pathInArchive, fs::FilePath("folder2/subfolder", false));
		CheckArchPath(tFolder2List);

		// back
		BOOST_CHECK(tExplorer.back());
		// back and out of archive
		BOOST_CHECK(tExplorer.back());
		std::vector<manga::PathToFile> tArchList = tExplorer.fileList();
		BOOST_REQUIRE_EQUAL(tArchList.size(), 1);
		BOOST_CHECK_EQUAL(tArchList[0].filePath, fs::FilePath("/path/to/archive/archive1.testarch", true));
		BOOST_CHECK(CheckPathInArcEmpty(tArchList));
	}

	class ExplorerTestFixture
	{
	public:
		ExplorerTestFixture()
			:	mFileSystem(new TestFileSystem), 
				mArchiver(mFileSystem.get())
		{
		}

		void addFile(const std::string &aPath, const std::string &aImageData)
		{
			commitArchive();
			mFileSystem->addFile(aPath, CreateTestImage(aImageData));
			mIterFiles.push_back(manga::PathToFile(fs::FilePath(aPath, true)));

			if( !aImageData.empty() )
				mIterImages.push_back(aImageData);
		}

		void addFolder(const std::string &aPath, bool aIsFirstFile = false)
		{
			commitArchive();
			mFileSystem->addFolder(aPath);
		}


		void addArchive(const std::string &aPath)
		{
			commitArchive();
			mCurrArchivePath = fs::FilePath(aPath, true);
			mCurrentArchive = TestArchive(mCurrArchivePath.getFileName());
		}

		void addFileToArchive(const std::string &aPath, const std::string &aImageData)
		{
			BOOST_REQUIRE(!mCurrentArchive.empty());
			mCurrentArchive.addFile(aPath, CreateTestImage(aImageData));
			mIterFiles.push_back(manga::PathToFile(mCurrArchivePath, fs::FilePath(aPath, true)));

			if( !aImageData.empty() )
				mIterImages.push_back(aImageData);
		}

		void addFolderToArchive(const std::string &aPath, bool aIsFirstFile = false)
		{
			BOOST_REQUIRE(!mCurrentArchive.empty());
			mCurrentArchive.addFolder("folder3");
		}

		TestFileSystem *releaseFileSystem()
		{
			return mFileSystem.release();
		}

		void DoNextIterationTest(manga::Book &aBook, bool aWithPreload)
		{
			BOOST_REQUIRE(aBook.toFirstFile());
			size_t i = 0;

			do
			{
				if( aWithPreload )
				{
					aBook.preload();
				}

				BOOST_REQUIRE_LT(i, mIterImages.size());
				BOOST_CHECK_EQUAL(mIterImages[i++], DataFromTestImage(aBook.currentImage()));
			} while (aBook.incrementPosition());

			BOOST_CHECK_EQUAL(i, mIterImages.size());
		}

		void DoNextIterationTest(manga::BookExplorer &aExplorer)
		{
			BOOST_REQUIRE(aExplorer.toFirstFile());
			size_t i = 0;
			do 
			{
				BOOST_REQUIRE_LT(i, mIterFiles.size());
				BOOST_CHECK_EQUAL(mIterFiles[i++], aExplorer.getCurrentPos());
			} while ( aExplorer.toNextFile() );
			BOOST_CHECK_EQUAL(i, mIterFiles.size());
		}

		void DoPreviousIterationTest(manga::BookExplorer &aExplorer)
		{
			BOOST_REQUIRE(aExplorer.toLastFile());
			size_t i = mIterFiles.size();
			do 
			{
				BOOST_REQUIRE_GT(i, 0U);
				BOOST_CHECK_EQUAL(mIterFiles[--i], aExplorer.getCurrentPos());
			} while (aExplorer.toPreviousFile());
			BOOST_CHECK_EQUAL(i, 0);
		}

		void Construct(bool aWithArchive, bool aWithFiles)
		{
			addFolder("/path/to/dir_00/");
			addFolder("/path/to/dir_1/subdir 1");
			addFolder("/path/to/dir_1/subdir 2");
			addFolder("/path/to/dir_1/subdir 3/sub sub dir");

			if( aWithFiles )
			{
				addFile("/path/to/dir_2/file1.testimg", "Image File 1");
				addFile("/path/to/dir_5/file2.testimg", "Image File 2");
				addFile("/path/to/dir_10/file3.testimg", "Image File 3");
			}

			if( aWithArchive )
			{
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

			if( aWithFiles )
			{
				addFile("/path/to/dir_100/subdir 001/file1.testimg", "Image File 10");
				addFile("/path/to/dir_100/subdir 02/file2.testimg", "Image File 11");
				addFile("/path/to/dir_100/subdir 100/file3.testimg", "Image File 12");
				addFile("/path/to/dir_100/subdir 100/file4.testimg", "Image File 13");
			}

			if( aWithArchive )
			{
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
		void commitArchive()
		{
			if(!mCurrentArchive.empty())
			{
				mFileSystem->addArchive(mCurrArchivePath.copy(mCurrArchivePath.getDirLevel() - 1).getPath(), mCurrentArchive);
				mCurrArchivePath = fs::FilePath();
				mCurrentArchive = TestArchive();
			}
		}
		std::auto_ptr<TestFileSystem> mFileSystem;

		TestArchiver mArchiver;
		TestArchive mCurrentArchive;
		fs::FilePath mCurrArchivePath;
		TestImageDecoder mImageDecoder;

		std::vector<manga::PathToFile> mIterFiles;
		std::vector<std::string> mIterImages;
	};

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir_NoArchives, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir_NoArchives, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir_ArchivesOnly, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir_ArchivesOnly, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_FileAndDir, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_FileAndDir, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}


	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File_NoArchives, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File_NoArchives, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File_ArchivesOnly, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File_ArchivesOnly, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNext_File, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIteratePrevious_File, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		DoPreviousIterationTest(tExplorer);
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNothing_FileAndDir, ExplorerTestFixture )
	{
		Construct(false, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::FileAndDirectory);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		BOOST_CHECK(!tExplorer.toFirstFile());
		BOOST_CHECK(!tExplorer.toLastFile());
	}

	BOOST_FIXTURE_TEST_CASE( ExplorerIterateNothing_File, ExplorerTestFixture )
	{
		Construct(false, false);

		manga::BookExplorer tExplorer(releaseFileSystem(), fs::IFileManager::File);

		tExplorer.setRoot(fs::FilePath("/path/to/", false));

		BOOST_CHECK(!tExplorer.toFirstFile());
		BOOST_CHECK(!tExplorer.toLastFile());
	}


	//////////////////////////////////////////////////////////////////////////////////////

	BOOST_AUTO_TEST_CASE( SmokeToolsTest )
	{
		TestImageDecoder tDecoder;
		tools::ByteArray tArray = CreateTestImage("Test data");
		img::Image tImage;
		BOOST_CHECK(tDecoder.decode(tArray, tImage));
		BOOST_CHECK_EQUAL(DataFromTestImage(tImage), "Test data");
		BOOST_CHECK(!tDecoder.decode(tools::toByteArray("Wrong image data|Wrong image data|Wrong image data"), tImage));
	}

	std::vector<fs::FilePath> CallFixUp(
		const char *aRoot,
		const char *aPath1, bool aIsFile1,
		const char *aPath2, bool aIsFile2)
	{
		std::vector<fs::FilePath> tPaths;

		if(aPath1)
			tPaths.push_back(fs::FilePath(aPath1, aIsFile1));

		if(aPath2)
			tPaths.push_back(fs::FilePath(aPath2, aIsFile2));

		manga::FixUpFileTreeForTest(tPaths, fs::FilePath(aRoot, false));
		return tPaths;
	}

	void DoCheckFixUp(
		const std::vector<fs::FilePath> &aFiles,
		const char *aPath1 = 0,
		const char *aPath2 = 0,
		const char *aPath3 = 0,
		const char *aPath4 = 0,
		const char *aPath5 = 0)
	{
		size_t tCount = 0;
		if( aPath1 )
			BOOST_CHECK_EQUAL(aFiles[tCount++].getPath(), aPath1);
		if( aPath2 )
			BOOST_CHECK_EQUAL(aFiles[tCount++].getPath(), aPath2);
		if( aPath3 )
			BOOST_CHECK_EQUAL(aFiles[tCount++].getPath(), aPath3);
		if( aPath4 )
			BOOST_CHECK_EQUAL(aFiles[tCount++].getPath(), aPath4);
		if( aPath5 )
			BOOST_CHECK_EQUAL(aFiles[tCount++].getPath(), aPath5);

		BOOST_CHECK_EQUAL(tCount, aFiles.size());
	}

	// --log_level=test_suite --run_test=TestBook/TestFixingUp
	BOOST_AUTO_TEST_CASE( TestFixingUp )
	{
		fs::FilePath::setGlobalSelector('/');
		std::vector<fs::FilePath> tList1 = CallFixUp("/abc/def", "/abc/def/folder1", false, "/abc/def/folder1/folder2/file1", true);
		DoCheckFixUp(tList1, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/folder2", "/abc/def/folder1/folder2/file1");

		std::vector<fs::FilePath> tList2 = CallFixUp("/abc/def", "/abc/def/folder1/file1", true, "/abc/def/folder2/file2", true);
		DoCheckFixUp(tList2, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/file1", "/abc/def/folder2", "/abc/def/folder2/file2");

		std::vector<fs::FilePath> tList3 = CallFixUp("/abc/def", "/abc/def/folder1/folder2/file1", true, "/abc/def/folder1/file2", true);
		DoCheckFixUp(tList3, "/abc/def", "/abc/def/folder1", "/abc/def/folder1/folder2", "/abc/def/folder1/folder2/file1", "/abc/def/folder1/file2");

		std::vector<fs::FilePath> tList4 = CallFixUp("/abc/def", "/abc/def/folder1", false, "/abc/def/folder2/folder3/file2", true);
		DoCheckFixUp(tList4, "/abc/def", "/abc/def/folder1", "/abc/def/folder2", "/abc/def/folder2/folder3", "/abc/def/folder2/folder3/file2");
	}

	//////////////////////////////////////////////////////////////////////////

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_ArchivesFiles, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, false);
	}

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Archives, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, false);
	}

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Files, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, false);
	}

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_ArchivesFiles, ExplorerTestFixture )
	{
		Construct(true, true);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, true);
	}

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_Archives, ExplorerTestFixture )
	{
		Construct(true, false);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, true);
	}

	BOOST_FIXTURE_TEST_CASE( BookIterate_Next_Preload_Files, ExplorerTestFixture )
	{
		Construct(false, true);

		manga::Book tBook(releaseFileSystem());
		tBook.setRoot(fs::FilePath("/path/to/", false));

		DoNextIterationTest(tBook, true);
	}
	

	BOOST_AUTO_TEST_SUITE_END()
}
