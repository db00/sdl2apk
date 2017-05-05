# sdl2apk

can run on Windows Linux MacOSX iPhoneOS Android Raspbian

## Linux:
` cd jni/src/ ; make `

## MacOSX:
` cd jni/src/ ; make `

## iPhone/iPad:
* AVFoundation.framework AudioToolbox.framework CoreAudio.framework CoreGraphics.framework Foundation.framework GameController.framework ImageIO.framework MobileCoreServices.framework OpenGLES.framework QuartzCore.framework UIKit.framework libSDL2.a libSDL2_image.a libSDL2_mixer.a libSDL2_ttf.a libiconv.dylib libstdc++.6.0.9.dylib openssl
* 
```
 open jni/src/___PROJECTNAME___.xcodeproj/ 
```

## Android:
```
adb pull /system/lib/libssl.so $ANDROID_NDK/platforms/android-9/arch-arm/usr/lib/ 
adb pull /system/lib/libcrypto.so $ANDROID_NDK/platforms/android-9/arch-arm/usr/lib/ 
```
`cd jni/src/ ; make apk `





# 目前仅提供Android，如需ios版，自己下载源码编译
# 字典: https://pan.baidu.com/s/1jH76fv4
# sound.zip: 为字典文件，解压，复制到Android手机 /sdcard/ 目录下即可

# contact: db0@qq.com

