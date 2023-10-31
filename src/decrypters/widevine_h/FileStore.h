#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// File class. The implementation is platform dependent.
class File {
 public:
  File() {}
  virtual ~File() {}
  virtual ssize_t Read(char* buffer, size_t bytes) = 0;
  virtual ssize_t Write(const char* buffer, size_t bytes) = 0;

  friend class FileSystem;
};

class FileSystem {
 public:
  FileSystem();
  FileSystem(const std::string& origin, void* extra_data);
  virtual ~FileSystem();

  class Impl;

  // defines as bit flag
  enum OpenFlags {
    kNoFlags = 0,
    kCreate = 1,
    kReadOnly = 2,  // defaults to read and write access
    kTruncate = 4
  };

  virtual std::unique_ptr<File> Open(const std::string& file_path, int flags);

  virtual bool Exists(const std::string& file_path);
  virtual bool Remove(const std::string& file_path);
  virtual ssize_t FileSize(const std::string& file_path);

  // Return the filenames stored at dir_path.
  // dir_path will be stripped from the returned names.
  virtual bool List(const std::string& dir_path,
                    std::vector<std::string>* names);

  const std::string& origin() const { return origin_; }
  void set_origin(const std::string& origin);

  const std::string& identifier() const { return identifier_; }
  void set_identifier(const std::string& identifier);
  bool IsGlobal() const { return identifier_.empty(); }

 private:
  std::unique_ptr<FileSystem::Impl> impl_;
  std::string origin_;
  std::string identifier_;
};