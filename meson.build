input_adaptive_includes = [
  './lib_adapter'
]

input_adaptive_export_includes = [
  include_directories('./lib_adapter')
]

link_libs = []
input_adaptive_sources = []

ap4_includes = []
subdir('lib/Bento4/Source/C++')
subdir('src')

subdir('lib/webm_parser')
link_libs += webm_parser_lib
input_adaptive_includes += ['lib/webm_parser/include']

link_libs += input_adaptive_init_lib

subdir('lib/mpegts')
link_libs += mpegts_lib
input_adaptive_includes += ['lib/mpegts']

input_adaptive_export_includes += ap4_includes

input_adaptive_lib = static_library(
  'input_adaptive',
  input_adaptive_sources,
  include_directories: input_adaptive_includes,
  cpp_args: ['-DSTATIC_LIB_WITHOUT_DECRYPTOR', '-DTARGET_POSIX'],
  dependencies: [
    dependency('pugixml'),
  ],
  link_with: link_libs,
)
