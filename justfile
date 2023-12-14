default:
  just --list

build:
  cd src && qmake
  cd src && make
  mkdir -p ./bin/lnx/AppDir/usr/share/applications
  mkdir -p ./bin/lnx/AppDir/usr/share/icons/hicolor/32x32/apps
  mkdir -p ./bin/lnx/AppDir/usr/bin
  cp ./src/Diskbutler ./bin/lnx/AppDir/usr/bin/DiskButler
  cp ./src/foxsdk/libFoxSDKCore.so /usr/lib/libFoxSDKCore.so
  cp ./diskbutler.png ./bin/lnx/AppDir/usr/share/icons/hicolor/32x32/apps/DiskButler.png
  cp diskbutler.desktop ./bin/lnx/AppDir/usr/share/applications/diskbutler.desktop
  cd ./bin/lnx && APPIMAGE_EXTRACT_AND_RUN=1 /usr/local/bin/linuxdeploy-x86_64.AppImage -i ./AppDir/usr/share/icons/hicolor/32x32/apps/DiskButler.png -d ./AppDir/usr/share/applications/diskbutler.desktop --appdir AppDir --plugin qt --output appimage

clean:
  cd src && make clean || true
  rm -f src/Diskbutler
  rm -rf bin
