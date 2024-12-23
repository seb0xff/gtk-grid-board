project('gtk-grid-board', 'c', default_options: ['c_std=c11'])

gtk_dep = dependency('gtk4')
dependencies = [gtk_dep]

sources = [
  'ggb.c',
]

headers = [
  'ggb.h',
]

cargs = ['-fPIC']

ggb_lib = shared_library(
  'ggb',
  sources,
  c_args: cargs,
  dependencies: dependencies,
  install: true,
)


install_headers(headers, subdir: 'ggb')

# adwaita_dep = dependency('libadwaita-1')
test1 = executable(
  'ggb_test',
  'ggb_test.c',
  link_with: [ggb_lib],
  include_directories: ['.'],
  dependencies: [dependencies],
)

test('ggb-test', test1, timeout: 0)

gir_includes = ['Gtk-4.0', 'GObject-2.0', 'Gdk-4.0']

gnome = import('gnome')
gnome.generate_gir(
  ggb_lib,
  namespace: 'Ggb',
  identifier_prefix: 'Ggb',
  symbol_prefix: 'ggb',
  nsversion: '0.1',
  sources: [sources, headers],
  dependencies: dependencies,
  includes: gir_includes,
  install: true,
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(
  ggb_lib,
  name: 'ggb-0.1',
  description: 'Gtk Grid Board Library',
  version: '0.1',
)
