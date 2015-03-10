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
			void appendFileName(std::string &parent, const std::string &file)
			{
				if( parent.empty() )
					parent = file;

				if( parent[parent.size() - 1] != '\\' && parent[parent.size() - 1] != '/' )
					parent.append("/");

				parent.append(file);
			}

			void doGetFileList(std::vector<fs::FilePath> &result , const std::string &root, EntryTypes entries, bool recursive)
			{
				struct dirent *dp;
				DIR *dir = opendir(root.c_str());
				if( !dir )
					return;

				while ((dp=readdir(dir)) != NULL)
				{
					std::string full_path = root;
					const std::string name = dp->d_name;

					const bool is_dir = (dp->d_type == DT_DIR);

					if( !name.empty() && name != "." && name != ".." )
					{
						appendFileName(full_path, name);

						fs::FilePath path(full_path, !is_dir );

						if( (entries & IFileManager::File) && !is_dir )
							result.push_back(path);

						if( (entries & IFileManager::Directory) && is_dir )
							result.push_back(path);

						if( recursive && is_dir )
							doGetFileList(result, full_path, entries, recursive);

					};
				}
				closedir(dir);
			}

			virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &root, EntryTypes entries, bool recursive)
			{
				std::vector<fs::FilePath> result;
				doGetFileList(result ,root.getPath(), entries, recursive);
				return result;
			}
		};
	}

	IFileManager *IFileManager::create()
	{
		return new lin::FileManagerLin;
	}
}
#endif
