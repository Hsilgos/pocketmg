#pragma once

namespace utils {
inline bool isDebugging() {
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}
}