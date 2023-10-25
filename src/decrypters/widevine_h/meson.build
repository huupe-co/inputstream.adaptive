if meson.is_cross_build()
  input_adaptive_includes += include_directories('/mnt/data/huupe-os/work/build/tmp/work/armv8a-oe4t-linux/gstreamer1.0-plugins-wvdecrypt/0.1.0-r0/recipe-sysroot/usr/include/')
endif

input_adaptive_includes += include_directories('.')

input_adaptive_sources += files(
  'WVCencSingleSampleDecrypter.cpp',
  'WVCdmAdapter.cpp',
  'WVDecrypter.cpp',
  'jsmn.c', 
  'HostImplementation.cpp',
  'FileStore.cpp',
  'Provision.cpp'
)
