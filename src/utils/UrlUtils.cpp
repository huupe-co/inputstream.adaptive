/*
 *  Copyright (C) 2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "UrlUtils.h"

#include "StringUtils.h"

#include "kodi/tools/StringUtils.h"

using namespace UTILS::URL;
using namespace kodi::tools;

namespace
{
constexpr std::string_view PREFIX_SINGLE_DOT{"./"};
constexpr std::string_view PREFIX_DOUBLE_DOT{"../"};

bool isUrl(std::string url,
           bool allowFragments,
           bool allowQueryParams,
           bool validateLenght,
           bool validateProtocol,
           bool requireProtocol,
           bool allowRelativeUrls)
{
  if (url.empty())
    return false;

  if (validateLenght && url.size() >= 2083)
    return false;

  if (!allowFragments && url.find('#') != std::string::npos)
    return false;

  if (!allowQueryParams &&
      (url.find('?') != std::string::npos || url.find('&') != std::string::npos))
    return false;

  size_t paramPos = url.find('#');
  if (paramPos != std::string::npos)
    url.resize(paramPos);

  paramPos = url.find('?');
  if (paramPos != std::string::npos)
    url.resize(paramPos);

  paramPos = url.find("://");
  if (paramPos != std::string::npos)
  {
    if (validateProtocol)
    {
      std::string protocol{url.substr(0, paramPos)};
      if (protocol != "http" && protocol != "https")
        return false;
    }
    url = url.substr(paramPos + 3);
  }
  else if (requireProtocol)
  {
    return false;
  }
  else if (url.compare(0, 1, "/") == 0)
  {
    if (!allowRelativeUrls)
      return false;

    url = url.substr(1);
  }

  if (url.empty())
    return false;

  return true;
}

/*
 * \brief Remove and resolve special dot's from the end of the url.
 *        e.g. "http://foo.bar/sub1/sub2/.././" will result "http://foo.bar/sub1/"
 */
std::string RemoveDotSegments(std::string url)
{
  // Count amount of special prefixes with double dots on the right side
  size_t numSegsRemove{0};
  size_t currPos{0};
  size_t startPos{url.size() - 2};
  while ((currPos = url.rfind("/", startPos)) != std::string::npos)
  {
    // Stop to ignore "/../" from the start of string, e.g. ignored --> "../../something/../" <-- handled
    if (url.substr(currPos + 1, startPos - currPos + 1) != PREFIX_DOUBLE_DOT)
      break;
    startPos = currPos - 1;
    numSegsRemove++;
  }

  // Remove special prefixes
  UTILS::STRING::ReplaceAll(url, PREFIX_DOUBLE_DOT, "");
  UTILS::STRING::ReplaceAll(url, PREFIX_SINGLE_DOT, "");

  size_t addrsStartPos{0};
  if (IsUrlAbsolute(url))
    addrsStartPos = url.find("://") + 3;
  else if (IsUrlRelativeLevel(url))
    addrsStartPos = 3;

  // Remove segments from the end (if any)
  for (; numSegsRemove > 0; numSegsRemove--)
  {
    std::size_t lastSlashPos = url.find_last_of('/', url.size() - 2);
    if ((lastSlashPos + 1) == addrsStartPos)
      break;
    url = url.substr(0, lastSlashPos + 1);
  }

  return url;
}

} // unnamed namespace

bool UTILS::URL::IsValidUrl(const std::string& url)
{
  return isUrl(url, false, true, true, true, true, false);
}

bool UTILS::URL::IsUrlAbsolute(std::string_view url)
{
  return (url.compare(0, 7, "http://") == 0 || url.compare(0, 8, "https://") == 0);
}

bool UTILS::URL::IsUrlRelative(std::string_view url)
{
  return !IsUrlAbsolute(url);
}

bool UTILS::URL::IsUrlRelativeLevel(std::string_view url)
{
  return (url.compare(0, 3, PREFIX_DOUBLE_DOT) == 0);
}

std::string UTILS::URL::GetParametersFromPlaceholder(std::string& url, std::string_view placeholder)
{
  std::string::size_type phPos = url.find(placeholder);
  if (phPos != std::string::npos)
  {
    while (phPos && url[phPos] != '&' && url[phPos] != '?')
    {
      --phPos;
    }
    if (phPos > 0)
      return url.substr(phPos);
  }
  return "";
}

std::string UTILS::URL::GetParameters(std::string& url) {
  size_t paramsPos = url.find('?');
  if (paramsPos != std::string::npos)
    return url.substr(paramsPos + 1);

  return "";
}

std::string UTILS::URL::RemoveParameters(std::string url)
{
  size_t paramsPos = url.find('?');
  if (paramsPos != std::string::npos)
    url.resize(paramsPos);

  return url;
}

std::string UTILS::URL::GetUrlPath(std::string url)
{
  if (url.empty())
    return url;

  size_t paramsPos = url.find('?');
  if (paramsPos != std::string::npos)
    url.resize(paramsPos);

  // The part of the base url after last / is not a directory so will not be taken into account
  if (url.back() != '/')
  {
    size_t slashPos = url.rfind("/");
    if (slashPos > url.find("://") + 3)
      url.erase(slashPos + 1);
  }

  return url;
}

void UTILS::URL::AppendParameters(std::string& url, std::string_view params)
{
  if (params.empty())
    return;

  if (url.find_first_of('?') == std::string::npos)
    url += "?";
  else
    url += "&";

  url += params.substr(params.front() == '&' || params.front() == '?' ? 1 : 0);
}

std::string UTILS::URL::GetBaseDomain(std::string url)
{
  if (IsUrlAbsolute(url))
  {
    const size_t paramsPos = url.find('?');
    if (paramsPos != std::string::npos)
      url.erase(paramsPos);

    const size_t domainStartPos = url.find("://") + 3;
    // Try remove url port number and path
    const size_t port = url.find_first_of(':', domainStartPos);
    if (port != std::string::npos)
      url.erase(port);
    else
    {
      // Try remove the path
      const size_t slashPos = url.find_first_of('/', domainStartPos);
      if (slashPos != std::string::npos)
        url.erase(slashPos);
    }
    return url;
  }
  return "";
}

std::string UTILS::URL::Join(std::string baseUrl, std::string relativeUrl)
{
  if (baseUrl.empty())
    return relativeUrl;

  if (relativeUrl.empty())
    return baseUrl;

  if (relativeUrl == ".") // Ignore single dot
    relativeUrl.clear();
  else if (relativeUrl.compare(0, 2, PREFIX_SINGLE_DOT) == 0) // Ignore prefix ./
    relativeUrl.erase(0, 2);

  // Sanitize for missing backslash
  if (relativeUrl == ".." || StringUtils::EndsWith(relativeUrl, "/.."))
    relativeUrl += "/";

  // The part of the base url after last / is not a directory so will not be taken into account
  if (baseUrl.back() != '/')
  {
    size_t slashPos = baseUrl.rfind("/");
    if (slashPos > baseUrl.find("://") + 3)
      baseUrl.erase(slashPos + 1);
  }

  if (baseUrl.back() != '/')
    baseUrl += "/";

  bool skipRemovingSegs{true};

  // Check if relative to domain
  if (!relativeUrl.empty() && relativeUrl.front() == '/')
  {
    skipRemovingSegs = false;
    relativeUrl.erase(0, 1);
    std::string domain = GetBaseDomain(baseUrl);
    if (!domain.empty())
      baseUrl = domain + "/";
  }

  if (IsUrlRelativeLevel(relativeUrl))
  {
    // Remove segments from the end of base url,
    // based on the initial prefixes "../" on the relativeUrl url
    size_t currPos{0};
    size_t startPos{0};
    while ((currPos = relativeUrl.find("/", startPos)) != std::string::npos)
    {
      // Stop to ignore "/../" from the end of string, e.g. handled --> "../../something/../" <-- ignored
      if (relativeUrl.substr(startPos, currPos + 1 - startPos) != PREFIX_DOUBLE_DOT)
        break;
      startPos = currPos + 1;
    }

    if (skipRemovingSegs)
      baseUrl = RemoveDotSegments(baseUrl + relativeUrl.substr(0, startPos));

    relativeUrl.erase(0, startPos);
  }

  return RemoveDotSegments(baseUrl + relativeUrl);
}

void UTILS::URL::EnsureEndingBackslash(std::string& url)
{
  if (!url.empty() && url.back() != '/')
    url += "/";
}
