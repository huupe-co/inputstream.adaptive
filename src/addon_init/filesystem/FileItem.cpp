/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileItem.h"

#include "URL.h"
#include "filesystem/CurlFile.h"
#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"
#include "utils/log.h"

CFileItem::CFileItem(void)
{
}

CFileItem::CFileItem(const std::string& strPath, bool bIsFolder)
{

  m_strPath = strPath;
  m_bIsFolder = bIsFolder;
  //FillInMimeType(false);
}

CFileItem::CFileItem(const CFileItem& item)
{
  m_strPath = item.GetPath();
  m_bIsFolder = item.m_bIsFolder;
}

bool CFileItem::IsPath(const std::string& path, bool ignoreURLOptions /* = false */) const
{
  return URIUtils::PathEquals(m_strPath, path, false, ignoreURLOptions);
}

CFileItem::~CFileItem(void)
{
}


/////////////////////////////////////////////////////////////////////////////////
/////
///// CFileItemList
/////
//////////////////////////////////////////////////////////////////////////////////

CFileItemList::CFileItemList() : CFileItem("", true)
{
}

CFileItemList::CFileItemList(const std::string& strPath) : CFileItem(strPath, true)
{
}

CFileItemList::~CFileItemList()
{
  Clear();
}

CFileItemPtr CFileItemList::operator[](int iItem)
{
  return Get(iItem);
}

const CFileItemPtr CFileItemList::operator[](int iItem) const
{
  return Get(iItem);
}

CFileItemPtr CFileItemList::operator[](const std::string& strPath)
{
  return Get(strPath);
}

const CFileItemPtr CFileItemList::operator[](const std::string& strPath) const
{
  return Get(strPath);
}

void CFileItemList::SetIgnoreURLOptions(bool ignoreURLOptions)
{
  m_ignoreURLOptions = ignoreURLOptions;

  if (m_fastLookup)
  {
    m_fastLookup = false; // Force SetFastlookup to clear map
    SetFastLookup(true); // and regenerate map
  }
}

void CFileItemList::SetFastLookup(bool fastLookup)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (fastLookup && !m_fastLookup)
  { // generate the map
    m_map.clear();
    for (unsigned int i = 0; i < m_items.size(); i++)
    {
      CFileItemPtr pItem = m_items[i];
      m_map.insert(MAPFILEITEMSPAIR(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions()
                                                       : pItem->GetPath(),
                                    pItem));
    }
  }
  if (!fastLookup && m_fastLookup)
    m_map.clear();
  m_fastLookup = fastLookup;
}

bool CFileItemList::Contains(const std::string& fileName) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (m_fastLookup)
    return m_map.find(m_ignoreURLOptions ? CURL(fileName).GetWithoutOptions() : fileName) !=
           m_map.end();

  return false;
}

void CFileItemList::Clear()
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  ClearItems();
}

void CFileItemList::ClearItems()
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  // make sure we free the memory of the items (these are GUIControls which may have allocated resources)
  //FreeMemory();
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    CFileItemPtr item = m_items[i];
    //item->FreeMemory();
  }
  m_items.clear();
  m_map.clear();
}

void CFileItemList::Add(CFileItemPtr pItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  if (m_fastLookup)
    m_map.insert(MAPFILEITEMSPAIR(
        m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath(), pItem));
  m_items.emplace_back(std::move(pItem));
}

void CFileItemList::Add(CFileItem&& item)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  auto ptr = std::make_shared<CFileItem>(std::move(item));
  if (m_fastLookup)
    m_map.insert(MAPFILEITEMSPAIR(
        m_ignoreURLOptions ? CURL(ptr->GetPath()).GetWithoutOptions() : ptr->GetPath(), ptr));
  m_items.emplace_back(std::move(ptr));
}

void CFileItemList::AddFront(const CFileItemPtr& pItem, int itemPosition)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (itemPosition >= 0)
  {
    m_items.insert(m_items.begin() + itemPosition, pItem);
  }
  else
  {
    m_items.insert(m_items.begin() + (m_items.size() + itemPosition), pItem);
  }
  if (m_fastLookup)
  {
    m_map.insert(MAPFILEITEMSPAIR(
        m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath(), pItem));
  }
}

void CFileItemList::Remove(CFileItem* pItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  for (IVECFILEITEMS it = m_items.begin(); it != m_items.end(); ++it)
  {
    if (pItem == it->get())
    {
      m_items.erase(it);
      if (m_fastLookup)
      {
        m_map.erase(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions()
                                       : pItem->GetPath());
      }
      break;
    }
  }
}

VECFILEITEMS::iterator CFileItemList::erase(VECFILEITEMS::iterator first,
                                            VECFILEITEMS::iterator last)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return m_items.erase(first, last);
}

void CFileItemList::Remove(int iItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (iItem >= 0 && iItem < Size())
  {
    CFileItemPtr pItem = *(m_items.begin() + iItem);
    if (m_fastLookup)
    {
      m_map.erase(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions()
                                     : pItem->GetPath());
    }
    m_items.erase(m_items.begin() + iItem);
  }
}

void CFileItemList::Append(const CFileItemList& itemlist)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  for (int i = 0; i < itemlist.Size(); ++i)
    Add(itemlist[i]);
}

void CFileItemList::Assign(const CFileItemList& itemlist, bool append)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  if (!append)
    Clear();
  Append(itemlist);
  SetPath(itemlist.GetPath());
}

bool CFileItemList::Copy(const CFileItemList& items, bool copyItems /* = true */)
{
  // assign all CFileItem parts
  *static_cast<CFileItem*>(this) = static_cast<const CFileItem&>(items);

  // assign the rest of the CFileItemList properties

  if (copyItems)
  {
    // make a copy of each item
    for (int i = 0; i < items.Size(); i++)
    {
      CFileItemPtr newItem(new CFileItem(*items[i]));
      Add(newItem);
    }
  }

  return true;
}

CFileItemPtr CFileItemList::Get(int iItem) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (iItem > -1 && iItem < (int)m_items.size())
    return m_items[iItem];

  return CFileItemPtr();
}

CFileItemPtr CFileItemList::Get(const std::string& strPath) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (m_fastLookup)
  {
    MAPFILEITEMS::const_iterator it =
        m_map.find(m_ignoreURLOptions ? CURL(strPath).GetWithoutOptions() : strPath);
    if (it != m_map.end())
      return it->second;

    return CFileItemPtr();
  }
  // slow method...
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (pItem->IsPath(m_ignoreURLOptions ? CURL(strPath).GetWithoutOptions() : strPath))
      return pItem;
  }

  return CFileItemPtr();
}

int CFileItemList::Size() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return (int)m_items.size();
}

bool CFileItemList::IsEmpty() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return m_items.empty();
}

void CFileItemList::Reserve(size_t iCount)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  m_items.reserve(iCount);
}

int CFileItemList::GetFolderCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  int nFolderCount = 0;
  for (int i = 0; i < (int)m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (pItem->m_bIsFolder)
      nFolderCount++;
  }

  return nFolderCount;
}

int CFileItemList::GetObjectCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  int numObjects = (int)m_items.size();
  // if (numObjects && m_items[0]->IsParentFolder())
  //   numObjects--;

  return numObjects;
}

int CFileItemList::GetFileCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  int nFileCount = 0;
  for (int i = 0; i < (int)m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (!pItem->m_bIsFolder)
      nFileCount++;
  }

  return nFileCount;
}

// int CFileItemList::GetSelectedCount() const
// {
//   std::unique_lock<CCriticalSection> lock(m_lock);
//   int count = 0;
//   for (int i = 0; i < (int)m_items.size(); i++)
//   {
//     CFileItemPtr pItem = m_items[i];
//     if (pItem->IsSelected())
//       count++;
//   }

//   return count;
// }

// void CFileItemList::FilterCueItems()
// {
//   std::unique_lock<CCriticalSection> lock(m_lock);
//   // Handle .CUE sheet files...
//   std::vector<std::string> itemstodelete;
//   for (int i = 0; i < (int)m_items.size(); i++)
//   {
//     CFileItemPtr pItem = m_items[i];
//     if (!pItem->m_bIsFolder)
//     { // see if it's a .CUE sheet
//       if (pItem->IsCUESheet())
//       {
//         CCueDocumentPtr cuesheet(new CCueDocument);
//         if (cuesheet->ParseFile(pItem->GetPath()))
//         {
//           std::vector<std::string> MediaFileVec;
//           cuesheet->GetMediaFiles(MediaFileVec);

//           // queue the cue sheet and the underlying media file for deletion
//           for (std::vector<std::string>::iterator itMedia = MediaFileVec.begin();
//                itMedia != MediaFileVec.end(); ++itMedia)
//           {
//             std::string strMediaFile = *itMedia;
//             std::string fileFromCue =
//                 strMediaFile; // save the file from the cue we're matching against,
//             // as we're going to search for others here...
//             bool bFoundMediaFile = CFile::Exists(strMediaFile);
//             if (!bFoundMediaFile)
//             {
//               // try file in same dir, not matching case...
//               if (Contains(strMediaFile))
//               {
//                 bFoundMediaFile = true;
//               }
//               else
//               {
//                 // try removing the .cue extension...
//                 strMediaFile = pItem->GetPath();
//                 URIUtils::RemoveExtension(strMediaFile);
//                 CFileItem item(strMediaFile, false);
//                 if (item.IsAudio() && Contains(strMediaFile))
//                 {
//                   bFoundMediaFile = true;
//                 }
//                 else
//                 { // try replacing the extension with one of our allowed ones.
//                   std::vector<std::string> extensions = StringUtils::Split(
//                       CServiceBroker::GetFileExtensionProvider().GetMusicExtensions(), "|");
//                   for (std::vector<std::string>::const_iterator i = extensions.begin();
//                        i != extensions.end(); ++i)
//                   {
//                     strMediaFile = URIUtils::ReplaceExtension(pItem->GetPath(), *i);
//                     CFileItem item(strMediaFile, false);
//                     if (!item.IsCUESheet() && !item.IsPlayList() && Contains(strMediaFile))
//                     {
//                       bFoundMediaFile = true;
//                       break;
//                     }
//                   }
//                 }
//               }
//             }
//             if (bFoundMediaFile)
//             {
//               cuesheet->UpdateMediaFile(fileFromCue, strMediaFile);
//               // apply CUE for later processing
//               for (int j = 0; j < (int)m_items.size(); j++)
//               {
//                 CFileItemPtr pItem = m_items[j];
//                 if (StringUtils::CompareNoCase(pItem->GetPath(), strMediaFile) == 0)
//                   pItem->SetCueDocument(cuesheet);
//               }
//             }
//           }
//         }
//         itemstodelete.push_back(pItem->GetPath());
//       }
//     }
//   }
//   // now delete the .CUE files.
//   for (int i = 0; i < (int)itemstodelete.size(); i++)
//   {
//     for (int j = 0; j < (int)m_items.size(); j++)
//     {
//       CFileItemPtr pItem = m_items[j];
//       if (StringUtils::CompareNoCase(pItem->GetPath(), itemstodelete[i]) == 0)
//       { // delete this item
//         m_items.erase(m_items.begin() + j);
//         break;
//       }
//     }
//   }
// }

// // Remove the extensions from the filenames
// void CFileItemList::RemoveExtensions()
// {
//   std::unique_lock<CCriticalSection> lock(m_lock);
//   for (int i = 0; i < Size(); ++i)
//     m_items[i]->RemoveExtension();
// }

// void CFileItemList::Stack(bool stackFiles /* = true */)
// {
//   std::unique_lock<CCriticalSection> lock(m_lock);

//   // not allowed here
//   if (IsVirtualDirectoryRoot() || IsLiveTV() || IsSourcesPath() || IsLibraryFolder())
//     return;

//   SetProperty("isstacked", true);

//   // items needs to be sorted for stuff below to work properly
//   Sort(SortByLabel, SortOrderAscending);

//   StackFolders();

//   if (stackFiles)
//     StackFiles();
// }

// void CFileItemList::StackFolders()
// {
//   // Precompile our REs
//   VECCREGEXP folderRegExps;
//   CRegExp folderRegExp(true, CRegExp::autoUtf8);
//   const std::vector<std::string>& strFolderRegExps =
//       CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_folderStackRegExps;

//   std::vector<std::string>::const_iterator strExpression = strFolderRegExps.begin();
//   while (strExpression != strFolderRegExps.end())
//   {
//     if (!folderRegExp.RegComp(*strExpression))
//       CLog::Log(LOGERROR, "{}: Invalid folder stack RegExp:'{}'", __FUNCTION__,
//                 strExpression->c_str());
//     else
//       folderRegExps.push_back(folderRegExp);

//     ++strExpression;
//   }

//   if (!folderRegExp.IsCompiled())
//   {
//     CLog::Log(LOGDEBUG, "{}: No stack expressions available. Skipping folder stacking",
//               __FUNCTION__);
//     return;
//   }

//   // stack folders
//   for (int i = 0; i < Size(); i++)
//   {
//     CFileItemPtr item = Get(i);
//     // combined the folder checks
//     if (item->m_bIsFolder)
//     {
//       // only check known fast sources?
//       // NOTES:
//       // 1. rars and zips may be on slow sources? is this supposed to be allowed?
//       if (!item->IsRemote() || item->IsSmb() || item->IsNfs() ||
//           URIUtils::IsInRAR(item->GetPath()) || URIUtils::IsInZIP(item->GetPath()) ||
//           URIUtils::IsOnLAN(item->GetPath()))
//       {
//         // stack cd# folders if contains only a single video file

//         bool bMatch(false);

//         VECCREGEXP::iterator expr = folderRegExps.begin();
//         while (!bMatch && expr != folderRegExps.end())
//         {
//           //CLog::Log(LOGDEBUG,"{}: Running expression {} on {}", __FUNCTION__, expr->GetPattern(), item->GetLabel());
//           bMatch = (expr->RegFind(item->GetLabel().c_str()) != -1);
//           if (bMatch)
//           {
//             CFileItemList items;
//             CDirectory::GetDirectory(
//                 item->GetPath(), items,
//                 CServiceBroker::GetFileExtensionProvider().GetVideoExtensions(), DIR_FLAG_DEFAULTS);
//             // optimized to only traverse listing once by checking for filecount
//             // and recording last file item for later use
//             int nFiles = 0;
//             int index = -1;
//             for (int j = 0; j < items.Size(); j++)
//             {
//               if (!items[j]->m_bIsFolder)
//               {
//                 nFiles++;
//                 index = j;
//               }

//               if (nFiles > 1)
//                 break;
//             }

//             if (nFiles == 1)
//               *item = *items[index];
//           }
//           ++expr;
//         }

//         // check for dvd folders
//         if (!bMatch)
//         {
//           std::string dvdPath = item->GetOpticalMediaPath();

//           if (!dvdPath.empty())
//           {
//             // NOTE: should this be done for the CD# folders too?
//             item->m_bIsFolder = false;
//             item->SetPath(dvdPath);
//             item->SetLabel2("");
//             item->SetLabelPreformatted(true);
//             m_sortDescription.sortBy = SortByNone; /* sorting is now broken */
//           }
//         }
//       }
//     }
//   }
// }

// void CFileItemList::StackFiles()
// {
//   // Precompile our REs
//   VECCREGEXP stackRegExps;
//   CRegExp tmpRegExp(true, CRegExp::autoUtf8);
//   const std::vector<std::string>& strStackRegExps =
//       CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoStackRegExps;
//   std::vector<std::string>::const_iterator strRegExp = strStackRegExps.begin();
//   while (strRegExp != strStackRegExps.end())
//   {
//     if (tmpRegExp.RegComp(*strRegExp))
//     {
//       if (tmpRegExp.GetCaptureTotal() == 4)
//         stackRegExps.push_back(tmpRegExp);
//       else
//         CLog::Log(LOGERROR, "Invalid video stack RE ({}). Must have 4 captures.",
//                   strRegExp->c_str());
//     }
//     ++strRegExp;
//   }

//   // now stack the files, some of which may be from the previous stack iteration
//   int i = 0;
//   while (i < Size())
//   {
//     CFileItemPtr item1 = Get(i);

//     // skip folders, nfo files, playlists
//     if (item1->m_bIsFolder || item1->IsParentFolder() || item1->IsNFO() || item1->IsPlayList())
//     {
//       // increment index
//       i++;
//       continue;
//     }

//     int64_t size = 0;
//     size_t offset = 0;
//     std::string stackName;
//     std::string file1;
//     std::string filePath;
//     std::vector<int> stack;
//     VECCREGEXP::iterator expr = stackRegExps.begin();

//     URIUtils::Split(item1->GetPath(), filePath, file1);
//     if (URIUtils::HasEncodedFilename(CURL(filePath)))
//       file1 = CURL::Decode(file1);

//     int j;
//     while (expr != stackRegExps.end())
//     {
//       if (expr->RegFind(file1, offset) != -1)
//       {
//         std::string Title1 = expr->GetMatch(1), Volume1 = expr->GetMatch(2),
//                     Ignore1 = expr->GetMatch(3), Extension1 = expr->GetMatch(4);
//         if (offset)
//           Title1 = file1.substr(0, expr->GetSubStart(2));
//         j = i + 1;
//         while (j < Size())
//         {
//           CFileItemPtr item2 = Get(j);

//           // skip folders, nfo files, playlists
//           if (item2->m_bIsFolder || item2->IsParentFolder() || item2->IsNFO() ||
//               item2->IsPlayList())
//           {
//             // increment index
//             j++;
//             continue;
//           }

//           std::string file2, filePath2;
//           URIUtils::Split(item2->GetPath(), filePath2, file2);
//           if (URIUtils::HasEncodedFilename(CURL(filePath2)))
//             file2 = CURL::Decode(file2);

//           if (expr->RegFind(file2, offset) != -1)
//           {
//             std::string Title2 = expr->GetMatch(1), Volume2 = expr->GetMatch(2),
//                         Ignore2 = expr->GetMatch(3), Extension2 = expr->GetMatch(4);
//             if (offset)
//               Title2 = file2.substr(0, expr->GetSubStart(2));
//             if (StringUtils::EqualsNoCase(Title1, Title2))
//             {
//               if (!StringUtils::EqualsNoCase(Volume1, Volume2))
//               {
//                 if (StringUtils::EqualsNoCase(Ignore1, Ignore2) &&
//                     StringUtils::EqualsNoCase(Extension1, Extension2))
//                 {
//                   if (stack.empty())
//                   {
//                     stackName = Title1 + Ignore1 + Extension1;
//                     stack.push_back(i);
//                     size += item1->m_dwSize;
//                   }
//                   stack.push_back(j);
//                   size += item2->m_dwSize;
//                 }
//                 else // Sequel
//                 {
//                   offset = 0;
//                   ++expr;
//                   break;
//                 }
//               }
//               else if (!StringUtils::EqualsNoCase(Ignore1,
//                                                   Ignore2)) // False positive, try again with offset
//               {
//                 offset = expr->GetSubStart(3);
//                 break;
//               }
//               else // Extension mismatch
//               {
//                 offset = 0;
//                 ++expr;
//                 break;
//               }
//             }
//             else // Title mismatch
//             {
//               offset = 0;
//               ++expr;
//               break;
//             }
//           }
//           else // No match 2, next expression
//           {
//             offset = 0;
//             ++expr;
//             break;
//           }
//           j++;
//         }
//         if (j == Size())
//           expr = stackRegExps.end();
//       }
//       else // No match 1
//       {
//         offset = 0;
//         ++expr;
//       }
//       if (stack.size() > 1)
//       {
//         // have a stack, remove the items and add the stacked item
//         // dont actually stack a multipart rar set, just remove all items but the first
//         std::string stackPath;
//         if (Get(stack[0])->IsRAR())
//           stackPath = Get(stack[0])->GetPath();
//         else
//         {
//           CStackDirectory dir;
//           stackPath = dir.ConstructStackPath(*this, stack);
//         }
//         item1->SetPath(stackPath);
//         // clean up list
//         for (unsigned k = 1; k < stack.size(); k++)
//           Remove(i + 1);
//         // item->m_bIsFolder = true;  // don't treat stacked files as folders
//         // the label may be in a different char set from the filename (eg over smb
//         // the label is converted from utf8, but the filename is not)
//         if (!CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
//                 CSettings::SETTING_FILELISTS_SHOWEXTENSIONS))
//           URIUtils::RemoveExtension(stackName);

//         item1->SetLabel(stackName);
//         item1->m_dwSize = size;
//         break;
//       }
//     }
//     i++;
//   }
// }
