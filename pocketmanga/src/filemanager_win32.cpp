#include "filemanager.h"

#ifdef WIN32

#include "filepath.h"
#include <Windows.h>

namespace fs
{
	namespace w32
	{
		class FileManager32: public IFileManager
		{
			void appendFileName(std::string &aParent, const std::string &aFile)
			{
				if( aParent.empty() )
					aParent = aFile;

				if( aParent[aParent.size() - 1] != '\\' && aParent[aParent.size() - 1] != '/' )
					aParent.append("\\");

				aParent.append(aFile);
			}

			void doGetFileList(std::vector<fs::FilePath> &aList, const std::string &aRoot, EntryTypes aEntries, bool aRecursive)
			{
				HANDLE hFind = INVALID_HANDLE_VALUE;

				WIN32_FIND_DATAA ffd = {0};

				std::string tRoot = aRoot;
				appendFileName(tRoot, "*");
				hFind = FindFirstFileA(tRoot.c_str(), &ffd);
				if (INVALID_HANDLE_VALUE == hFind) 
					return;

				do
				{
					const bool tIsDir = (FILE_ATTRIBUTE_DIRECTORY == (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));

					std::string tFileName = ffd.cFileName;
					if( tFileName == "." || tFileName == "..")
						continue;

					std::string tFullPath = aRoot;
					appendFileName(tFullPath, ffd.cFileName);
					if( tIsDir )
						tFullPath += "\\";

					fs::FilePath tPath(tFullPath, !tIsDir );	

					if( (aEntries&IFileManager::Directory) && tIsDir )
						aList.push_back(tPath);

					if( (aEntries&IFileManager::File) && !tIsDir )
						aList.push_back(tPath);

					if( aRecursive && tIsDir )
						doGetFileList(aList, tPath.getPath(), aEntries, aRecursive);
				}
				while (FindNextFileA(hFind, &ffd) != 0);
			}

			virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &aRoot, EntryTypes aEntries, bool aRecursive)
			{
				std::vector<fs::FilePath> tResult;

				doGetFileList(tResult, aRoot.getPath(), aEntries, aRecursive);

				return tResult;
			}
		};
	}


	IFileManager *IFileManager::create()
	{
		return new w32::FileManager32;
	}
}

#endif
