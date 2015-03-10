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
	std::auto_ptr<IBookListHandler> handler_;
	std::auto_ptr<IBooklistCfg> cfg_;

	std::vector<fs::FilePath> books_;

	typedef std::map<int, void(BookList::*)(const Position &, int /*index*/, int /*state*/)> MethodMap;
	MethodMap methods_;

	ifont *font_;

	int last_line_;
private:

	void paintBook(const Position &pos, const fs::FilePath &cur_entry, bool is_selected)
	{
		int text_x_offset 	= TextXOffset;
		int text_width 		= ScreenWidth();

		SetFont(font_, BLACK);

		text_width -= text_x_offset;

		DrawTextRect(
			pos.x + text_x_offset,
			pos.y,
			text_width,
			RowHeight,
			cur_entry.getLastEntry().c_str(),
			ALIGN_LEFT | VALIGN_MIDDLE | DOTS | RTLAUTO);

		DrawLine(0, pos.y, pos.x + ScreenWidth(), pos.y, BLACK);

		if( NeedDrawSelection && is_selected )
			DrawSelection(pos.x, pos.y, ScreenWidth(), RowHeight, BLACK);
	}

	void paintAddBook(const Position &pos, bool is_selected)
	{
		int text_width 		= ScreenWidth();
		int text_x_offset 	= TextXOffset;

		DrawBitmap(pos.x + TextXOffset, pos.y + (RowHeight - add_folder.height)/2, &add_folder );
		text_x_offset += TextXOffset + add_folder.width;

		SetFont(font_, BLACK);

		DrawTextRect(
					pos.x + text_x_offset,
					pos.y,
					text_width,
					RowHeight,
					"Add new book",
					ALIGN_LEFT | VALIGN_MIDDLE | DOTS | RTLAUTO);
	}

	void onPaint(const Position &pos, int index, int state)
	{
		if( 0 == index )
		{
			paintAddBook(pos, 0 != state);
		}
		else
		{
			if( index <= books_.size() )
				paintBook(pos, books_[index - 1], 0 != state);
		}

		last_line_ = pos.y + RowHeight;
	}

	void onEndPaint(const Position &pos, int index, int state)
	{
		DrawLine(0, last_line_, pos.x + ScreenWidth(), last_line_, BLACK);
	}

	void onExit(const Position &pos, int index, int state)
	{
		if( handler_.get() )
			handler_->onExit();
	}

	void onOpen(const Position &pos, int index, int state)
	{
			if( 0 == index )
			{
				if( handler_.get() )
					handler_->addNewBook();
			}
			else
			{
				if( index <= books_.size() && handler_.get())
				{
					std::auto_ptr<manga::Book> new_book(new manga::Book);
					new_book->setRoot( books_[index - 1] );
					handler_->startShow(new_book);
				}
			}
	}
public:
	BookList()
		:last_line_(0)
	{
		font_ = OpenFont(DEFAULTFONTB, FontHeight, 1);

		methods_[LIST_PAINT] 	= &BookList::onPaint;
		methods_[LIST_ENDPAINT] = &BookList::onEndPaint;
		methods_[LIST_EXIT] 	= &BookList::onExit;
		methods_[LIST_OPEN] 	= &BookList::onOpen;
	}

	~BookList()
	{
		CloseFont(font_);
	}

	int listHandler(int action, int x, int y, int idx, int state)
	{
		MethodMap::const_iterator it = methods_.find(action);

		if( it != methods_.end() )
		{
			(this->*(it->second))( Position(x, y), idx, state);
		}

		return 0;
	}

	static int listHandlerStat(int action, int x, int y, int idx, int state)
	{
		return getInstance().listHandler(action, x, y, idx, state);
	}

	void show(IBookListHandler *handler, IBooklistCfg *cfg)
	{
		handler_.reset(handler);
		cfg_.reset(cfg);

		if( handler_.get() && cfg_.get() )
		{
			books_ = cfg_->getBooks();

			OpenList("Book list", NULL, ScreenWidth(), RowHeight, books_.size() + 1, 0, listHandlerStat);
		}
	}
};

void showBooklist(IBookListHandler *handler, IBooklistCfg *cfg)
{
	BookList::getInstance().show(handler, cfg);
}


}
