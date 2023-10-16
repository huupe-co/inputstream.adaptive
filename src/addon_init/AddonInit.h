#pragma once

#include <kodi/addon-instance/Inputstream.h>

struct initAddonParams
{
  void (*log_callback)(int level, const char* msg);
};

class CInputStreamAdaptive;

void initAddon(const initAddonParams& params);