#pragma once

struct initAddonParams
{
  void (*log_callback)(int level, const char* msg);
};

void initAddon(const initAddonParams& params);