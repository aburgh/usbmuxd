# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to, avoiding a bug in XCode 1.5
all.Debug: \
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Debug/libusbmuxd.dylib\
	/Users/aburgh/Projects/iPhone/usbmuxd/daemon/Debug/usbmuxd\
	/Users/aburgh/Projects/iPhone/usbmuxd/tools/Debug/iproxy

all.Release: \
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Release/libusbmuxd.dylib\
	/Users/aburgh/Projects/iPhone/usbmuxd/daemon/Release/usbmuxd\
	/Users/aburgh/Projects/iPhone/usbmuxd/tools/Release/iproxy

all.MinSizeRel: \
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/MinSizeRel/libusbmuxd.dylib\
	/Users/aburgh/Projects/iPhone/usbmuxd/daemon/MinSizeRel/usbmuxd\
	/Users/aburgh/Projects/iPhone/usbmuxd/tools/MinSizeRel/iproxy

all.RelWithDebInfo: \
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/RelWithDebInfo/libusbmuxd.dylib\
	/Users/aburgh/Projects/iPhone/usbmuxd/daemon/RelWithDebInfo/usbmuxd\
	/Users/aburgh/Projects/iPhone/usbmuxd/tools/RelWithDebInfo/iproxy

# For each target create a dummy rule so the target does not have to exist
/usr/lib/libpthread.dylib:
/Users/aburgh/Library/Caches/Xcode/Release/libusb.dylib:
/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Debug/libusbmuxd.dylib:
/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/MinSizeRel/libusbmuxd.dylib:
/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/RelWithDebInfo/libusbmuxd.dylib:
/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Release/libusbmuxd.dylib:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Debug/libusbmuxd.dylib:\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Debug/libusbmuxd.dylib


/Users/aburgh/Projects/iPhone/usbmuxd/daemon/Debug/usbmuxd:\
	/Users/aburgh/Library/Caches/Xcode/Release/libusb.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/daemon/Debug/usbmuxd


/Users/aburgh/Projects/iPhone/usbmuxd/tools/Debug/iproxy:\
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Debug/libusbmuxd.dylib\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/tools/Debug/iproxy


/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Release/libusbmuxd.dylib:\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Release/libusbmuxd.dylib


/Users/aburgh/Projects/iPhone/usbmuxd/daemon/Release/usbmuxd:\
	/Users/aburgh/Library/Caches/Xcode/Release/libusb.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/daemon/Release/usbmuxd


/Users/aburgh/Projects/iPhone/usbmuxd/tools/Release/iproxy:\
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/Release/libusbmuxd.dylib\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/tools/Release/iproxy


/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/MinSizeRel/libusbmuxd.dylib:\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/MinSizeRel/libusbmuxd.dylib


/Users/aburgh/Projects/iPhone/usbmuxd/daemon/MinSizeRel/usbmuxd:\
	/Users/aburgh/Library/Caches/Xcode/Release/libusb.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/daemon/MinSizeRel/usbmuxd


/Users/aburgh/Projects/iPhone/usbmuxd/tools/MinSizeRel/iproxy:\
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/MinSizeRel/libusbmuxd.dylib\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/tools/MinSizeRel/iproxy


/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/RelWithDebInfo/libusbmuxd.dylib:\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/RelWithDebInfo/libusbmuxd.dylib


/Users/aburgh/Projects/iPhone/usbmuxd/daemon/RelWithDebInfo/usbmuxd:\
	/Users/aburgh/Library/Caches/Xcode/Release/libusb.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/daemon/RelWithDebInfo/usbmuxd


/Users/aburgh/Projects/iPhone/usbmuxd/tools/RelWithDebInfo/iproxy:\
	/Users/aburgh/Projects/iPhone/usbmuxd/libusbmuxd/RelWithDebInfo/libusbmuxd.dylib\
	/usr/lib/libpthread.dylib
	/bin/rm -f /Users/aburgh/Projects/iPhone/usbmuxd/tools/RelWithDebInfo/iproxy


