target:="android-14"

app_name:=new
package:=my.app
class:=New

old_app_name:=test
old_package:=my.test
old_class:=Test

package2:=$(subst .,_,$(package))#将"."替换成"_"
package3:=$(subst .,/,$(package))#将"."替换成"/"
old_package2:=$(subst .,_,$(old_package))#将"."替换成"_"
old_package3:=$(subst .,/,$(old_package))#将"."替换成"/"

path:=../$(app_name)


# AndroidManifest.xml
permission:= <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"\/> \
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"\/> \
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE"\/> \
<uses-permission android:name="android.permission.INTERNET"\/> \
<uses-sdk android:minSdkVersion="10" android:targetSdkVersion="12" \/>


#android create project -n $(app_name) -t 1 -p $(path) -k $(package) -a $(class)
#android --clear-cache update project --name $(app_name) --path $(path) --target $(target)  --subprojects
.PHONY : gen
gen:
#ifeq (0,1)
	rm -rf $(path)
	android create project -n $(app_name) --target $(target) -p $(path) -k $(package) -a $(class)
	cp Makefile $(path)/Makefile
	cp -r jni $(path)/jni
#endif
	sed -e 's/$(old_package)/$(package)/' -e 's/$(old_class)/$(class)/g' src/$(old_package3)/$(old_class).java > $(path)/src/$(package3)/$(class).java
	sed -e 's/$(old_package2)/$(package2)/' -e 's/$(old_class)/$(class)/g' jni/SDL2/src/main/android/SDL_android_main.c > $(path)/jni/SDL2/src/main/android/SDL_android_main.c
	sed -e 's/$(old_package2)/$(package2)/' -e 's/$(old_class)/$(class)/g' jni/SDL2/src/core/android/SDL_android.c > $(path)/jni/SDL2/src/core/android/SDL_android.c
	sed -e 's/$(old_package2)/$(package2)/' -e 's/$(old_class)/$(class)/' jni/src/Android.mk > $(path)/jni/src/Android.mk
	sed -e 's/$(old_class)/$(class)/g' -e 's/$(old_package)/$(package)/' -e 's/<application /$(permission)<application /' AndroidManifest.xml > $(path)/AndroidManifest.xml 
	sed -e 's/$(old_app_name)/$(app_name)/g' -e 's/$(old_class)/$(class)/g' -e 's/$(old_package)/$(package)/g' jni/src/Makefile > $(path)/jni/src/Makefile

apk:
	ndk-build NDK_DEBUG=1 -C $(path) && ant debug -f $(path)/build.xml && adb install -r $(path)/bin/$(app_name)-debug.apk && adb shell am start -a android.intenon.MAIN -n $(package)/.$(class)
