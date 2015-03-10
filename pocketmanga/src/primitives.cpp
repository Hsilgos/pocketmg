#include "primitives.h"

Position::Position()
  :x(0),y(0) {

}

Position::Position(int ax, int ay)
  :x(ax),y(ay) {
}

void Position::clean() {
  x = 0;
  y = 0;
}

//////////////


Rect::Rect()
  :x(0), y(0), width(0), height(0) {
}

Rect::Rect(int ax, int ay, int width, int height)
  :x(ax), y(ay), width(width), height(height) {
}

void Rect::clear() {
  x = 0;
  y = 0;
  width = 0;
  height = 0;
}


