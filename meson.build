project('oswald', 'c')

src = [
  'src/adt/vec.c',

  'src/buffer/list.c',
  'src/buffer/open.c',
  'src/buffer/mark.c',

  'src/term/escapes.c',
  'src/term/mode.c',
  'src/term/input.c',

  'src/draw/draw.c',

  'src/plugins/debug.c',

  'src/event.c',
  'src/x.c',
  'src/main.c'
]
inc = include_directories('src')

# tree_sitter_inc = include_directories('tree_sitter/include')

executable(
  'oswald',
  src,
  include_directories : [inc],
  c_args : ['-static'],
  # objects: ['tree_sitter/libtree-sitter.a']
)
