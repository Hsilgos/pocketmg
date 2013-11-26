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
		std::auto_ptr<IDirectoryHandler> mSelector;
		iv_handler mCurrentHanler;

		ifont *mFont;

		//std::string mCurrentDir;
		fs::FilePath mCurrentDir;
		const fs::FilePath mEmpty;

		std::auto_ptr<fs::IFileManager> mFileMgr;
		std::vector<fs::FilePath> mCurrentFiles;
		
		std::vector<fs::FilePath> mRootPaths;

		//const ibitmap *mFolderBmp;
		const ibitmap *mFileBmp;
		const ibitmap *mReturnBmp;
		const ibitmap *mAddBmp;
		
		Position mPointerPos;
		
		int mLastLine;
		
		typedef std::map<int, void(DirSelector::*)(const Position &, const fs::FilePath &, int)> MethodMap;
		MethodMap mMethods;
	public:
		DirSelector()
			:mCurrentHanler(0),
			 mFileMgr(fs::IFileManager::create()),
			 mLastLine(0)
		{
			mFont 		= OpenFont(DEFAULTFONTB, FontHeight, 1);

			mFileBmp 	= &file;
			mReturnBmp 	= &return_back;
			mAddBmp 	= &add_folder;
			
			mRootPaths.push_back(SDPath);
			mRootPaths.push_back(USBPath);
			mRootPaths.push_back(FlashPath);

			mMethods[LIST_PAINT] 	= &DirSelector::onPaint;
			mMethods[LIST_ENDPAINT] = &DirSelector::onEndPaint;
			mMethods[LIST_POINTER] 	= &DirSelector::onPointer;
			mMethods[LIST_OPEN] 	= &DirSelector::onOpen;
			mMethods[LIST_EXIT] 	= &DirSelector::onExit;
		}

		~DirSelector()
		{
			CloseFont(mFont);
		}

		int listHandler(int action, int x, int y, int aIdx, int state)
		{
			//std::cout << "action:"<< action << ",x:"<<x<<",y:"<<y<<",idx:"<<aIdx<<",state:"<<state << std::endl;
			
			MethodMap::const_iterator it = mMethods.find(action);
			
			if( it != mMethods.end() )
			{
				const fs::FilePath &tCurEntr = (aIdx >= 0 && aIdx < mCurrentFiles.size())?(mCurrentFiles[aIdx]):(mEmpty);
				
				(this->*(it->second))( Position(x, y), tCurEntr, state);
			}
			
			return 0;
		}
		
		const ibitmap *chooseBmp(const fs::FilePath &aCurEntry) const
		{
			if( !aCurEntry.isDirectory() )
				return mFileBmp;
			
			if( aCurEntry.isBack() )
				return mReturnBmp;
			
			//std::cout << "sd:" << SDPath.getPath() << ", usb:" << USBPath.getPath() << std::endl;
			if( aCurEntry.startsWith(SDPath) )
				return &folder_sd;

			if( aCurEntry.startsWith(USBPath) )
				return &folder_usb;

			return &folder;
		}
		
		///////////////////////////////////////////////////////////////////////////////
		void onPaint(const Position &aPos, const fs::FilePath &aCurEntry, int state)
		{
			int tTextXOffset 	= TextXOffset;
			int tTextWidth 		= ScreenWidth();
			
			const ibitmap *tBmp = chooseBmp(aCurEntry);
			if( tBmp )
			{
				DrawBitmap(aPos.x + TextXOffset, aPos.y + (RowHeight - tBmp->height)/2, tBmp );
				tTextXOffset += TextXOffset + tBmp->width;
			}
			
			if( aCurEntry.isDirectory() && mAddBmp )
			{
				DrawBitmap(ScreenWidth() - mAddBmp->width, aPos.y, mAddBmp );
				tTextWidth -= mAddBmp->width;
			}
			
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

			if( NeedDrawSelection && state )
				DrawSelection(aPos.x, aPos.y, ScreenWidth(), RowHeight, BLACK);

			mLastLine = aPos.y + RowHeight;
		}
		
		void onEndPaint(const Position &aPos, const fs::FilePath &, int)
		{
			DrawLine(0, mLastLine, aPos.x + ScreenWidth(), mLastLine, BLACK);
		}
		
		void onPointer(const Position &aPos, const fs::FilePath &, int )
		{
			mPointerPos = aPos;
		}
		
		bool isInSelection() const
		{
			return (mAddBmp && (mPointerPos.x > (ScreenWidth() - mAddBmp->width)) );
		}
		
		void onOpen(const Position &aPos, const fs::FilePath &aCurEntry, int /*state*/)
		{
			if( !aCurEntry.isDirectory() )
				return;

			fs::FilePath tNewPath = mCurrentDir;
			
			if( aCurEntry.isBack() )
				tNewPath.removeLastEntry();
			else
				tNewPath = aCurEntry;
			
			if( isInSelection() )
			{
				if( mSelector.get() )
					mSelector->selected(tNewPath);
				doExit();
			}
			else
			{
				openList(tNewPath);
			}
		}		
		
		void onExit(const Position &, const fs::FilePath &, int)
		{
			doExit();
		}
		
		
		void doExit()
		{
			if( mSelector.get() )
				mSelector->onExit();
			else
				SetEventHandler(mCurrentHanler);

			mSelector.reset();
		}
		///////////////////////////////////////////////////////////////////////////////

		static int listHandlerStat(int action, int x, int y, int idx, int state)
		{
			return getInstance().listHandler(action, x, y, idx, state);
		}
		
		static bool isHidden(const fs::FilePath &aPath)
		{
			return aPath.isHidden();
		}

		void doOpenList(bool aAddBack)
		{
			mCurrentFiles.erase(std::remove_if(mCurrentFiles.begin(), mCurrentFiles.end(), isHidden), mCurrentFiles.end());

			if( aAddBack )
				mCurrentFiles.insert( mCurrentFiles.begin(), fs::FilePath("..", false) );

			OpenList("Select directory", 0, ScreenWidth(), RowHeight, mCurrentFiles.size(), 0, listHandlerStat);

		}

		void openRoot()
		{
			mCurrentFiles.clear();

			for( size_t i = 0; i < mRootPaths.size(); ++i )
			{
				std::vector<fs::FilePath>
					tExt = mFileMgr->getFileList(
						mRootPaths[i], fs::IFileManager::FileAndDirectory, false);

				for( size_t j = 0; j < tExt.size(); ++j )
					tExt[j].setFirstLevel(mRootPaths[i].getLevel());

				mCurrentFiles.insert(mCurrentFiles.end(), tExt.begin(), tExt.end());
			}

			std::sort(mCurrentFiles.begin(), mCurrentFiles.end(), fs::WordNumberOrder());

			for( size_t i = 0; i < mCurrentFiles.size(); ++i )
				mCurrentFiles[i].setFirstLevel(0);

			doOpenList(false);
		}

		bool isSomeRoot(const fs::FilePath &aPath) const
		{
			return std::find(mRootPaths.begin(), mRootPaths.end(), aPath) != mRootPaths.end();
		}

		static void menuhandler(int index)
		{

		}

		void openList(const fs::FilePath &aPath)
		{
			if( isSomeRoot(aPath) )
			{
				openRoot();
			}
			else
			{
				mCurrentDir = aPath;

				mCurrentFiles = mFileMgr->getFileList(mCurrentDir, fs::IFileManager::FileAndDirectory, false);

				std::sort(mCurrentFiles.begin(), mCurrentFiles.end(), fs::WordNumberOrder());

				doOpenList(true);
			}
		}

		void openDirectorySelector(IDirectoryHandler *aSelector)
		{
			mSelector.reset(aSelector);

			mCurrentHanler = GetEventHandler();

			openRoot();
		}
	};

	void openDirectorySelector(IDirectoryHandler *aSelector)
	{
		DirSelector::getInstance().openDirectorySelector(aSelector);
	}
}

