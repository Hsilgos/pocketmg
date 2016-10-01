#include "common/iArchive.h"

#define ENV_HAVE_WCTYPE_H

#include "include_windows/windows.h"

#include "myWindows/StdAfx.h"

// #include "7zip/ICoder.h"
#include "Common/MyCom.h"


#include "Common/MyWindows.h"

#include "Common/Defs.h"
#include "Common/MyInitGuid.h"

#include "Common/IntToString.h"
#include "Common/StringConvert.h"
#include "Common/UTFConvert.h"

#include "Windows/DLL.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/FileName.h"
#include "Windows/NtCheck.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConv.h"

#include "7zip/Common/FileStreams.h"
#include "7zip/Archive/IArchive.h"

#include "7zip/IPassword.h"
#include "../C/7zVersion.h"

#include "7zip/UI/Common/OpenArchive.h"

#include <fstream>

STDAPI CreateArchiver(const GUID *clsid, const GUID *iid, void **outObject);

namespace {
DEFINE_GUID(CLSID_CFormat7z,
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);

DEFINE_GUID(CLSID_CArchiveHandler,
    k_7zip_GUID_Data1,
    k_7zip_GUID_Data2,
    k_7zip_GUID_Data3_Common,
    0x10, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00);

DEFINE_GUID(IID_IFolderArchiveExtractCallback2,
    k_7zip_GUID_Data1,
    k_7zip_GUID_Data2,
    k_7zip_GUID_Data3_Common,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x00, 0x00);

DEFINE_GUID(IID_IFolderExtractToStreamCallback,
    k_7zip_GUID_Data1,
    k_7zip_GUID_Data2,
    k_7zip_GUID_Data3_Common,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00);

DEFINE_GUID(IID_IGetProp,
    k_7zip_GUID_Data1,
    k_7zip_GUID_Data2,
    k_7zip_GUID_Data3_Common,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00);

#define CLSID_Format CLSID_CFormat7z

class CArchiveOpenCallback:
  public IArchiveOpenCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp {
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
  STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  bool PasswordIsDefined;
  UString Password;

  CArchiveOpenCallback() : PasswordIsDefined(false) {}
};

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64 * /* files */, const UInt64 * /* bytes */) {
  return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64 * /* files */, const UInt64 * /* bytes */) {
  return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR *password) {
  /*if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);*/
  return S_OK;
}

class CArchiveExtractCallback:
  public IArchiveExtractCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  // IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
  CMyComPtr<IInArchive> _archiveHandler;
  FString _directoryPath;  // Output directory
  UString _filePath;       // name inside arcvhive
  FString _diskFilePath;   // full path to file on disk
  bool _extractMode;
  struct CProcessedFileInfo
  {
    FILETIME MTime;
    UInt32 Attrib;
    bool isDir;
    bool AttribDefined;
    bool MTimeDefined;
  } _processedFileInfo;

  COutFileStream *_outFileStreamSpec;
  CMyComPtr<ISequentialOutStream> _outFileStream;

  CObjectVector<NWindows::NFile::NDir::CDelayedSymLink> _delayedSymLinks;

public:
  void Init(IInArchive *archiveHandler, const FString &directoryPath);

  HRESULT SetFinalAttribs();

  UInt64 NumErrors;
  bool PasswordIsDefined;
  UString Password;

  CArchiveExtractCallback() : PasswordIsDefined(false) {}
};

void CArchiveExtractCallback::Init(IInArchive *archiveHandler, const FString &directoryPath)
{
  /*NumErrors = 0;
  _archiveHandler = archiveHandler;
  _directoryPath = directoryPath;
  NName::NormalizeDirPathPrefix(_directoryPath);*/
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 /* size */)
{
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 * /* completeValue */)
{
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index,
    ISequentialOutStream **outStream, Int32 askExtractMode)
{
  /**outStream = 0;
  _outFileStream.Release();

  {
    // Get Name
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));

    UString fullPath;
    if (prop.vt == VT_EMPTY)
      fullPath = kEmptyFileAlias;
    else
    {
      if (prop.vt != VT_BSTR)
        return E_FAIL;
      fullPath = prop.bstrVal;
    }
    _filePath = fullPath;
  }

  if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
    return S_OK;

  {
    // Get Attrib
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
    if (prop.vt == VT_EMPTY)
    {
      _processedFileInfo.Attrib = 0;
      _processedFileInfo.AttribDefined = false;
    }
    else
    {
      if (prop.vt != VT_UI4)
        return E_FAIL;
      _processedFileInfo.Attrib = prop.ulVal;
      _processedFileInfo.AttribDefined = true;
    }
  }

  RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));

  {
    // Get Modified Time
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
    _processedFileInfo.MTimeDefined = false;
    switch (prop.vt)
    {
      case VT_EMPTY:
        // _processedFileInfo.MTime = _utcMTimeDefault;
        break;
      case VT_FILETIME:
        _processedFileInfo.MTime = prop.filetime;
        _processedFileInfo.MTimeDefined = true;
        break;
      default:
        return E_FAIL;
    }

  }
  {
    // Get Size
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
    UInt64 newFileSize;
    ConvertPropVariantToUInt64(prop, newFileSize);
  }


  {
    // Create folders for file
    int slashPos = _filePath.ReverseFind_PathSepar();
    if (slashPos >= 0)
      CreateComplexDir(_directoryPath + us2fs(_filePath.Left(slashPos)));
  }

  FString fullProcessedPath = _directoryPath + us2fs(_filePath);
  _diskFilePath = fullProcessedPath;

  if (_processedFileInfo.isDir)
  {
    CreateComplexDir(fullProcessedPath);
  }
  else
  {
    NFind::CFileInfo fi;
    if (fi.Find(fullProcessedPath))
    {
      if (!DeleteFileAlways(fullProcessedPath))
      {
        PrintError("Can not delete output file", fullProcessedPath);
        return E_ABORT;
      }
    }

    _outFileStreamSpec = new COutFileStream;
    CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
    if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
    {
      PrintError("Can not open output file", fullProcessedPath);
      return E_ABORT;
    }
    _outFileStream = outStreamLoc;
    *outStream = outStreamLoc.Detach();
  }*/
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
  /*_extractMode = false;
  switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  _extractMode = true; break;
  };
  switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  PrintString(kExtractingString); break;
    case NArchive::NExtract::NAskMode::kTest:  PrintString(kTestingString); break;
    case NArchive::NExtract::NAskMode::kSkip:  PrintString(kSkippingString); break;
  };
  PrintString(_filePath);*/
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
  /*switch (operationResult)
  {
    case NArchive::NExtract::NOperationResult::kOK:
      break;
    default:
    {
      NumErrors++;
      PrintString("  :  ");
      const char *s = NULL;
      switch (operationResult)
      {
        case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
          s = kUnsupportedMethod;
          break;
        case NArchive::NExtract::NOperationResult::kCRCError:
          s = kCRCFailed;
          break;
        case NArchive::NExtract::NOperationResult::kDataError:
          s = kDataError;
          break;
        case NArchive::NExtract::NOperationResult::kUnavailable:
          s = kUnavailableData;
          break;
        case NArchive::NExtract::NOperationResult::kUnexpectedEnd:
          s = kUnexpectedEnd;
          break;
        case NArchive::NExtract::NOperationResult::kDataAfterEnd:
          s = kDataAfterEnd;
          break;
        case NArchive::NExtract::NOperationResult::kIsNotArc:
          s = kIsNotArc;
          break;
        case NArchive::NExtract::NOperationResult::kHeadersError:
          s = kHeadersError;
          break;
      }
      if (s)
      {
        PrintString("Error : ");
        PrintString(s);
      }
      else
      {
        char temp[16];
        ConvertUInt32ToString(operationResult, temp);
        PrintString("Error #");
        PrintString(temp);
      }
    }
  }

  if (_outFileStream)
  {
    if (_processedFileInfo.MTimeDefined)
      _outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
    RINOK(_outFileStreamSpec->Close());
  }
  _outFileStream.Release();
  if (_extractMode && _processedFileInfo.AttribDefined)
    SetFileAttrib(_diskFilePath, _processedFileInfo.Attrib, &_delayedSymLinks);
  PrintNewLine();*/
  return S_OK;
}

HRESULT CArchiveExtractCallback::SetFinalAttribs()
{
  HRESULT result = S_OK;

  /*for (int i = 0; i != _delayedSymLinks.Size(); ++i)
    if (!_delayedSymLinks[i].Create())
      result = E_FAIL;

  _delayedSymLinks.Clear();*/

  return result;
}

STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
  /*if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);*/

  return E_ABORT;
}

class StdFileStream : public IInStream,
                     public CMyUnknownImp
{
public:
    StdFileStream(std::string const& file_path) : m_file(file_path.c_str()) {

    }

private:
    MY_QUERYINTERFACE_BEGIN2(IInStream)
    //MY_QUERYINTERFACE_ENTRY(IStreamGetSize)
    MY_QUERYINTERFACE_END
    MY_ADDREF_RELEASE

    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize)
    {
        m_file.read(reinterpret_cast<char*>(data), size);
        if (processedSize)
            *processedSize = m_file.gcount();
        return m_file.good() ? S_OK : S_FALSE;
    }

    STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
    {
        // m_file.seekg()
        switch (seekOrigin) {
        case STREAM_SEEK_SET:
            m_file.seekg(offset, std::ifstream::beg);
            break;
        case STREAM_SEEK_CUR:
            m_file.seekg(offset, std::ifstream::cur);
            break;
        case STREAM_SEEK_END:
            m_file.seekg(offset, std::ifstream::end);
            break;
        default:
            return S_FALSE;
        }

        if (newPosition)
            *newPosition = m_file.tellg();

        return m_file.good() ? S_OK : S_FALSE;
    }

    std::ifstream m_file;
};

}

namespace archive {
class SevenZipArchive : public IArchive {
public:
  SevenZipArchive(const std::string& extension) : m_extension(extension) {
  }

private:
  bool open(const std::string& fileName) {
    COpenOptions op;

    CCodecs *codecs = new CCodecs;
    m_codecsRef = codecs;

    m_openCallback = new COpenCallbackImp;

    codecs->Load();

    // #ifndef _SFX
    CObjectVector<CProperty> props;
    op.props = &props;
    // #endif
    op.codecs = codecs;
    op.callback = m_openCallback;
    //op.types = &types2;
    CIntVector excludedFormats;
    op.excludedFormats = &excludedFormats;
    //op.stdInMode = options.StdInMode;
    //StdFileStream* memory_stream = new StdFileStream(fileName);
    //CMyComPtr<IInStream> stream;
    //stream = memory_stream;
    UString pathUnicode;
    ConvertUTF8ToUnicode(fileName.c_str(), pathUnicode);
    // op.stream = stream;
    op.filePath = pathUnicode;
    CObjectVector<COpenType> types;
    std::wstring l_ext(m_extension.begin(), m_extension.end());
    ParseOpenTypes(*codecs, l_ext.c_str(), types);
    op.types = &types;

    HRESULT result = m_arcLink.Open(op);
    return result == S_OK;
  }

  void close() {
      m_arcLink.Close();
      m_codecsRef.Release();
  }

  std::vector<fs::FilePath> getFileList(bool files_only) {
    std::vector<fs::FilePath> result;
    IInArchive* in_archive = m_arcLink.GetArchive();
    if (in_archive) {
      UInt32 items_count = 0;
      in_archive->GetNumberOfItems(&items_count);
      result.reserve(items_count);
      const CArc* arc = m_arcLink.GetArc();
      CReadArcItem archive_item;
      for (UInt32 i = 0; i < items_count; ++i) {
        UString path;
        arc->GetItem(i, archive_item);
        AString pathUtf8;
        ConvertUnicodeToUTF8(archive_item.Path, pathUtf8);
        if (!archive_item.IsDir && files_only || !files_only)
          result.push_back(fs::FilePath(pathUtf8.Ptr(), true));
      }
    }
    return result;
  }

  tools::ByteArray getFile(const fs::FilePath& file_in_archive, size_t max_size) {
  }

private:
  CMyComPtr<IUnknown> m_codecsRef;
  CArchiveLink m_arcLink;
  std::string const m_extension;
  CMyComPtr<COpenCallbackImp> m_openCallback;
};

AUTO_REGISTER_ARCHIVER1("7z", SevenZipArchive, "7z");
AUTO_REGISTER_ARCHIVER1("rar", SevenZipArchive, "rar");
AUTO_REGISTER_ARCHIVER1("tar", SevenZipArchive, "tar");
AUTO_REGISTER_ARCHIVER1("tar.gz", SevenZipArchive, "tar.gz");
AUTO_REGISTER_ARCHIVER1("cab", SevenZipArchive, "cab");
AUTO_REGISTER_ARCHIVER1("bzip2", SevenZipArchive, "bzip2");
AUTO_REGISTER_ARCHIVER1("gzip", SevenZipArchive, "gzip");
AUTO_REGISTER_ARCHIVER1("jar", SevenZipArchive, "jar");
// AUTO_REGISTER_ARCHIVER1("gz", SevenZipArchive, "gz");
// AUTO_REGISTER_ARCHIVER1("gzip", SevenZipArchive, "tar.gz");
}
