#pragma once


#include <vector>
#include <memory>
#include "book.h"


namespace pocket {

class IBookListHandler {
public:
  virtual ~IBookListHandler();

  // you have to delete book object yourself
  virtual void startShow( std::auto_ptr<manga::Book> book) = 0;
  virtual void addNewBook() = 0;
  virtual void onExit() = 0;
};

class IBooklistCfg {
public:
  virtual ~IBooklistCfg();

  virtual std::vector<fs::FilePath> getBooks() = 0;
};

void showBooklist(IBookListHandler *handler, IBooklistCfg *cfg);
}

