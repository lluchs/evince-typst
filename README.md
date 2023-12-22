# evince-typst

(Experimental) addon for the [Evince][evince] document viewer to load [Typst][typst] files directly, without creating an intermediate PDF file.

[evince]: https://wiki.gnome.org/Apps/Evince
[typst]: https://typst.app

## Building

Configure:
```sh
meson setup build --buildtype=release
```

Build:
```sh
cargo build --release
ninja -C build
```

Install:
```sh
# Path will vary depending on your distribution
sudo cp -t /usr/lib/evince/4/backends build/evince-backend/{typstdocument.evince-backend,libtypstdocument.so}
sudo mkdir -p /usr/local/share/mime/packages
sudo cp -t /usr/local/share/mime/packages evince-typst.xml
sudo update-mime-database /usr/local/share/mime/
```

## License

Code in `src/` is based on typst-cli and is licensed under Apache-2.0, see LICENSE.

Code in `evince-backend/` is based on evince and is licensed under GPL version 2 or later, see the comments in the respective files.
