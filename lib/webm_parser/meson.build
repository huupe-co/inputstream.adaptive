webm_parser_includes = [
  'include'
]

webm_parser_sources = []

subdir('src')

webm_parser_lib = static_library(
  'webm_parser',
  webm_parser_sources,
  include_directories: webm_parser_includes,
  # cpp_args: ['-DSTATIC_LIB_WITHOUT_DECRYPTOR'],
  dependencies: [
    # dependency('pugixml'),
  ]
)
