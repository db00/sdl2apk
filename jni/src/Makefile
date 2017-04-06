PROJ_PATH=../..
ifneq ($(OS),Windows_NT)
OS:=$(shell uname)
endif
#JAVA_HOME:=/usr/lib/jvm/java-8-openjdk-i386/jre
ifeq ($(OS),Darwin)
NDK_HOME:= $(HOME)/android-ndk-r9c
SDK_HOME:= $(HOME)/android-sdk-macosx/tools
else
NDK_HOME:= $(HOME)/android-ndk-r10
SDK_HOME:= $(HOME)/android-sdk-linux/tools
endif
APK_DIR:= $(PROJ_PATH)/bin
APP_NAME=test
PACKAGE:=my.test
PACKAGE2:=$(subst .,_,$(PACKAGE))#将"."替换成"_"
PACKAGE3:=$(subst .,/,$(PACKAGE))#将"."替换成"_"
CLASS_NAME:=Test
APK:= $(APK_DIR)/$(APP_NAME)-debug.apk
CC:=gcc
SRC_DIR:=.
LIB:= -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -lSDL2 -lssl -lcrypto -lpthread
SRC:= sprite.c matrix.c input.c myregex.c kodi.c utf8.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c \
		dict.c readbaidu.c update.c testime.c textfield.c read_card.c pinyin.c music.c searhdict.c\
		tween.c ease.c urlcode.c filetypes.c httpserver.c array.c base64.c mysurface.c datas.c sqlite.c besier.c sdlstring.c# sqlite3.c
DEFINES:= -D STDC_HEADERS -D test_ime
TARGET:= ./a.out
ifeq ($(OS),Darwin)
INCLUDE:=-I"/usr/local/include/SDL2/" -I"include"
LIB:=$(LIB) -L"/usr/local/lib/" -ldl -liconv -lsqlite3
else ifeq ($(OS),Linux)
INCLUDE:=-I"/usr/include/SDL2/" -I"include" -ldl -lsqlite3
else ifeq ($(OS),Windows_NT)
DEFINES:= $(DEFINES) -D main=SDL_main
TARGET:= a.exe
LIB:=-lmingw32 -lSDL2main $(LIB)  -mwindows  -Wl,--no-undefined -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -liconv -lws2_32 -lGLESv2 -lopengl32 -lwsock32 # lib/libssl.a lib/libcrypto.a lib/libeay32.dll.a lib/libssl32.dll.a#  -lavformat -lavcodec -lavutil -lswscale -lwsock32
INCLUDE:=-I"../SDL2/include/" -I"." -I"../SDL2_image/" -I"../SDL2_mixer/" -I"../SDL2_ttf/" -I"include"
SRC:= $(SRC) regex.c sqlite3.c
endif
# -D debug_sprite
KEY:=$(PROJ_PATH)/my.keystore
#SRC:= $(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.c)))
OBJS:= $(patsubst %.c,%.o,$(SRC))

CFLAGS :=  -g $(INCLUDE) $(DEFINES) $(LIB)
#SDL_PATH:=$(HOME)/SDL2-2.0.5/

all:$(TARGET)
	echo $(OS)
ifeq ($(OS),Windows_NT)
#	@echo $(shell set)
else	
#	@echo $(shell export)
#	@echo $(shell set)
endif
#	gdb $(TARGET)
	$(TARGET)
	@echo "all ok"

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

update:
ifdef SDL_PATH
	rm -rf $(PROJ_PATH)/jni/SDL2/* ; cp -r $(SDL_PATH)/src $(PROJ_PATH)/jni/SDL2/src ; cp -r $(SDL_PATH)/include $(PROJ_PATH)/jni/SDL2/include ; cp -r $(SDL_PATH)/Android.mk $(PROJ_PATH)/jni/SDL2/Androi.mk ;
	sed -e 's/org.libsdl.app/$(PACKAGE)/' -e 's/SDLActivity/$(CLASS_NAME)/g' $(SDL_PATH)/android-project/src/org/libsdl/app/SDLActivity.java > tmp ; cat tmp > $(PROJ_PATH)/src/$(PACKAGE3)/$(CLASS_NAME).java
	sed -e 's/org_libsdl_app/$(PACKAGE2)/' -e 's/SDLActivity/$(CLASS_NAME)/g' $(SDL_PATH)/src/main/android/SDL_android_main.c > $(PROJ_PATH)/jni/SDL2/src/main/android/SDL_android_main.c
	sed -e 's/org_libsdl_app/$(PACKAGE2)/' -e 's/SDLActivity/$(CLASS_NAME)/g' $(SDL_PATH)/src/core/android/SDL_android.c > $(PROJ_PATH)/jni/SDL2/src/core/android/SDL_android.c ; rm tmp
	sed -e 's/org_libsdl_app/$(PACKAGE2)/' -e 's/SDLActivity/$(CLASS_NAME)/' Android.mk > tmp ; cat tmp >  Android.mk
	sed -e 's/SDLActivity/$(CLASS_NAME)/g' -e 's/org.libsdl.app/$(PACKAGE)/' -e 's/<application /$(permission)<application /' $(PROJ_PATH)/AndroidManifest.xml > tmp ; cat tmp > $(PROJ_PATH)/AndroidManifest.xml 
endif
	rm -rf $(PROJ_PATH)/bin
	$(SDK_HOME)/android --clear-cache update project --name $(APP_NAME) --path $(PROJ_PATH) --target "android-14"  --subprojects

upload: $(APK)
	scp $(APK) p@p:/sdcard/

install:
	adb shell pm uninstall $(PACKAGE)
	adb install -r $(APK) && adb shell am start -a android.intenon.MAIN -n $(PACKAGE)/.$(CLASS_NAME)
#	adb shell logcat SDL:V *:I | grep sdl

apk:
#	$(SDK_HOME)/tools/android create project -n $(APP_NAME) -t 14 -p $(PROJ_PATH) -k $(PACKAGE) -a $(APP_NAME)
#	$(SDK_HOME)/tools/dx -dex --dump-to=out.dex --core-library $(PROJ_PATH)/bin
#	$(SDK_HOME)/tools/aapt -A assets -S res -M AndroidManifest.xml -I $(SDK_HOME)/platforms/android-10/android.jar -F $(PROJ_PATH)/bin/out.ap_
	$(NDK_HOME)/ndk-build NDK_DEBUG=1 -C $(PROJ_PATH) && ant debug -f $(PROJ_PATH)/build.xml
#	scp $(APK) p@p:/sdcard/
#	adb install -r -s $(APK) && adb shell am start -a android.intenon.MAIN -n $(PACKAGE)/.$(CLASS_NAME)
	adb install -r $(APK) && adb shell am start -a android.intenon.MAIN -n $(PACKAGE)/.$(CLASS_NAME)
	adb push $(APK) /sdcard/
#	adb shell logcat SDL:V *:I

push:
	adb push $(APK) /sdcard/
#scp $(APK) p@p:/sdcard/test.apk

debug:
	adb shell am start -a android.intenon.MAIN -n $(PACKAGE)/.$(CLASS_NAME) && \
		adb shell logcat SDL:V *:I

key:
	keytool -v -genkey -alias my -keyalg RSA -validity 40000 -keystore $(KEY) -storepass 123456  -dname "CN=db0@qq.com, OU=biao, O=biao, L=hengyang, ST=hunane, C=cn"
#	rm -f $(PROJ_PATH)/my.keystore

sign: $(KEY) $(PROJ_PATH)/bin/$(APP_NAME)-release-unsigned.apk
	jarsigner -verbose -keystore $(KEY) -storepass 123456 -keypass 123456 -signedjar $(PROJ_PATH)/bin/$(APP_NAME).apk $(PROJ_PATH)/bin/$(APP_NAME)-release-unsigned.apk my

.c.o:
	$(CC) $(CFLAGS) -c $<
#上下两句相等
#%.o:%.c
#	$(CC) -c -o $@ $< $(CFLAGS)

ipa:
ifeq ($(OS),Darwin)
	xcodebuild  CODE_SIGN_IDENTITY="iPhone Distribution:hunan huaien network technology co., ltd.(6ZVSARMRUF)" #-target ___PROJECTNAME___ -configuration Release #
	xcrun -sdk iphoneos PackageApplication -v build/Release-iphoneos/___PROJECTNAME___.app -o ~/Desktop/$(APP_NAME).ipa --sign "iPhone Distribution:hunan huaien network technology co., ltd.(6ZVSARMRUF)"
endif

.PHONY : clean
clean:
ifeq ($(OS),Darwin)
	xcodebuild clean
endif
	$(RM) $(TARGET) *.o
	$(NDK_HOME)/ndk-build clean -C $(PROJ_PATH) && ant clean -f $(PROJ_PATH)/build.xml && rm -rf $(PROJ_PATH)/libs $(PROJ_PATH)/obj
