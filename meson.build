project('evince-typst', 'c')

if get_option('buildtype') == 'release'
  cargo_target = 'release'
else
  cargo_target = 'debug'
endif  

libevince_typst = declare_dependency(
  link_args: [meson.source_root() / 'target' / cargo_target / 'libevince_typst.a'], 
)

subdir('evince-backend')
