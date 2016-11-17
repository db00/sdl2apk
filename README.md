# sdl2apk

can run on Linux MacOSX iphoneos Android Raspbian

## Linux:
cd jni/src/ ; make

## MacOSX:
cd jni/src/ ; make

## iPhone/iPad:
AVFoundation.framework AudioToolbox.framework CoreAudio.framework CoreGraphics.framework Foundation.framework GameController.framework ImageIO.framework MobileCoreServices.framework OpenGLES.framework QuartzCore.framework UIKit.framework libSDL2.a libSDL2_image.a libSDL2_mixer.a libSDL2_ttf.a libiconv.dylib libstdc++.6.0.9.dylib openssl
open jni/src/___PROJECTNAME___.xcodeproj/


## Android:
adb pull /system/lib/libssl.so $ANDROID_NDK/platforms/android-9/arch-arm/usr/lib/
adb pull /system/lib/libcrypto.so $ANDROID_NDK/platforms/android-9/arch-arm/usr/lib/
cd jni/src/ ; make apk
