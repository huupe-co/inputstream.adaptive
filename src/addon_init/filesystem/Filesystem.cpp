#include "Filesystem.h"

// #include "FileItem.h"
//#include "Util.h"
//#include "addons/binary-addons/AddonDll.h"
// #include "CurlFile.h"
#include "File.h"
//#include "filesystem/SpecialProtocol.h"
//#include "platform/Filesystem.h"
//#include "utils/Crc32.h"
#include "utils/HttpHeader.h"
//#include "utils/StringUtils.h"
//#include "utils/URIUtils.h"
//#include "utils/log.h"

#include <filesystem>

#include <sys/stat.h>

using namespace kodi; // addon-dev-kit namespace
using namespace XFILE;

namespace fs = std::filesystem;

void Interface_Filesystem::Init(AddonGlobalInterface* addonInterface)
{
  addonInterface->toKodi->kodi_filesystem = new AddonToKodiFuncTable_kodi_filesystem();

  addonInterface->toKodi->kodi_filesystem->can_open_directory = can_open_directory;
  addonInterface->toKodi->kodi_filesystem->create_directory = create_directory;
  addonInterface->toKodi->kodi_filesystem->directory_exists = directory_exists;
  addonInterface->toKodi->kodi_filesystem->remove_directory = remove_directory;
  addonInterface->toKodi->kodi_filesystem->remove_directory_recursive = remove_directory_recursive;
  addonInterface->toKodi->kodi_filesystem->get_directory = get_directory;
  addonInterface->toKodi->kodi_filesystem->free_directory = free_directory;

  addonInterface->toKodi->kodi_filesystem->file_exists = file_exists;
  addonInterface->toKodi->kodi_filesystem->stat_file = stat_file;
  addonInterface->toKodi->kodi_filesystem->delete_file = delete_file;
  addonInterface->toKodi->kodi_filesystem->rename_file = rename_file;
  addonInterface->toKodi->kodi_filesystem->copy_file = copy_file;
  addonInterface->toKodi->kodi_filesystem->get_file_md5 = get_file_md5;
  addonInterface->toKodi->kodi_filesystem->get_cache_thumb_name = get_cache_thumb_name;
  addonInterface->toKodi->kodi_filesystem->make_legal_filename = make_legal_filename;
  addonInterface->toKodi->kodi_filesystem->make_legal_path = make_legal_path;
  addonInterface->toKodi->kodi_filesystem->translate_special_protocol = translate_special_protocol;
  addonInterface->toKodi->kodi_filesystem->get_disk_space = get_disk_space;
  addonInterface->toKodi->kodi_filesystem->is_internet_stream = is_internet_stream;
  addonInterface->toKodi->kodi_filesystem->is_on_lan = is_on_lan;
  addonInterface->toKodi->kodi_filesystem->is_remote = is_remote;
  addonInterface->toKodi->kodi_filesystem->is_local = is_local;
  addonInterface->toKodi->kodi_filesystem->is_url = is_url;
  addonInterface->toKodi->kodi_filesystem->get_http_header = get_http_header;
  addonInterface->toKodi->kodi_filesystem->get_mime_type = get_mime_type;
  addonInterface->toKodi->kodi_filesystem->get_content_type = get_content_type;
  addonInterface->toKodi->kodi_filesystem->get_cookies = get_cookies;

  addonInterface->toKodi->kodi_filesystem->http_header_create = http_header_create;
  addonInterface->toKodi->kodi_filesystem->http_header_free = http_header_free;

  addonInterface->toKodi->kodi_filesystem->open_file = open_file;
  addonInterface->toKodi->kodi_filesystem->open_file_for_write = open_file_for_write;
  addonInterface->toKodi->kodi_filesystem->read_file = read_file;
  addonInterface->toKodi->kodi_filesystem->read_file_string = read_file_string;
  addonInterface->toKodi->kodi_filesystem->write_file = write_file;
  addonInterface->toKodi->kodi_filesystem->flush_file = flush_file;
  addonInterface->toKodi->kodi_filesystem->seek_file = seek_file;
  addonInterface->toKodi->kodi_filesystem->truncate_file = truncate_file;
  addonInterface->toKodi->kodi_filesystem->get_file_position = get_file_position;
  addonInterface->toKodi->kodi_filesystem->get_file_length = get_file_length;
  addonInterface->toKodi->kodi_filesystem->get_file_download_speed = get_file_download_speed;
  addonInterface->toKodi->kodi_filesystem->close_file = close_file;
  addonInterface->toKodi->kodi_filesystem->get_file_chunk_size = get_file_chunk_size;
  addonInterface->toKodi->kodi_filesystem->io_control_get_seek_possible =
      io_control_get_seek_possible;
  addonInterface->toKodi->kodi_filesystem->io_control_get_cache_status =
      io_control_get_cache_status;
  addonInterface->toKodi->kodi_filesystem->io_control_set_cache_rate = io_control_set_cache_rate;
  addonInterface->toKodi->kodi_filesystem->io_control_set_retry = io_control_set_retry;
  addonInterface->toKodi->kodi_filesystem->get_property_values = get_property_values;

  addonInterface->toKodi->kodi_filesystem->curl_create = curl_create;
  addonInterface->toKodi->kodi_filesystem->curl_add_option = curl_add_option;
  addonInterface->toKodi->kodi_filesystem->curl_open = curl_open;
}

void Interface_Filesystem::DeInit(AddonGlobalInterface* addonInterface)
{
  if (addonInterface->toKodi) /* <-- Safe check, needed so long old addon way is present */
  {
    delete addonInterface->toKodi->kodi_filesystem;
    addonInterface->toKodi->kodi_filesystem = nullptr;
  }
}

unsigned int Interface_Filesystem::TranslateFileReadBitsToKodi(unsigned int addonFlags)
{
  unsigned int kodiFlags = 0;

  if (addonFlags & ADDON_READ_TRUNCATED)
    kodiFlags |= READ_TRUNCATED;
  if (addonFlags & ADDON_READ_CHUNKED)
    kodiFlags |= READ_CHUNKED;
  if (addonFlags & ADDON_READ_CACHED)
    kodiFlags |= READ_CACHED;
  if (addonFlags & ADDON_READ_NO_CACHE)
    kodiFlags |= READ_NO_CACHE;
  if (addonFlags & ADDON_READ_BITRATE)
    kodiFlags |= READ_BITRATE;
  if (addonFlags & ADDON_READ_MULTI_STREAM)
    kodiFlags |= READ_MULTI_STREAM;
  if (addonFlags & ADDON_READ_AUDIO_VIDEO)
    kodiFlags |= READ_AUDIO_VIDEO;
  if (addonFlags & ADDON_READ_AFTER_WRITE)
    kodiFlags |= READ_AFTER_WRITE;
  if (addonFlags & READ_REOPEN)
    kodiFlags |= READ_REOPEN;

  return kodiFlags;
}

bool Interface_Filesystem::can_open_directory(void* kodiBase, const char* url)
{
  return true;
}

bool Interface_Filesystem::create_directory(void* kodiBase, const char* path)
{
  return fs::create_directories(path);
}

bool Interface_Filesystem::directory_exists(void* kodiBase, const char* path)
{
  return fs::exists(path);
}

bool Interface_Filesystem::remove_directory(void* kodiBase, const char* path)
{
  if (fs::is_directory(path))
  {
    for (auto& child : fs::directory_iterator(path))
    {
      fs::remove(child.path());
    }
  }

  return fs::remove(path);
}

bool Interface_Filesystem::remove_directory_recursive(void* kodiBase, const char* path)
{
  if (fs::is_directory(path))
  {
    fs::remove_all(path);
  }

  return true;
}

static void CFileItemListToVFSDirEntries(VFSDirEntry* entries,
                                         std::vector<fs::directory_entry> items)
{
  for (unsigned int i = 0; i < static_cast<unsigned int>(items.size()); ++i)
  {
    entries[i].label = strdup(items[i].path().filename().c_str());
    std::string path = items[i].path();
    entries[i].path = strdup(path.c_str());
    entries[i].size = items[i].file_size();
    entries[i].folder = items[i].is_directory();
    //items[i]->m_dateTime.GetAsTime(entries[i].date_time);
  }
}

bool Interface_Filesystem::get_directory(void* kodiBase,
                                         const char* path,
                                         const char* mask,
                                         struct VFSDirEntry** items,
                                         unsigned int* num_items)
{
  if (fs::is_directory(path))
  {
    std::vector<fs::directory_entry> fileItems;
    for (const fs::directory_entry& child : fs::directory_iterator(path))
    {
      fileItems.push_back(child);
    }
    if (fileItems.size() > 0)
    {
      *num_items = static_cast<unsigned int>(fileItems.size());
      *items = new VFSDirEntry[fileItems.size()];
      CFileItemListToVFSDirEntries(*items, fileItems);
    }
    else
    {
      *num_items = 0;
      *items = nullptr;
    }
  }
}

void Interface_Filesystem::free_directory(void* kodiBase,
                                          struct VFSDirEntry* items,
                                          unsigned int num_items)
{
  for (unsigned int i = 0; i < num_items; ++i)
  {
    free(items[i].label);
    free(items[i].path);
  }
  delete[] items;
}

//------------------------------------------------------------------------------

bool Interface_Filesystem::file_exists(void* kodiBase, const char* filename, bool useCache)
{

  return fs::exists(filename);
}

bool Interface_Filesystem::stat_file(void* kodiBase,
                                     const char* filename,
                                     struct STAT_STRUCTURE* buffer)
{

  struct stat64 statBuffer;

  if (stat64(filename, &statBuffer) != 0)
    return false;

  buffer->deviceId = statBuffer.st_dev;
  buffer->fileSerialNumber = statBuffer.st_ino;
  buffer->size = statBuffer.st_size;
  buffer->accessTime = statBuffer.st_atime;
  buffer->modificationTime = statBuffer.st_mtime;
  buffer->statusTime = statBuffer.st_ctime;
  buffer->isDirectory = S_ISDIR(statBuffer.st_mode);
  buffer->isSymLink = S_ISLNK(statBuffer.st_mode);
  buffer->isBlock = S_ISBLK(statBuffer.st_mode);
  buffer->isCharacter = S_ISCHR(statBuffer.st_mode);
  buffer->isFifo = S_ISFIFO(statBuffer.st_mode);
  buffer->isRegular = S_ISREG(statBuffer.st_mode);
  buffer->isSocket = S_ISSOCK(statBuffer.st_mode);

  return true;
}

bool Interface_Filesystem::delete_file(void* kodiBase, const char* filename)
{
  return fs::remove(filename);
}

bool Interface_Filesystem::rename_file(void* kodiBase,
                                       const char* filename,
                                       const char* newFileName)
{
  std::error_code ec;
  fs::rename(filename, newFileName, ec);
  return (bool)ec;
}

bool Interface_Filesystem::copy_file(void* kodiBase, const char* filename, const char* dest)
{
  return fs::copy_file(filename, dest);
}

char* Interface_Filesystem::get_file_md5(void* kodiBase, const char* filename)
{
  //std::string string = CUtil::GetFileDigest(filename, KODI::UTILITY::CDigest::Type::MD5);
  //char* buffer = strdup(string.c_str());
  //return buffer;
  printf("get_file_md5!!!!!!!!!!");
  return "";
}

char* Interface_Filesystem::get_cache_thumb_name(void* kodiBase, const char* filename)
{
  // const auto crc = Crc32::ComputeFromLowerCase(filename);
  // const auto hex = StringUtils::Format("{:08x}.tbn", crc);
  // char* buffer = strdup(hex.c_str());
  // return buffer;
  printf("get_cache_thumb_name!!!!!!!!!!");
  return "";
}

char* Interface_Filesystem::make_legal_filename(void* kodiBase, const char* filename)
{
  // std::string string = CUtil::MakeLegalFileName(filename);
  // char* buffer = strdup(string.c_str());
  // return buffer;
  printf("make_legal_filename!!!!!!!!!!");
  return (char*)filename;
}

char* Interface_Filesystem::make_legal_path(void* kodiBase, const char* path)
{
  // std::string string = CUtil::MakeLegalPath(path);
  // char* buffer = strdup(string.c_str());
  // return buffer;
  printf("make_legal_path!!!!!!!!!!");
  return (char*)path;
}

char* Interface_Filesystem::translate_special_protocol(void* kodiBase, const char* strSource)
{
  //return strdup(CSpecialProtocol::TranslatePath(strSource).c_str());
  printf("translate_special_protocol!!!!!!!!!!");
  char* trans = (char*)malloc(strlen(strSource) + 1);
  strcpy(trans, strSource);
  return trans;
}

bool Interface_Filesystem::get_disk_space(
    void* kodiBase, const char* path, uint64_t* capacity, uint64_t* free, uint64_t* available)
{
  return false;
}

bool Interface_Filesystem::is_internet_stream(void* kodiBase, const char* path, bool strictCheck)
{
  //return URIUtils::IsInternetStream(path, strictCheck);
  return true;
}

bool Interface_Filesystem::is_on_lan(void* kodiBase, const char* path)
{
  //return URIUtils::IsOnLAN(path);
  return false;
}

bool Interface_Filesystem::is_remote(void* kodiBase, const char* path)
{
  //return URIUtils::IsRemote(path);
  return true;
}

bool Interface_Filesystem::is_local(void* kodiBase, const char* path)
{
  //return CURL(path).IsLocal();
  return true;
}

bool Interface_Filesystem::is_url(void* kodiBase, const char* path)
{
  //return URIUtils::IsURL(path);
  return true;
}

bool Interface_Filesystem::get_mime_type(void* kodiBase,
                                         const char* url,
                                         char** content,
                                         const char* useragent)
{

  // std::string kodiContent;
  // bool ret = XFILE::CCurlFile::GetMimeType(CURL(url), kodiContent, useragent);
  // if (ret && !kodiContent.empty())
  // {
  //   *content = strdup(kodiContent.c_str());
  // }
  return false;
}

bool Interface_Filesystem::get_content_type(void* kodiBase,
                                            const char* url,
                                            char** content,
                                            const char* useragent)
{
  // std::string kodiContent;
  // bool ret = XFILE::CCurlFile::GetContentType(CURL(url), kodiContent, useragent);
  // if (ret && !kodiContent.empty())
  // {
  //   *content = strdup(kodiContent.c_str());
  // }
  return false;
}

bool Interface_Filesystem::get_cookies(void* kodiBase, const char* url, char** cookies)
{
  // std::string kodiCookies;
  // bool ret = XFILE::CCurlFile::GetCookies(CURL(url), kodiCookies);
  // if (ret && !kodiCookies.empty())
  // {
  //   *cookies = strdup(kodiCookies.c_str());
  // }
  return false;
}

bool Interface_Filesystem::get_http_header(void* kodiBase,
                                           const char* url,
                                           struct KODI_HTTP_HEADER* headers)
{
  // CHttpHeader* httpHeader = static_cast<CHttpHeader*>(headers->handle);
  // return XFILE::CCurlFile::GetHttpHeader(CURL(url), *httpHeader);
  return false;
}

//------------------------------------------------------------------------------

bool Interface_Filesystem::http_header_create(void* kodiBase, struct KODI_HTTP_HEADER* headers)
{
  headers->handle = new CHttpHeader;
  headers->get_value = http_header_get_value;
  headers->get_values = http_header_get_values;
  headers->get_header = http_header_get_header;
  headers->get_mime_type = http_header_get_mime_type;
  headers->get_charset = http_header_get_charset;
  headers->get_proto_line = http_header_get_proto_line;

  return true;
}

void Interface_Filesystem::http_header_free(void* kodiBase, struct KODI_HTTP_HEADER* headers)
{
  delete static_cast<CHttpHeader*>(headers->handle);
  headers->handle = nullptr;
}

char* Interface_Filesystem::http_header_get_value(void* kodiBase, void* handle, const char* param)
{
  std::string string = static_cast<CHttpHeader*>(handle)->GetValue(param);

  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;
}

char** Interface_Filesystem::http_header_get_values(void* kodiBase,
                                                    void* handle,
                                                    const char* param,
                                                    int* length)
{
  std::vector<std::string> values = static_cast<CHttpHeader*>(handle)->GetValues(param);
  *length = values.size();
  char** ret = static_cast<char**>(malloc(sizeof(char*) * values.size()));
  for (int i = 0; i < *length; ++i)
  {
    ret[i] = strdup(values[i].c_str());
  }
  return ret;
}

char* Interface_Filesystem::http_header_get_header(void* kodiBase, void* handle)
{

  std::string string = static_cast<CHttpHeader*>(handle)->GetHeader();

  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::http_header_get_mime_type(void* kodiBase, void* handle)
{


  std::string string = static_cast<CHttpHeader*>(handle)->GetMimeType();

  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::http_header_get_charset(void* kodiBase, void* handle)
{

  std::string string = static_cast<CHttpHeader*>(handle)->GetCharset();

  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::http_header_get_proto_line(void* kodiBase, void* handle)
{

  std::string string = static_cast<CHttpHeader*>(handle)->GetProtoLine();

  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;
}

//------------------------------------------------------------------------------

void* Interface_Filesystem::open_file(void* kodiBase, const char* filename, unsigned int flags)
{


  CFile* file = new CFile;
  if (file->Open(filename, TranslateFileReadBitsToKodi(flags)))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

void* Interface_Filesystem::open_file_for_write(void* kodiBase,
                                                const char* filename,
                                                bool overwrite)
{

  CFile* file = new CFile;
  if (file->OpenForWrite(filename, overwrite))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

ssize_t Interface_Filesystem::read_file(void* kodiBase, void* file, void* ptr, size_t size)
{

  return static_cast<CFile*>(file)->Read(ptr, size);
}

bool Interface_Filesystem::read_file_string(void* kodiBase,
                                            void* file,
                                            char* szLine,
                                            int lineLength)
{

  return static_cast<CFile*>(file)->ReadString(szLine, lineLength);
}

ssize_t Interface_Filesystem::write_file(void* kodiBase, void* file, const void* ptr, size_t size)
{
  return static_cast<CFile*>(file)->Write(ptr, size);
}

void Interface_Filesystem::flush_file(void* kodiBase, void* file)
{

  static_cast<CFile*>(file)->Flush();
}

int64_t Interface_Filesystem::seek_file(void* kodiBase, void* file, int64_t position, int whence)
{

  return static_cast<CFile*>(file)->Seek(position, whence);
}

int Interface_Filesystem::truncate_file(void* kodiBase, void* file, int64_t size)
{

  return static_cast<CFile*>(file)->Truncate(size);
}

int64_t Interface_Filesystem::get_file_position(void* kodiBase, void* file)
{

  return static_cast<CFile*>(file)->GetPosition();
}

int64_t Interface_Filesystem::get_file_length(void* kodiBase, void* file)
{
  return static_cast<CFile*>(file)->GetLength();
}

double Interface_Filesystem::get_file_download_speed(void* kodiBase, void* file)
{
  return static_cast<CFile*>(file)->GetDownloadSpeed();
}

void Interface_Filesystem::close_file(void* kodiBase, void* file)
{
  static_cast<CFile*>(file)->Close();
  delete static_cast<CFile*>(file);
}

int Interface_Filesystem::get_file_chunk_size(void* kodiBase, void* file)
{
  return static_cast<CFile*>(file)->GetChunkSize();
}

bool Interface_Filesystem::io_control_get_seek_possible(void* kodiBase, void* file)
{

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_SEEK_POSSIBLE, nullptr) != 0
             ? true
             : false;
}

bool Interface_Filesystem::io_control_get_cache_status(void* kodiBase,
                                                       void* file,
                                                       struct VFS_CACHE_STATUS_DATA* status)
{
  SCacheStatus data = {};
  int ret = static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_CACHE_STATUS, &data);
  if (ret >= 0)
  {
    status->forward = data.forward;
    status->maxrate = data.maxrate;
    status->currate = data.currate;
    status->lowrate = data.lowrate;
    return true;
  }
  return false;
}

bool Interface_Filesystem::io_control_set_cache_rate(void* kodiBase, void* file, uint32_t rate)
{

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_CACHE_SETRATE, &rate) >= 0 ? true
                                                                                            : false;
}

bool Interface_Filesystem::io_control_set_retry(void* kodiBase, void* file, bool retry)
{

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_SET_RETRY, &retry) >= 0 ? true
                                                                                         : false;
}

char** Interface_Filesystem::get_property_values(
    void* kodiBase, void* file, int type, const char* name, int* numValues)
{

  XFILE::FileProperty internalType;
  switch (type)
  {
    case ADDON_FILE_PROPERTY_RESPONSE_PROTOCOL:
      internalType = XFILE::FILE_PROPERTY_RESPONSE_PROTOCOL;
      break;
    case ADDON_FILE_PROPERTY_RESPONSE_HEADER:
      internalType = XFILE::FILE_PROPERTY_RESPONSE_HEADER;
      break;
    case ADDON_FILE_PROPERTY_CONTENT_TYPE:
      internalType = XFILE::FILE_PROPERTY_CONTENT_TYPE;
      break;
    case ADDON_FILE_PROPERTY_CONTENT_CHARSET:
      internalType = XFILE::FILE_PROPERTY_CONTENT_CHARSET;
      break;
    case ADDON_FILE_PROPERTY_MIME_TYPE:
      internalType = XFILE::FILE_PROPERTY_MIME_TYPE;
      break;
    case ADDON_FILE_PROPERTY_EFFECTIVE_URL:
      internalType = XFILE::FILE_PROPERTY_EFFECTIVE_URL;
      break;
    default:
      // CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
      //           __FUNCTION__, kodiBase, file);
      return nullptr;
  };
  std::vector<std::string> values =
      static_cast<CFile*>(file)->GetPropertyValues(internalType, name);
  *numValues = values.size();
  char** ret = static_cast<char**>(malloc(sizeof(char*) * values.size()));
  for (int i = 0; i < *numValues; ++i)
  {
    ret[i] = strdup(values[i].c_str());
  }
  return ret;
}

void* Interface_Filesystem::curl_create(void* kodiBase, const char* url)
{

  CFile* file = new CFile;
  if (file->CURLCreate(url))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

bool Interface_Filesystem::curl_add_option(
    void* kodiBase, void* file, int type, const char* name, const char* value)
{

  XFILE::CURLOPTIONTYPE internalType;
  switch (type)
  {
    case ADDON_CURL_OPTION_OPTION:
      internalType = XFILE::CURL_OPTION_OPTION;
      break;
    case ADDON_CURL_OPTION_PROTOCOL:
      internalType = XFILE::CURL_OPTION_PROTOCOL;
      break;
    case ADDON_CURL_OPTION_CREDENTIALS:
      internalType = XFILE::CURL_OPTION_CREDENTIALS;
      break;
    case ADDON_CURL_OPTION_HEADER:
      internalType = XFILE::CURL_OPTION_HEADER;
      break;
    default:
      throw std::logic_error("Interface_Filesystem::curl_add_option - invalid curl option type");
      return false;
  };

  return static_cast<CFile*>(file)->CURLAddOption(internalType, name, value);
}

bool Interface_Filesystem::curl_open(void* kodiBase, void* file, unsigned int flags)
{
  return static_cast<CFile*>(file)->CURLOpen(TranslateFileReadBitsToKodi(flags));
}