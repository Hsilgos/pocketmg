#pragma once

//#include "debugUtils.h"
//#include "smartDelete.h"

//#include <boost/thread/mutex.hpp>

namespace utils {
template<class T>
class SingletonStatic {
public:
  static T& getInstance() {
    static T *volatile t_InstancePtr = NULL;

    if( !t_InstancePtr ) {
      //static boost::mutex t_mutex;
      //boost::mutex::scoped_lock t_lock(t_mutex);

      if( !t_InstancePtr ) {
        //disableLeakDetecting();
        static T t_Instance;
        //enableLeakDetecting();
        t_InstancePtr = &t_Instance;
      }
    }

    return *t_InstancePtr;
  }
};
}
