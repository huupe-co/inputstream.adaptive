input_adaptive_init_includes = [include_directories('.'), include_directories('../../lib_adapter')]
input_adaptive_init_sources = []

subdir('commons')
subdir('main')
subdir('filesystem')
subdir('utils')
subdir('threads')
subdir('platform')

input_adaptive_init_sources += files(
  'AddonInit.cpp',
)

input_adaptive_init_lib = static_library(
  'input_adaptive_init',
  input_adaptive_init_sources,
  include_directories: input_adaptive_init_includes,
  cpp_args: ['-DTARGET_POSIX'],
  dependencies: [
  ]
)