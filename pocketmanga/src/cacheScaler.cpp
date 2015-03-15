#include "cacheScaler.h"

#include <assert.h>

#include "rotate.h"
#include "scale.h"
#include "debugUtils.h"

namespace manga {
IBookCache *CacheScaler::clone() {
  return new CacheScaler(screen_width_, screen_height_);
}

void CacheScaler::swap(IBookCache *other_cache) {
  CacheScaler *other =
    utils::isDebugging() ? dynamic_cast<CacheScaler *>(other_cache) : static_cast<CacheScaler *>(other_cache);

  assert(other);
  orig_.swap(other->orig_);
  scaled_.swap(other->scaled_);
}

CacheScaler::Cache::Cache() {
  image.enableMinimumReallocations(true);
}

void CacheScaler::Cache::swap(Cache &other) {
  std::swap(orientation, other.orientation);
  std::swap(representation, other.representation);
  std::swap(bounds, other.bounds);
  std::swap(currentShowing, other.currentShowing);
  image.swap(other.image);
}

bool CacheScaler::Cache::nextBounds() {
  if( Parts3 == representation ) {
    const int second_frame = (image.width() - bounds.width) / 2;
    if( bounds.x < second_frame ) {
      bounds.x = second_frame;
      return true;
    }

    const int third_frame = image.width() - bounds.width;
    if( bounds.x < third_frame ) {
      bounds.x = third_frame;
      return true;
    }
  }

  return false;
}

bool CacheScaler::Cache::previousBounds() {
  if( Parts3 == representation ) {
    const int second_frame = (image.width() - bounds.width) / 2;
    if( bounds.x > second_frame ) {
      bounds.x = second_frame;
      return true;
    }

    if( bounds.x > 0 ) {
      bounds.x = 0;
      return true;
    }
  }

  return false;
}

bool CacheScaler::onLoaded(img::Image &image) {
  img::toBgr(image, orig_.image);
  img::toGray(orig_.image, orig_.image);
  if( orig_.image.width() < screen_width_ && orig_.image.height() < screen_height_ ) {
    scaled_.representation = Whole;
    scaled_.orientation = Vertical;

    img::copy(orig_.image, scaled_.image);
  } else if( orig_.image.width() < screen_height_ && orig_.image.height() < screen_width_ ) {
    scaled_.representation = Whole;
    scaled_.orientation = Horizontal;

    img::rotate(orig_.image, scaled_.image, img::Angle_270);
  }
  if( orig_.image.width() <= orig_.image.height() ) {
    scaled_.representation = Parts3;
    scaled_.orientation = Vertical;

    img::scale(orig_.image, scaled_.image, img::HighScaling,  screen_height_, 0);
    img::rotate(scaled_.image, scaled_.image, img::Angle_270);

    scaled_.bounds.x = 0;
    scaled_.bounds.y = 0;
    scaled_.bounds.width  = screen_width_;
    scaled_.bounds.height  = screen_height_;
    scaled_.currentShowing = 0;
    /*drawable.representType = Parts3;
    drawable.orientation = Vertical;

    const double img_prop = static_cast<double>(image_.width())/static_cast<double>(image_.height());
    const double scr_prop = static_cast<double>(screen_width)/static_cast<double>(screen_height_);

    if( img_prop < scr_prop )
     img::scale(image_, drawable_.image, img::HighScaling,  0, screen_height_);
    else
     img::scale(image_, drawable_.image, img::HighScaling, screen_width, 0);

    drawable.currectBouds.x = 0;
    drawable.currectBouds.y = 0;
    drawable.currectBouds.width  = screen_width;
    drawable.currectBouds.height  = screen_height_;*/
  } else {
    scaled_.representation = Parts3;
    scaled_.orientation = Vertical;

    img::scale(orig_.image, scaled_.image, img::HighScaling,  0, screen_height_);
    //img::rotate(drawable_.image, drawable_.image, img::Angle_270);

    scaled_.bounds.x = 0;
    scaled_.bounds.y = 0;
    scaled_.bounds.width  = screen_width_;
    scaled_.bounds.height  = screen_height_;
    scaled_.currentShowing = 0;
    /*drawable.representType = Parts3;
    drawable.orientation = Vertical;

    const double img_prop = static_cast<double>(image_.height())/static_cast<double>(image_.width());
    const double scr_prop = static_cast<double>(screen_width)/static_cast<double>(screen_height_);

    if( img_prop < scr_prop )
     img::scale(image_, drawable_.image, img::HighScaling, screen_height_, 0);
    else
     img::scale(image_, drawable_.image, img::HighScaling, 0, screen_width);

    img::rotate(drawable_.image, drawable_.image, img::Angle_270);
    drawable.currectBouds.x = 0;
    drawable.currectBouds.y = 0;
    drawable.currectBouds.width  = screen_width;
    drawable.currectBouds.height  = screen_height_;*/
  }

  return true;
}

CacheScaler::Cache &CacheScaler::scaledGrey() {
  return scaled_;
}

CacheScaler::CacheScaler(const size_t screen_width, const size_t screen_height)
  :screen_width_(screen_width), screen_height_(screen_height) {
}
}

