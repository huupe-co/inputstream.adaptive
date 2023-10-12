#include "AddonInit.h"

#include "filesystem/Filesystem.h"

#include <kodi/addon-instance/Inputstream.h>

struct FuncTable
{
  static void addon_log_msg(const KODI_ADDON_BACKEND_HDL hdl,
                            const int addonLogLevel,
                            const char* strMessage);
  static char* get_type_version(const KODI_ADDON_BACKEND_HDL hdl, int type);
  static char* get_addon_path(const KODI_ADDON_BACKEND_HDL hdl);
  static char* get_lib_path(const KODI_ADDON_BACKEND_HDL hdl);
  static char* get_user_path(const KODI_ADDON_BACKEND_HDL hdl);
  static char* get_temp_path(const KODI_ADDON_BACKEND_HDL hdl);
  static char* get_localized_string(const KODI_ADDON_BACKEND_HDL hdl, long label_id);
  static char* get_addon_info(const KODI_ADDON_BACKEND_HDL hdl, const char* id);
  static bool open_settings_dialog(const KODI_ADDON_BACKEND_HDL hdl);
  static bool is_setting_using_default(const KODI_ADDON_BACKEND_HDL hdl, const char* id);
  static bool get_setting_bool(const KODI_ADDON_BACKEND_HDL hdl, const char* id, bool* value);
  static bool get_setting_int(const KODI_ADDON_BACKEND_HDL hdl, const char* id, int* value);
  static bool get_setting_float(const KODI_ADDON_BACKEND_HDL hdl, const char* id, float* value);
  static bool get_setting_string(const KODI_ADDON_BACKEND_HDL hdl, const char* id, char** value);
  static bool set_setting_bool(const KODI_ADDON_BACKEND_HDL hdl, const char* id, bool value);
  static bool set_setting_int(const KODI_ADDON_BACKEND_HDL hdl, const char* id, int value);
  static bool set_setting_float(const KODI_ADDON_BACKEND_HDL hdl, const char* id, float value);
  static bool set_setting_string(const KODI_ADDON_BACKEND_HDL hdl,
                                 const char* id,
                                 const char* value);
  static void free_string(const KODI_ADDON_BACKEND_HDL hdl, char* str);
  static void free_string_array(const KODI_ADDON_BACKEND_HDL hdl, char** arr, int numElements);
  static void* get_interface(const KODI_ADDON_BACKEND_HDL hdl,
                             const char* name,
                             const char* version);
};

static initAddonParams globalParams;

void initAddon(const initAddonParams& params)
{
  globalParams = params;
  static AddonGlobalInterface addonInterface;
  static AddonToKodiFuncTable_Addon toKodi;
  addonInterface.toKodi = &toKodi;
  //toKodi.kodiBase = addon;
  toKodi.addon_log_msg = FuncTable::addon_log_msg;
  toKodi.free_string = FuncTable::free_string;
  toKodi.free_string_array = FuncTable::free_string_array;

  toKodi.kodi_addon = new AddonToKodiFuncTable_kodi_addon();
  toKodi.kodi_addon->get_addon_path = FuncTable::get_addon_path;
  toKodi.kodi_addon->get_lib_path = FuncTable::get_lib_path;
  toKodi.kodi_addon->get_user_path = FuncTable::get_user_path;
  toKodi.kodi_addon->get_temp_path = FuncTable::get_temp_path;
  toKodi.kodi_addon->get_localized_string = FuncTable::get_localized_string;
  toKodi.kodi_addon->open_settings_dialog = FuncTable::open_settings_dialog;
  toKodi.kodi_addon->is_setting_using_default = FuncTable::is_setting_using_default;
  toKodi.kodi_addon->get_setting_bool = FuncTable::get_setting_bool;
  toKodi.kodi_addon->get_setting_int = FuncTable::get_setting_int;
  toKodi.kodi_addon->get_setting_float = FuncTable::get_setting_float;
  toKodi.kodi_addon->get_setting_string = FuncTable::get_setting_string;
  toKodi.kodi_addon->set_setting_bool = FuncTable::set_setting_bool;
  toKodi.kodi_addon->set_setting_int = FuncTable::set_setting_int;
  toKodi.kodi_addon->set_setting_float = FuncTable::set_setting_float;
  toKodi.kodi_addon->set_setting_string = FuncTable::set_setting_string;
  toKodi.kodi_addon->get_addon_info = FuncTable::get_addon_info;
  toKodi.kodi_addon->get_type_version = FuncTable::get_type_version;
  toKodi.kodi_addon->get_interface = FuncTable::get_interface;

  Interface_Filesystem::Init(&addonInterface);
  kodi::addon::CPrivateBase::m_interface = &addonInterface;
}


char* FuncTable::get_type_version(const KODI_ADDON_BACKEND_HDL hdl, int type)
{
  return strdup(kodi::addon::GetTypeVersion(type));
}

char* FuncTable::get_addon_path(const KODI_ADDON_BACKEND_HDL hdl)
{
  return "/mnt/data/input.adaptive";
}

char* FuncTable::get_lib_path(const KODI_ADDON_BACKEND_HDL hdl)
{
  return "/mnt/data/input.adaptive/libs";
}

char* FuncTable::get_user_path(const KODI_ADDON_BACKEND_HDL hdl)
{
  return "/mnt/data/input.adaptive/user";
}

char* FuncTable::get_temp_path(const KODI_ADDON_BACKEND_HDL hdl)
{
  return "/tmp/input.adaptive/";
}

char* FuncTable::get_localized_string(const KODI_ADDON_BACKEND_HDL hdl, long label_id)
{
  return "";
}

char* FuncTable::get_addon_info(const KODI_ADDON_BACKEND_HDL hdl, const char* id)
{
  return "";
}

bool FuncTable::open_settings_dialog(const KODI_ADDON_BACKEND_HDL hdl)
{
  return false;
}

bool FuncTable::is_setting_using_default(const KODI_ADDON_BACKEND_HDL hdl, const char* id)
{
  return true;
}

bool FuncTable::get_setting_bool(const KODI_ADDON_BACKEND_HDL hdl, const char* id, bool* value)
{
  return false;
}

bool FuncTable::get_setting_int(const KODI_ADDON_BACKEND_HDL hdl, const char* id, int* value)
{
  return false;
}

bool FuncTable::get_setting_float(const KODI_ADDON_BACKEND_HDL hdl, const char* id, float* value)
{
  return false;
}

bool FuncTable::get_setting_string(const KODI_ADDON_BACKEND_HDL hdl, const char* id, char** value)
{
  return false;
}

bool FuncTable::set_setting_bool(const KODI_ADDON_BACKEND_HDL hdl, const char* id, bool value)
{
  return false;
}

bool FuncTable::set_setting_int(const KODI_ADDON_BACKEND_HDL hdl, const char* id, int value)
{

  return false;
}

bool FuncTable::set_setting_float(const KODI_ADDON_BACKEND_HDL hdl, const char* id, float value)
{
  return false;
}

bool FuncTable::set_setting_string(const KODI_ADDON_BACKEND_HDL hdl,
                                   const char* id,
                                   const char* value)
{
  return false;
}

void FuncTable::free_string(const KODI_ADDON_BACKEND_HDL hdl, char* str)
{
  if (str)
    free(str);
}

void FuncTable::free_string_array(const KODI_ADDON_BACKEND_HDL hdl, char** arr, int numElements)
{
  if (arr)
  {
    for (int i = 0; i < numElements; ++i)
    {
      free(arr[i]);
    }
    free(arr);
  }
}

void* FuncTable::get_interface(const KODI_ADDON_BACKEND_HDL hdl,
                               const char* name,
                               const char* version)
{
  return NULL;
}

void FuncTable::addon_log_msg(const KODI_ADDON_BACKEND_HDL hdl,
                              const int addonLogLevel,
                              const char* strMessage)
{
  if (globalParams.log_callback)
  {
    globalParams.log_callback(addonLogLevel, strMessage);
  }
}