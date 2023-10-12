input_adaptive_includes += include_directories('.')

# subdir('codechandler')
subdir('common')
# subdir('decrypters')
subdir('parser')
subdir('samplereader')
subdir('utils')

subdir('codechandler')
subdir('addon_init')

input_adaptive_sources += files(
  'AdaptiveByteStream.cpp',
  'InputAdaptiveMain.cpp',
  'oscompat.cpp',
  'Session.cpp',
  'Stream.cpp',
  'TSReader.cpp',
  'aes_decrypter.cpp',
  'ADTSReader.cpp',
  'WebmReader.cpp',
)

input_adaptive_export_includes = [
  include_directories('.'),
  include_directories('addon_init')
]