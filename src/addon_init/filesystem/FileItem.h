/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

/*!
 \file FileItem.h
 \brief
 */


#include "XBDateTime.h"
#include "threads/CriticalSection.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>


class CURL;
class CVariant;

class CFileItemList;

#define STARTOFFSET_RESUME (-1)


/*!
  \brief Represents a file on a share
  \sa CFileItemList
  */
class CFileItem //: public CGUIListItem, public IArchivable, public ISerializable, public ISortable
{
public:
  CFileItem(void);
  CFileItem(const CFileItem& item);
  CFileItem(const std::string& strPath, bool bIsFolder);
  ~CFileItem(void);

  const std::string& GetPath() const { return m_strPath; }
  void SetPath(const std::string& path) { m_strPath = path; }

  bool IsPath(const std::string& path, bool ignoreURLOptions = false) const;

  int64_t m_dwSize; ///< file size (0 for folders)
  bool m_bIsFolder; ///< is item a folder or a file

private:
  std::string m_strPath; ///< complete path to item
};

/*!
  \brief A shared pointer to CFileItem
  \sa CFileItem
  */
typedef std::shared_ptr<CFileItem> CFileItemPtr;

/*!
  \brief A vector of pointer to CFileItem
  \sa CFileItem
  */
typedef std::vector<CFileItemPtr> VECFILEITEMS;

/*!
  \brief Iterator for VECFILEITEMS
  \sa CFileItemList
  */
typedef std::vector<CFileItemPtr>::iterator IVECFILEITEMS;

/*!
  \brief A map of pointers to CFileItem
  \sa CFileItem
  */
typedef std::map<std::string, CFileItemPtr> MAPFILEITEMS;

/*!
  \brief Pair for MAPFILEITEMS
  \sa MAPFILEITEMS
  */
typedef std::pair<std::string, CFileItemPtr> MAPFILEITEMSPAIR;

typedef bool (*FILEITEMLISTCOMPARISONFUNC)(const CFileItemPtr& pItem1, const CFileItemPtr& pItem2);
typedef void (*FILEITEMFILLFUNC)(CFileItemPtr& item);

/*!
  \brief Represents a list of files
  \sa CFileItemList, CFileItem
  */
class CFileItemList : public CFileItem
{
public:
  enum CACHE_TYPE
  {
    CACHE_NEVER = 0,
    CACHE_IF_SLOW,
    CACHE_ALWAYS
  };

  CFileItemList();
  explicit CFileItemList(const std::string& strPath);
  ~CFileItemList();
  //void Archive(CArchive& ar) override;
  CFileItemPtr operator[](int iItem);
  const CFileItemPtr operator[](int iItem) const;
  CFileItemPtr operator[](const std::string& strPath);
  const CFileItemPtr operator[](const std::string& strPath) const;
  void Clear();
  void ClearItems();
  void Add(CFileItemPtr item);
  void Add(CFileItem&& item);
  void AddFront(const CFileItemPtr& pItem, int itemPosition);
  void Remove(CFileItem* pItem);
  void Remove(int iItem);
  CFileItemPtr Get(int iItem) const;
  const VECFILEITEMS& GetList() const { return m_items; }
  CFileItemPtr Get(const std::string& strPath) const;
  int Size() const;
  bool IsEmpty() const;
  void Append(const CFileItemList& itemlist);
  void Assign(const CFileItemList& itemlist, bool append = false);
  bool Copy(const CFileItemList& item, bool copyItems = true);
  void Reserve(size_t iCount);
  //void Sort(SortBy sortBy, SortOrder sortOrder, SortAttribute sortAttributes = SortAttributeNone);
  /* \brief Sorts the items based on the given sorting options

  In contrast to Sort (see above) this does not change the internal
  state by storing the sorting method and order used and therefore
  will always execute the sorting even if the list of items has
  already been sorted with the same options before.
  */
  //void Sort(SortDescription sortDescription);
  void Randomize();
  void FillInDefaultIcons();
  int GetFolderCount() const;
  int GetFileCount() const;
  int GetSelectedCount() const;
  int GetObjectCount() const;
  void FilterCueItems();
  void RemoveExtensions();
  void SetIgnoreURLOptions(bool ignoreURLOptions);
  void SetFastLookup(bool fastLookup);
  bool Contains(const std::string& fileName) const;
  bool GetFastLookup() const { return m_fastLookup; }

  /*! \brief stack a CFileItemList
   By default we stack all items (files and folders) in a CFileItemList
   \param stackFiles whether to stack all items or just collapse folders (defaults to true)
   \sa StackFiles,StackFolders
   */
  void Stack(bool stackFiles = true);


  void Swap(unsigned int item1, unsigned int item2);

  /*! \brief Update an item in the item list
   \param item the new item, which we match based on path to an existing item in the list
   \return true if the item exists in the list (and was thus updated), false otherwise.
   */
  bool UpdateItem(const CFileItem* item);

  VECFILEITEMS::iterator begin() { return m_items.begin(); }
  VECFILEITEMS::iterator end() { return m_items.end(); }
  VECFILEITEMS::iterator erase(VECFILEITEMS::iterator first, VECFILEITEMS::iterator last);
  VECFILEITEMS::const_iterator begin() const { return m_items.begin(); }
  VECFILEITEMS::const_iterator end() const { return m_items.end(); }
  VECFILEITEMS::const_iterator cbegin() const { return m_items.cbegin(); }
  VECFILEITEMS::const_iterator cend() const { return m_items.cend(); }
  std::reverse_iterator<VECFILEITEMS::const_iterator> rbegin() const { return m_items.rbegin(); }
  std::reverse_iterator<VECFILEITEMS::const_iterator> rend() const { return m_items.rend(); }

private:
  /*!
   \brief stack files in a CFileItemList
   \sa Stack
   */
  void StackFiles();

  /*!
   \brief stack folders in a CFileItemList
   \sa Stack
   */
  void StackFolders();

  VECFILEITEMS m_items;
  MAPFILEITEMS m_map;
  bool m_ignoreURLOptions = false;
  bool m_fastLookup = false;

  mutable CCriticalSection m_lock;
};
