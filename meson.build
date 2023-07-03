project('cmaze', 'c', 'cpp', version: '0.1.0')

cc = meson.get_compiler('c')
math = cc.find_library('m')

sdl = dependency('sdl2', required: false)

if sdl.found()
  add_project_arguments('-DHAVE_SDL', language: 'c')
endif

sources = files('vector.c', 'vector.h', 'map.c', 'map.h', 'gen.c', 'gen.h')
subdir('backends')

exe = executable('cmaze', 'main.c', sources: sources, include_directories: ['backends', '.'], dependencies: [math, sdl])

run_target('run', command: exe)

