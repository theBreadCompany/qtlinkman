# qtlinkman

A sample application using Qt.

## Disclaimer

The MIT license applies, meaning that all of my code is delivered without any waranty. Also, for the Qt parts, the Qt Open Source licenses apply, so please mind those as you progress.

## project

My goal was to create a simple app that can upload images to a server and maintain links to those. This goal should be achieved using C++ and Qt for GUI stuff. 

The reason for this is that I yet need to learn, especially C++ and C(ish) in general. This also means that this code, style etc. is far from perfect. I will improve this asap.

## building and installing

Dependencies: cmake, qt6 (either libs or [whole SDK](https://www.qt.io/download-qt-installer-oss))

```bash
git clone https://github.com/thebreadcompany/qtlinkman
cd qtlinkman
mkdir build
cd build
cmake ..
make
```

The resulting executable, i.e. the .app for macOS, will be in the `build` directory.

## yet TODO

- CLI version
- learn and use in another version that is yet to come
- change history table to history tree so collections actually make sense
- add subdomains
- add link redirects
- implement multifile upload
- add icon
- organize stuff

Please forgive me for again not pushing any files until I'm somewhat done.
