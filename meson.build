project('oswald', 'c')

src = [
  'src/adt/vec.c',
  'src/adt/map.c',

  'src/buffer/open.c',
  'src/buffer/mark.c',

  'src/draw/draw.c',

  'src/plugins/debug.c',
  'src/plugins/navigation.c',

  'src/highlight.c',
  'src/draw/highlight_cursor.c',

  'src/command.c',
  'src/config.c',
  'src/event.c',
  'src/x.c',
  'src/main.c'
]
# TODO: fix includes to use "" so we dont need this
inc = include_directories('src')

# tree_sitter_inc = include_directories('tree_sitter/include')

libmeraki = subproject('libmeraki').get_variable('libmeraki_dep')
libimp = subproject('libimp').get_variable('libimp_dep')

executable(
  'oswald',
  src,
  include_directories : [inc],
  c_args : ['-static'],
  dependencies : [libmeraki, libimp],
  # objects: ['tree_sitter/libtree-sitter.a']
)
