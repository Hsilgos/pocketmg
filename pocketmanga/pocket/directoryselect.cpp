#include "directoryselect.h"

#include "inkview.h"

#include "singleton.h"

#include "filemanager.h"
#include "filepath.h"
#include "primitives.h"

#include <memory>

#include<iostream>
#include<algorithm>
#include<map>

namespace
{
	static const int RowHeight = 60;
	static const int FontHeight = 24;
	static const int TextXOffset = (RowHeight - FontHeight)/2;
	//static const int TextYOffset = (RowHeight - FontHeight)/2;

#ifdef IVSAPP// emulator?
	static const fs::FilePath SDPath("/usr/local/pocketbook/mnt/ext2", false);
	static const fs::FilePath USBPath("/usr/local/pocketbook/mnt/ext3", false);
	static const fs::FilePath FlashPath("/usr/local/pocketbook/mnt/ext1", false);
#else
	static const fs::FilePath SDPath(SDCARDDIR, false);
	static const fs::FilePath USBPath(USBDIR, false);
	static const fs::FilePath FlashPath(FLASHDIR, false);
#endif

	static const bool NeedDrawSelection = false;
}



extern const ibitmap file; 
extern const ibitmap return_back;
extern const ibitmap add_folder;

extern const ibitmap folder;
extern const ibitmap folder_usb;
extern const ibitmap folder_sd;

// /mnt/ext1

namespace pocket
{
	
	IDirectoryHandler::~IDirectoryHandler()
	{
	}

	class DirSelector: public utils::SingletonStatic<DirSelector>
	{
		std::auto_ptr<IDirectoryHandler> selector_;
		iv_handler current_hanler_;

		ifont *font_;

		//std::string current_dir_;
		fs::FilePath current_dir_;
		const fs::FilePath empty_;

		std::auto_ptr<fs::IFileManager> file_mgr_;
		std::vector<fs::FilePath> current_files_;
		
		std::vector<fs::FilePath> root_paths_;

		//const ibitmap *folder_bmp_;
		const ibitmap *file_bmp_;
		const ibitmap *return_bmp_;
		const ibitmap *add_bmp_;
		
		Position pointer_pos_;
		
		int last_line_;
		
		typedef std::map<int, void(DirSelector::*)(const Position &, const fs::FilePath &, int)> MethodMap;
		MethodMap methods_;
	public:
		DirSelector()
			:current_hanler_(0),
			 file_mgr_(fs::IFileManager::create()),
			 last_line_(0)
		{
			font_ 		= OpenFont(DEFAULTFONTB, FontHeight, 1);

			file_bmp_ 	= &file;
			return_bmp_ 	= &return_back;
			add_bmp_ 	= &add_folder;
			
			root_paths_.push_back(SDPath);
			root_paths_.push_back(USBPath);
			root_paths_.push_back(FlashPath);

			methods_[LIST_PAINT] 	= &DirSelector::onPaint;
			methods_[LIST_ENDPAINT] = &DirSelector::onEndPaint;
			methods_[LIST_POINTER] 	= &DirSelector::onPointer;
			methods_[LIST_OPEN] 	= &DirSelector::onOpen;
			methods_[LIST_EXIT] 	= &DirSelector::onExit;
		}

		~DirSelector()
		{
			CloseFont(font_);
		}

		int listHandler(int action, int x, int y, int idx, int state)
		{
			//std::cout << "action:"<< action << ",x:"<<x<<",y:"<<y<<",idx:"<<idx<<",state:"<<state << std::endl;
			
			MethodMap::const_iterator it = methods_.find(action);
			
			if( it != methods_.end() )
			{
				const fs::FilePath &cur_entr = (idx >= 0 && idx < current_files_.size())?(current_files_[idx]):(empty_);
				
				(this->*(it->second))( Position(x, y), cur_entr, state);
			}
			
			return 0;
		}
		
		const ibitmap *chooseBmp(const fs::FilePath &cur_entry) const
		{
			if( !cur_entry.isDirectory() )
				return file_bmp_;
			
			if( cur_entry.isBack() )
				return return_bmp_;
			
			//std::cout << "sd:" << SDPath.getPath() << ", usb:" << USBPath.getPath() << std::endl;
			if( cur_entry.startsWith(SDPath) )
				return &folder_sd;

			if( cur_entry.startsWith(USBPath) )
				return &folder_usb;

			return &folder;
		}
		
		///////////////////////////////////////////////////////////////////////////////
		void onPaint(const Position &pos, const fs::FilePath &cur_entry, int state)
		{
			int text_x_offset 	= TextXOffset;
			int text_width 		= ScreenWidth();
			
			const ibitmap *bmp = chooseBmp(cur_entry);
			if( bmp )
			{
				DrawBitmap(pos.x + TextXOffset, pos.y + (RowHeight - bmp->height)/2, bmp );
				text_x_offset += TextXOffset + bmp->width;
			}
			
			if( cur_entry.isDirectory() && add_bmp_ )
			{
				DrawBitmap(ScreenWidth() - add_bmp_->width, pos.y, add_bmp_ );
				text_width -= add_bmp_->width;
			}
			
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

			if( NeedDrawSelection && state )
				DrawSelection(pos.x, pos.y, ScreenWidth(), RowHeight, BLACK);

			last_line_ = pos.y + RowHeight;
		}
		
		void onEndPaint(const Position &pos, const fs::FilePath &, int)
		{
			DrawLine(0, last_line_, pos.x + ScreenWidth(), last_line_, BLACK);
		}
		
		void onPointer(const Position &pos, const fs::FilePath &, int )
		{
			pointer_pos_ = pos;
		}
		
		bool isInSelection() const
		{
			return (add_bmp_ && (pointer_pos_.x > (ScreenWidth() - add_bmp_->width)) );
		}
		
		void onOpen(const Position &pos, const fs::FilePath &cur_entry, int /*state*/)
		{
			if( !cur_entry.isDirectory() )
				return;

			fs::FilePath new_path = current_dir_;
			
			if( cur_entry.isBack() )
				new_path.removeLastEntry();
			else
				new_path = cur_entry;
			
			if( isInSelection() )
			{
				if( selector_.get() )
					selector_->selected(new_path);
				doExit();
			}
			else
			{
				openList(new_path);
			}
		}		
		
		void onExit(const Position &, const fs::FilePath &, int)
		{
			doExit();
		}
		
		
		void doExit()
		{
			if( selector_.get() )
				selector_->onExit();
			else
				SetEventHandler(current_hanler_);

			selector_.reset();
		}
		///////////////////////////////////////////////////////////////////////////////

		static int listHandlerStat(int action, int x, int y, int idx, int state)
		{
			return getInstance().listHandler(action, x, y, idx, state);
		}
		
		static bool isHidden(const fs::FilePath &path)
		{
			return path.isHidden();
		}

		void doOpenList(bool add_back)
		{
			current_files_.erase(std::remove_if(current_files_.begin(), current_files_.end(), isHidden), current_files_.end());

			if( add_back )
				current_files_.insert( current_files_.begin(), fs::FilePath("..", false) );

			OpenList("Select directory", 0, ScreenWidth(), RowHeight, current_files_.size(), 0, listHandlerStat);

		}

		void openRoot()
		{
			current_files_.clear();

			for( size_t i = 0; i < root_paths_.size(); ++i )
			{
				std::vector<fs::FilePath>
					ext = file_mgr_->getFileList(
						root_paths_[i], fs::IFileManager::FileAndDirectory, false);

				for( size_t j = 0; j < ext.size(); ++j )
					ext[j].setFirstLevel(root_paths_[i].getLevel());

				current_files_.insert(current_files_.end(), ext.begin(), ext.end());
			}

			std::sort(current_files_.begin(), current_files_.end(), fs::WordNumberOrder());

			for( size_t i = 0; i < current_files_.size(); ++i )
				current_files_[i].setFirstLevel(0);

			doOpenList(false);
		}

		bool isSomeRoot(const fs::FilePath &path) const
		{
			return std::find(root_paths_.begin(), root_paths_.end(), path) != root_paths_.end();
		}

		static void menuhandler(int index)
		{

		}

		void openList(const fs::FilePath &path)
		{
			if( isSomeRoot(path) )
			{
				openRoot();
			}
			else
			{
				current_dir_ = path;

				current_files_ = file_mgr_->getFileList(current_dir_, fs::IFileManager::FileAndDirectory, false);

				std::sort(current_files_.begin(), current_files_.end(), fs::WordNumberOrder());

				doOpenList(true);
			}
		}

		void openDirectorySelector(IDirectoryHandler *selector)
		{
			selector_.reset(selector);

			current_hanler_ = GetEventHandler();

			openRoot();
		}
	};

	void openDirectorySelector(IDirectoryHandler *selector)
	{
		DirSelector::getInstance().openDirectorySelector(selector);
	}
}

