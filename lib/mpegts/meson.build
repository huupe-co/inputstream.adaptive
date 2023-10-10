mpegts_includes = [
]

mpegts_sources = []

mpegts_sources += files(
  'mpegts/debug.cpp',
  'mpegts/debug.h',
  'mpegts/elementaryStream.cpp',
  'mpegts/elementaryStream.h',
  'mpegts/ES_AAC.cpp',
  'mpegts/ES_AAC.h',
  'mpegts/ES_AC3.cpp',
  'mpegts/ES_AC3.h',
  'mpegts/ES_h264.cpp',
  'mpegts/ES_h264.h',
  'mpegts/ES_hevc.cpp',
  'mpegts/ES_hevc.h',
  'mpegts/ES_MPEGAudio.cpp',
  'mpegts/ES_MPEGAudio.h',
  'mpegts/ES_MPEGVideo.cpp',
  'mpegts/ES_MPEGVideo.h',
  'mpegts/ES_Subtitle.cpp',
  'mpegts/ES_Subtitle.h',
  'mpegts/ES_Teletext.cpp',
  'mpegts/ES_Teletext.h',
  'mpegts/mutex.h',
  'mpegts/tsDemuxer.cpp',
  'mpegts/tsDemuxer.h',
  'mpegts/tsPacket.h',
  'mpegts/tsTable.h',
  'mpegts/bitstream.cpp',
  'mpegts/bitstream.h'
)

mpegts_lib = static_library(
  'mpegts',
  mpegts_sources,
  include_directories: mpegts_includes,
  # cpp_args: ['-DSTATIC_LIB_WITHOUT_DECRYPTOR'],
  dependencies: [
    # dependency('pugixml'),
  ]
)
