#pragma once

#include <memory>

#include "inkview.h"

#include "singleton.h"
#include "image.h"

namespace manga {
class Book;
class CacheScaler;
}

namespace pocket {
class PictureView: public utils::SingletonStatic<PictureView> {
  std::auto_ptr<manga::Book> book_;

  manga::CacheScaler *scaler_;
public:
  void setBook( std::auto_ptr<manga::Book> book);
  bool next();
  bool previous();



  void draw();
  void draw(manga::CacheScaler *scaler);
};
}
