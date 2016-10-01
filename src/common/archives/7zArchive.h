
#include <memory>

namespace archive {
class IArchive;
std::auto_ptr<IArchive> create7ZipArchiver();
}
