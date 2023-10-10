input_adaptive_includes = [
  './lib_adapter'
]

link_libs = []
input_adaptive_sources = []

subdir('lib/Bento4/Source/C++')
subdir('src')

subdir('lib/webm_parser')
link_libs += webm_parser_lib
input_adaptive_includes += ['lib/webm_parser/include']

subdir('lib/mpegts')
link_libs += mpegts_lib
input_adaptive_includes += ['lib/mpegts']

input_adaptive_lib = static_library(
  'input_adaptive',
  input_adaptive_sources,
  include_directories: input_adaptive_includes,
  cpp_args: ['-DSTATIC_LIB_WITHOUT_DECRYPTOR'],
  dependencies: [
    dependency('pugixml'),
  ],
  link_with: link_libs,
)
