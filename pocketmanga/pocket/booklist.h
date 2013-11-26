#pragma once


#include <vector>
#include <memory>
#include "book.h"


namespace pocket
{

	class IBookListHandler
	{
	public:
		virtual ~IBookListHandler();

		// you have to delete aBook object yourself
		virtual void startShow( std::auto_ptr<manga::Book> aBook) = 0;
		virtual void addNewBook() = 0;
		virtual void onExit() = 0;
	};

	class IBooklistCfg
	{
	public:
		virtual ~IBooklistCfg();

		virtual std::vector<fs::FilePath> getBooks() = 0;
	};

	void showBooklist(IBookListHandler *aHandler, IBooklistCfg *aCfg);
}

