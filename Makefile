app_name:=test
path:=../$(app_name)
class:=Test
target:="android-14"
package:=my.test
package2:=my_test
package3:=my/test

# AndroidManifest.xml
permission:= <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"\/> \
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"\/> \
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE"\/> \
<uses-permission android:name="android.permission.INTERNET"\/> \
<uses-sdk android:minSdkVersion="10" android:targetSdkVersion="12" \/>


# %s/org_libsdl_app/$(package2)/g
# %s/SDLActivity/$(class)/g
# src/org/libsdl/app/SDLActivity.java
# jni/SDL2/src/main/android/SDL_android_main.c
# jni/SDL2/src/core/android/SDL_android.c
# jni/src/Android.mk

#android create project -n $(app_name) -t 1 -p $(path) -k $(package) -a $(class)
#android --clear-cache update project --name $(app_name) --path $(path) --target $(target)  --subprojects
.PHONY : gen
gen:
	rm -rf $(path)
	android create project -n $(app_name) --target $(target) -p $(path) -k $(package) -a $(class)
	cp Makefile $(path)/Makefile
	cp -r jni $(path)/jni
	sed -e 's/org.libsdl.app/$(package)/' -e 's/SDLActivity/$(class)/g' src/org/libsdl/app/SDLActivity.java > $(path)/src/$(package3)/$(class).java
	sed -e 's/org_libsdl_app/$(package2)/' -e 's/SDLActivity/$(class)/g' jni/SDL2/src/main/android/SDL_android_main.c > $(path)/jni/SDL2/src/main/android/SDL_android_main.c
	sed -e 's/org_libsdl_app/$(package2)/' -e 's/SDLActivity/$(class)/g' jni/SDL2/src/core/android/SDL_android.c > $(path)/jni/SDL2/src/core/android/SDL_android.c
	sed -e 's/org_libsdl_app/$(package2)/' -e 's/SDLActivity/$(class)/' jni/src/Android.mk > $(path)/jni/src/Android.mk
	cp $(path)/AndroidManifest.xml $(path)/AndroidManifest2.xml && sed -e 's/SDLActivity/$(class)/g' -e 's/org.libsdl.app/$(package)/' -e 's/<application /$(permission)<application /' $(path)/AndroidManifest2.xml > $(path)/AndroidManifest.xml && rm $(path)/AndroidManifest2.xml

apk:
	ndk-build NDK_DEBUG=1 -C $(path) && ant debug -f $(path)/build.xml && adb install -r $(path)/bin/$(app_name)-debug.apk && adb shell am start -a android.intenon.MAIN -n $(package)/.$(class)
