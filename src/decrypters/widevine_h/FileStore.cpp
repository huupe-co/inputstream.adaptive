#include "FileStore.h"

#include <cstring>
#include <errno.h>
#include <memory>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#include <gst/gst.h>
#include "utils/log.h"

namespace
{
const char kCurrentDirectory[] = ".";
const char kParentDirectory[] = "..";
const char kDirectoryDelimiter = '/';
const char kWildcard[] = "*";

bool IsCurrentOrParentDirectory(char* dir)
{
  return strcmp(dir, kCurrentDirectory) == 0 || strcmp(dir, kParentDirectory) == 0;
}

bool IsDirectory(const std::string& path)
{
  struct stat buf;
  if (stat(path.c_str(), &buf) == 0)
    return buf.st_mode & S_IFDIR;
  else
    return false;
}

bool CreateDirectory(const std::string& path_in)
{
  std::string path = path_in;
  size_t size = path.size();
  if ((size == 1) && (path[0] == kDirectoryDelimiter))
    return true;

  if (size <= 1)
    return false;

  size_t pos = path.find(kDirectoryDelimiter, 1);
  while (pos < size)
  {
    path[pos] = '\0';
    if (mkdir(path.c_str(), 0700) != 0)
    {
      if (errno != EEXIST)
      {
        LOG::Log(LOGINFO, "File::CreateDirectory: mkdir failed: %d, %s", errno, strerror(errno));
        return false;
      }
    }
    path[pos] = kDirectoryDelimiter;
    pos = path.find(kDirectoryDelimiter, pos + 1);
  }

  if (path[size - 1] != kDirectoryDelimiter)
  {
    if (mkdir(path.c_str(), 0700) != 0)
    {
      if (errno != EEXIST)
      {
        LOG::Log(LOGINFO, "File::CreateDirectory: mkdir failed: %d, %s", errno, strerror(errno));
        return false;
      }
    }
  }
  return true;
}
} // namespace

class FileImpl : public File
{
public:
  FileImpl() {}

  void FlushFile()
  {
    fflush(file_);
    fsync(fileno(file_));
  }

  ~FileImpl() override
  {
    if (file_)
    {
      FlushFile();
      fclose(file_);
      file_ = nullptr;
    }
  }

  ssize_t Read(char* buffer, size_t bytes) override
  {
    if (!buffer)
    {
      LOG::Log(LOGINFO, "File::Read: buffer is empty");
      return -1;
    }
    if (!file_)
    {
      LOG::Log(LOGINFO, "File::Read: file not open");
      return -1;
    }
    size_t len = fread(buffer, sizeof(char), bytes, file_);
    if (len != bytes)
    {
      LOG::Log(LOGINFO, "File::Read: fread failed: %d, %s", errno, strerror(errno));
    }
    return len;
  }

  ssize_t Write(const char* buffer, size_t bytes) override
  {
    if (!buffer)
    {
      LOG::Log(LOGINFO, "File::Write: buffer is empty");
      return -1;
    }
    if (!file_)
    {
      LOG::Log(LOGINFO, "File::Write: file not open");
      return -1;
    }
    size_t len = fwrite(buffer, sizeof(char), bytes, file_);
    if (len != bytes)
    {
      LOG::Log(LOGINFO, "File::Write: fwrite failed: %d, %s", errno, strerror(errno));
    }
    FlushFile();
    return len;
  }

  FILE* file_;
  std::string file_path_;
};

class FileSystem::Impl
{
};

FileSystem::FileSystem()
{
}
FileSystem::FileSystem(const std::string& origin, void*) : origin_(origin)
{
}
FileSystem::~FileSystem()
{
}

std::unique_ptr<File> FileSystem::Open(const std::string& name, int flags)
{
  std::string open_flags;

  // create the enclosing directory if it does not exist
  size_t delimiter_pos = name.rfind(kDirectoryDelimiter);
  if (delimiter_pos != std::string::npos)
  {
    std::string dir_path = name.substr(0, delimiter_pos);
    if ((flags & FileSystem::kCreate) && !Exists(dir_path))
      CreateDirectory(dir_path);
  }

  // ensure only owners has access
  mode_t old_mask = umask(077);
  if (((flags & FileSystem::kTruncate) && Exists(name)) ||
      ((flags & FileSystem::kCreate) && !Exists(name)))
  {
    FILE* fp = fopen(name.c_str(), "w+");
    if (fp)
    {
      fclose(fp);
    }
  }

  open_flags = (flags & FileSystem::kReadOnly) ? "rb" : "rb+";

  std::unique_ptr<FileImpl> file_impl(new FileImpl());
  file_impl->file_ = fopen(name.c_str(), open_flags.c_str());
  umask(old_mask);
  if (!file_impl->file_)
  {
    LOG::Log(LOGINFO, "File::Open: fopen failed: %d, %s", errno, strerror(errno));
    return nullptr;
  }
  file_impl->file_path_ = name;
  return file_impl;
}

bool FileSystem::Exists(const std::string& path)
{
  struct stat buf;
  int res = stat(path.c_str(), &buf) == 0;
  if (!res)
  {
    LOG::Log(LOGINFO, "File::Exists: stat failed: %d, %s", errno, strerror(errno));
  }
  return res;
}

bool FileSystem::Remove(const std::string& path)
{
  if (IsDirectory(path))
  {
    // Handle directory deletion
    DIR* dir;
    if ((dir = opendir(path.c_str())) != nullptr)
    {
      // first remove files and dir within it
      struct dirent* entry;
      while ((entry = readdir(dir)) != nullptr)
      {
        if (!IsCurrentOrParentDirectory(entry->d_name))
        {
          std::string path_to_remove = path + kDirectoryDelimiter;
          path_to_remove += entry->d_name;
          if (!Remove(path_to_remove))
          {
            closedir(dir);
            return false;
          }
        }
      }
      closedir(dir);
    }
    if (rmdir(path.c_str()))
    {
      LOG::Log(LOGINFO, "File::Remove: rmdir failed: %d, %s", errno, strerror(errno));
      return false;
    }
    return true;
  }
  else
  {
    size_t wildcard_pos = path.find(kWildcard);
    if (wildcard_pos == std::string::npos)
    {
      // Handle file deletion
      if (unlink(path.c_str()) && (errno != ENOENT))
      {
        LOG::Log(LOGINFO, "File::Remove: unlink failed: %d, %s", errno, strerror(errno));
        return false;
      }
    }
    else
    {
      // Handle wildcard specified file deletion
      size_t delimiter_pos = path.rfind(kDirectoryDelimiter, wildcard_pos);
      if (delimiter_pos == std::string::npos)
      {
        LOG::Log(LOGINFO, "File::Remove: unable to find path delimiter before wildcard");
        return false;
      }

      DIR* dir;
      std::string dir_path = path.substr(0, delimiter_pos);
      if ((dir = opendir(dir_path.c_str())) == nullptr)
      {
        LOG::Log(LOGINFO, "File::Remove: directory open failed for wildcard");
        return false;
      }

      struct dirent* entry;
      std::string ext = path.substr(wildcard_pos + 1);

      while ((entry = readdir(dir)) != nullptr)
      {
        size_t filename_len = strlen(entry->d_name);
        if (filename_len > ext.size())
        {
          if (strcmp(entry->d_name + filename_len - ext.size(), ext.c_str()) == 0)
          {
            std::string file_path_to_remove = dir_path + kDirectoryDelimiter + entry->d_name;
            if (!Remove(file_path_to_remove))
            {
              closedir(dir);
              return false;
            }
          }
        }
      }
      closedir(dir);
    }
    return true;
  }
}

ssize_t FileSystem::FileSize(const std::string& path)
{
  struct stat buf;
  if (stat(path.c_str(), &buf) == 0)
    return buf.st_size;
  else
    return -1;
}

// Accept a directory, return all the files in that directory.
// Returns false if the directory does not exist.
bool FileSystem::List(const std::string& dirpath, std::vector<std::string>* filenames)
{
  if (filenames == nullptr)
  {
    LOG::Log(LOGINFO, "FileSystem::List: destination not provided");
    return false;
  }
  if (!Exists(dirpath))
  {
    LOG::Log(LOGINFO, "FileSystem::List: path %s does not exist: %d, %s", dirpath.c_str(), errno,
             strerror(errno));
    return false;
  }

  DIR* dir = opendir(dirpath.c_str());
  if (dir == nullptr)
  {
    LOG::Log(LOGINFO, "FileSystem::List: directory open failed %s: %d, %s", dirpath.c_str(), errno,
             strerror(errno));
    return false;
  }

  filenames->clear();
  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr)
  {
    if (!IsCurrentOrParentDirectory(entry->d_name))
    {
      filenames->push_back(entry->d_name);
    }
  }
  closedir(dir);

  return true;
}

void FileSystem::set_origin(const std::string& origin)
{
  origin_ = origin;
}

void FileSystem::set_identifier(const std::string& identifier)
{
  identifier_ = identifier;
}
