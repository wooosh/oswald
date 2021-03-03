project('oswald', 'cpp')

src = [
  'src/buffer/mark.cxx',
  'src/buffer/portion.cxx',
  
  'src/draw/draw.cxx',

  'src/terminal/terminal.cxx',
  'src/terminal/keypress.cxx',

  'src/modules/highlight_regex.cxx',
  'src/modules/highlight_none.cxx',

  'src/main.cxx'
]

inc = include_directories('src')
tree_sitter_inc = include_directories('tree_sitter/include')

executable('oswald', src, include_directories : [inc, tree_sitter_inc], objects: ['tree_sitter/libtree-sitter.a'])
