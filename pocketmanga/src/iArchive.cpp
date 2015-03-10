#include "iArchive.h"

#include "zipArchive.h"

#include <map>

namespace archive {
class ArchiveFactory {
  typedef std::map<std::string, IArchive::FactoryMethod> Archivers;
  Archivers archivers_;
public:
  static ArchiveFactory &instance() {
    static ArchiveFactory inst;
    return inst;
  }

  void registerArchiver(const std::string &pref_ext, IArchive::FactoryMethod method) {
    archivers_[pref_ext] = method;
  }

  void unregisterArchiver(const std::string &pref_ext) {
    archivers_.erase(pref_ext);
  }

  IArchive *recognize(const fs::FilePath &path) const {
    if( path.isDirectory() )
      return 0;

    const std::string ext = path.getExtension();

    std::auto_ptr<IArchive> conc_arch;

    Archivers::const_iterator itExt = archivers_.find(ext);
    if( itExt != archivers_.end() ) {
      conc_arch.reset( itExt->second() );
      if( conc_arch->open(path.getPath()) )
        return conc_arch.release();
    }

    for(
      Archivers::const_iterator it = archivers_.begin(), itEnd = archivers_.end(); it != itEnd; ++it ) {
      conc_arch.reset( it->second() );
      if( conc_arch->open(path.getPath()) )
        return conc_arch.release();
    }

    return 0;
  }
};

void IArchive::registerArchiver(const std::string &pref_ext, IArchive::FactoryMethod method) {
  ArchiveFactory::instance().registerArchiver(pref_ext, method);
}

void IArchive::unregisterArchiver(const std::string &pref_ext) {
  ArchiveFactory::instance().unregisterArchiver(pref_ext);
}

IArchive *recognize(const fs::FilePath &path) {
  return ArchiveFactory::instance().recognize(path);
}
}
