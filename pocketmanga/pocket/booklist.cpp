#include "booklist.h"

#include "singleton.h"

#include "inkview.h"

#include "primitives.h"

#include <map>

namespace
{
	static const int RowHeight = 60;
	static const int FontHeight = 24;

	static const int TextXOffset = (RowHeight - FontHeight)/2;

	static const bool NeedDrawSelection = false;
}

extern const ibitmap add_folder;

namespace pocket
{

IBookListHandler::~IBookListHandler()
{

}

IBooklistCfg::~IBooklistCfg()
{

}

////

class BookList: public utils::SingletonStatic<BookList>
{
	std::auto_ptr<IBookListHandler> mHandler;
	std::auto_ptr<IBooklistCfg> mCfg;

	std::vector<fs::FilePath> mBooks;

	typedef std::map<int, void(BookList::*)(const Position &, int /*index*/, int /*state*/)> MethodMap;
	MethodMap mMethods;

	ifont *mFont;

	int mLastLine;
private:

	void paintBook(const Position &aPos, const fs::FilePath &aCurEntry, bool aIsSelected)
	{
		int tTextXOffset 	= TextXOffset;
		int tTextWidth 		= ScreenWidth();

		SetFont(mFont, BLACK);

		tTextWidth -= tTextXOffset;

		DrawTextRect(
			aPos.x + tTextXOffset,
			aPos.y,
			tTextWidth,
			RowHeight,
			aCurEntry.getLastEntry().c_str(),
			ALIGN_LEFT | VALIGN_MIDDLE | DOTS | RTLAUTO);

		DrawLine(0, aPos.y, aPos.x + ScreenWidth(), aPos.y, BLACK);

		if( NeedDrawSelection && aIsSelected )
			DrawSelection(aPos.x, aPos.y, ScreenWidth(), RowHeight, BLACK);
	}

	void paintAddBook(const Position &aPos, bool aIsSelected)
	{
		int tTextWidth 		= ScreenWidth();
		int tTextXOffset 	= TextXOffset;

		DrawBitmap(aPos.x + TextXOffset, aPos.y + (RowHeight - add_folder.height)/2, &add_folder );
		tTextXOffset += TextXOffset + add_folder.width;

		SetFont(mFont, BLACK);

		DrawTextRect(
					aPos.x + tTextXOffset,
					aPos.y,
					tTextWidth,
					RowHeight,
					"Add new book",
					ALIGN_LEFT | VALIGN_MIDDLE | DOTS | RTLAUTO);
	}

	void onPaint(const Position &aPos, int aIndex, int state)
	{
		if( 0 == aIndex )
		{
			paintAddBook(aPos, 0 != state);
		}
		else
		{
			if( aIndex <= mBooks.size() )
				paintBook(aPos, mBooks[aIndex - 1], 0 != state);
		}

		mLastLine = aPos.y + RowHeight;
	}

	void onEndPaint(const Position &aPos, int aIndex, int state)
	{
		DrawLine(0, mLastLine, aPos.x + ScreenWidth(), mLastLine, BLACK);
	}

	void onExit(const Position &aPos, int aIndex, int state)
	{
		if( mHandler.get() )
			mHandler->onExit();
	}

	void onOpen(const Position &aPos, int aIndex, int state)
	{
			if( 0 == aIndex )
			{
				if( mHandler.get() )
					mHandler->addNewBook();
			}
			else
			{
				if( aIndex <= mBooks.size() && mHandler.get())
				{
					std::auto_ptr<manga::Book> tNewBook(new manga::Book);
					tNewBook->setRoot( mBooks[aIndex - 1].getPath() );
					mHandler->startShow(tNewBook);
				}
			}
	}
public:
	BookList()
		:mLastLine(0)
	{
		mFont = OpenFont(DEFAULTFONTB, FontHeight, 1);

		mMethods[LIST_PAINT] 	= &BookList::onPaint;
		mMethods[LIST_ENDPAINT] = &BookList::onEndPaint;
		mMethods[LIST_EXIT] 	= &BookList::onExit;
		mMethods[LIST_OPEN] 	= &BookList::onOpen;
	}

	~BookList()
	{
		CloseFont(mFont);
	}

	int listHandler(int action, int x, int y, int aIdx, int state)
	{
		MethodMap::const_iterator it = mMethods.find(action);

		if( it != mMethods.end() )
		{
			(this->*(it->second))( Position(x, y), aIdx, state);
		}

		return 0;
	}

	static int listHandlerStat(int action, int x, int y, int idx, int state)
	{
		return getInstance().listHandler(action, x, y, idx, state);
	}

	void show(IBookListHandler *aHandler, IBooklistCfg *aCfg)
	{
		mHandler.reset(aHandler);
		mCfg.reset(aCfg);

		if( mHandler.get() && mCfg.get() )
		{
			mBooks = mCfg->getBooks();

			OpenList("Book list", NULL, ScreenWidth(), RowHeight, mBooks.size() + 1, 0, listHandlerStat);
		}
	}
};

void showBooklist(IBookListHandler *aHandler, IBooklistCfg *aCfg)
{
	BookList::getInstance().show(aHandler, aCfg);
}


}
