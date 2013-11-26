#include "filemanager.h"


#ifdef __unix__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "filepath.h"

namespace fs
{
	namespace lin
	{
		class FileManagerLin: public IFileManager
		{
			void appendFileName(std::string &aParent, const std::string &aFile)
			{
				if( aParent.empty() )
					aParent = aFile;

				if( aParent[aParent.size() - 1] != '\\' && aParent[aParent.size() - 1] != '/' )
					aParent.append("/");

				aParent.append(aFile);
			}

			void doGetFileList(std::vector<fs::FilePath> &aResult , const std::string &aRoot, EntryTypes aEntries, bool aRecursive)
			{
				struct dirent *dp;
				DIR *dir = opendir(aRoot.c_str());
				if( !dir )
					return;

				while ((dp=readdir(dir)) != NULL)
				{
					std::string tFullPath = aRoot;
					const std::string tName = dp->d_name;

					const bool tIsDir = (dp->d_type == DT_DIR);

					if( !tName.empty() && tName != "." && tName != ".." )
					{
						appendFileName(tFullPath, tName);

						fs::FilePath tPath(tFullPath, !tIsDir );

						if( (aEntries & IFileManager::File) && !tIsDir )
							aResult.push_back(tPath);

						if( (aEntries & IFileManager::Directory) && tIsDir )
							aResult.push_back(tPath);

						if( aRecursive && tIsDir )
							doGetFileList(aResult, tFullPath, aEntries, aRecursive);

					};
				}
				closedir(dir);
			}

			virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &aRoot, EntryTypes aEntries, bool aRecursive)
			{
				std::vector<fs::FilePath> tResult;
				doGetFileList(tResult ,aRoot.getPath(), aEntries, aRecursive);
				return tResult;
			}
		};
	}

	IFileManager *IFileManager::create()
	{
		return new lin::FileManagerLin;
	}
}
#endif
