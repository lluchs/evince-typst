evince_document = dependency('evince-document-3.0')
ev_localedir = '/usr/share/locale'

shared_module(
  'typstdocument',
  sources: files('typst-document.c'),
  include_directories: [],
  dependencies: [evince_document, libevince_typst],
  c_args: [
    '-DGNOMELOCALEDIR="@0@"'.format(ev_localedir),
    '-DGETTEXT_PACKAGE="evince"',
  ],
)
