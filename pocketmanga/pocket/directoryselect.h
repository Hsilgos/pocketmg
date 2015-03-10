#pragma once

namespace fs
{
	class FilePath;
}

namespace pocket
{
	class IDirectoryHandler
	{
	public:
		virtual ~IDirectoryHandler();
		virtual void selected(const fs::FilePath &path) = 0;
		virtual void onExit() = 0;
		//virtual std::vector<std::string> getRootDirectories();
	};

	void openDirectorySelector(IDirectoryHandler *selector);
}
