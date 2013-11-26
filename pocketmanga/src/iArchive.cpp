#include "iArchive.h"

#include "zipArchive.h"

#include <map>

namespace archive
{
	class ArchiveFactory
	{
		typedef std::map<std::string, IArchive::FactoryMethod> Archivers;
		Archivers mArchivers;
	public:
		static ArchiveFactory &instance()
		{
			static ArchiveFactory tInst;
			return tInst;
		}

		void registerArchiver(const std::string &aPrefExt, IArchive::FactoryMethod aMethod)
		{
			mArchivers[aPrefExt] = aMethod;
		}

		void unregisterArchiver(const std::string &aPrefExt)
		{
			mArchivers.erase(aPrefExt);
		}

		IArchive *recognize(const fs::FilePath &aPath) const
		{
			if( aPath.isDirectory() )
				return 0;

			const std::string tExt = aPath.getExtension();

			std::auto_ptr<IArchive> tConcArch;

			Archivers::const_iterator itExt = mArchivers.find(tExt);
			if( itExt != mArchivers.end() )
			{
				tConcArch.reset( itExt->second() );
				if( tConcArch->open(aPath.getPath()) )
					return tConcArch.release();
			}

			for( 
				Archivers::const_iterator it = mArchivers.begin(), itEnd = mArchivers.end(); it != itEnd; ++it )
			{
				tConcArch.reset( it->second() );
				if( tConcArch->open(aPath.getPath()) )
					return tConcArch.release();
			}

			return 0;
		}
	};

	void IArchive::registerArchiver(const std::string &aPrefExt, IArchive::FactoryMethod aMethod)
	{
		ArchiveFactory::instance().registerArchiver(aPrefExt, aMethod);
	}

	void IArchive::unregisterArchiver(const std::string &aPrefExt)
	{
		ArchiveFactory::instance().unregisterArchiver(aPrefExt);
	}

	IArchive *recognize(const fs::FilePath &aPath)
	{
		return ArchiveFactory::instance().recognize(aPath);
	}
}
