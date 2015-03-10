// pocketmanga.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <sstream>

#include "src/iArchive.h"
#include "src/zipArchive.h"
#include "src/filepath.h"
#include "src/filemanager.h"
#include "src/image.h"
#include "src/book.h"

#include "pocket/directoryselect.h"
#include "pocket/booklist.h"
#include "pocket/config.h"
#include "pocket/pictureview.h"

#include "inkview.h"

namespace
{
#ifdef IVSAPP// emulator?
static const fs::FilePath ConfigPath("/usr/local/pocketbook/config/pocketmanga.cfg", true);
#else
static const fs::FilePath ConfigPath(USERDATA"/config/fbreader.cfg", true);
#endif
}
//#include <opencv2/opencv.hpp>

void mainscreen_repaint() {

  ClearScreen();

  DrawLine(5, 500, 595, 500, BLACK);
  DrawLine(5, 502, 595, 502, DGRAY);
  DrawLine(5, 504, 595, 504, LGRAY);
  DrawLine(19, 516, 20, 517, BLACK);
  DrawLine(22, 516, 23, 516, BLACK);

  //DrawTextRect(25, 400, 510, 350, sometext, ALIGN_LEFT);

  FullUpdate();

}

std::string bookCfgName(int i)
{
	std::stringstream name;
	name << "book_" << i;
	return name.str();
}

std::vector<fs::FilePath> loadBooks()
{
	std::vector<fs::FilePath> result;

	pocket::Config cfg(ConfigPath.getPath());
	const int count = cfg.readInt("bookcount");
	for( int i = 0; i < count; ++i )
	{
		std::string name = bookCfgName(i);
		std::string book_path = cfg.readString( name );

		if( !book_path.empty() )
			result.push_back( fs::FilePath(book_path, false) );
	}

	return result;
}

void addBook(const fs::FilePath &book)
{
	pocket::Config cfg(ConfigPath.getPath());
	const int count = cfg.readInt("bookcount");
	cfg.write("bookcount", count + 1);

	cfg.write(bookCfgName(count), book.getPath());
}

class TestSelector: public pocket::IDirectoryHandler
{
public:
	virtual void selected(const fs::FilePath &path)
	{
		addBook(path);
	}

	virtual void onExit();
};

class TestBookListHandler: public pocket::IBookListHandler
{
	virtual void startShow( std::auto_ptr<manga::Book> book)
	{
		pocket::PictureView::getInstance().setBook(book);
	}

	virtual void addNewBook()
	{
		pocket::openDirectorySelector(new TestSelector);
	}

	virtual void onExit()
	{
		CloseApp();
	}
};

class TestBooklistCfg: public pocket::IBooklistCfg
{
	virtual std::vector<fs::FilePath> getBooks()
	{
		return loadBooks();
	}
};

void TestSelector::onExit()
{
	pocket::showBooklist(new TestBookListHandler, new TestBooklistCfg);
}

int main_handler(int type, int par1, int par2) {

  std::cout << "type:"<< type << ",part1:"<<par1<<",part2:"<<par2 << std::endl;

  if (type == EVT_INIT) {
	  pocket::showBooklist(new TestBookListHandler, new TestBooklistCfg);
  }

  if (type == EVT_SHOW) {
	  //pocket::PictureView::getInstance().draw();
  }

  if (type == EVT_KEYPRESS) {

	  switch(par1)
	  {
	  case KEY_NEXT:
		  //pocket::PictureView::getInstance().next();
		  break;
	  }

    /*switch (par1) {

      case KEY_OK:
        OpenMenu(menu1, cindex, 20, 20, menu1_handler);
        break;

      case KEY_BACK:
        CloseApp();
        break;

      case KEY_LEFT:
        msg("KEY_LEFT");
        break;

      case KEY_RIGHT:
        msg("KEY_RIGHT");
        break;

      case KEY_UP:
        msg("KEY_UP");
        break;

      case KEY_DOWN:
        msg("KEY_DOWN");
        break;

      case KEY_MUSIC:
        msg("KEY_MUSIC");
        break;

      case KEY_MENU:
        msg("KEY_MENU");
        break;

      case KEY_DELETE:
        msg("KEY_DELETE");
        break;

    }*/
  }

  if (type == EVT_EXIT) {
    // occurs only in main handler when exiting or when SIGINT received.
    // save configuration here, if needed
  }

  return 0;

}

int main()
{
	OpenScreen();
  
	InkViewMain(main_handler);
	return 0;
}

