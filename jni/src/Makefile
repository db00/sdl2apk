PROJ_PATH=../..
ifneq ($(OS),Windows_NT)
#Linux Darwin Windows_NT
OS:=$(shell uname)
endif
#JAVA_HOME:=/usr/lib/jvm/java-8-openjdk-i386/jre
ifeq ($(OS),Darwin)
NDK_HOME:= $(HOME)/android-ndk-r9c
SDK_HOME:= $(HOME)/android-sdk_r24-macosx/tools
else
NDK_HOME:= $(HOME)/android-ndk-r10
SDK_HOME:= $(HOME)/android-sdk-linux/tools
endif
APK_DIR:= $(PROJ_PATH)/bin
APP_NAME=test
PACKAGE:=my.test
CLASS_NAME:=Test
APK:= $(APK_DIR)/$(APP_NAME)-debug.apk
CC:=gcc
SRC_DIR:=.
ifeq ($(OS),Darwin)
INCLUDE_ROOT=/usr/local/include
INCLUDE:=-I"$(INCLUDE_ROOT)/SDL2/" -I"$(INCLUDE_ROOT)/SDL2_ttf/" -I"$(INCLUDE_ROOT)/SDL2_image/" -I"$(INCLUDE_ROOT)/SDL2_mixer/" 
else
INCLUDE_ROOT=..
INCLUDE:=-I"$(INCLUDE_ROOT)/SDL2/include/" -I"$(INCLUDE_ROOT)/SDL2_ttf/" -I"$(INCLUDE_ROOT)/SDL2_image/" -I"$(INCLUDE_ROOT)/SDL2_mixer/" 
endif
# -I"$(ANDROID_NDK_HOME)/sources/ffmpeg/android/arm/include/"
DEFINES:= -DSTDC_HEADERS
# -D debug_sprite
KEY:=$(PROJ_PATH)/my.keystore
SRC:=\
		sprite.c matrix.c input.c myregex.c kodi.c utf8.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c \
		dict.c readbaidu.c update.c testime.c textfield.c read_card.c pinyin.c music.c searhdict.c\
		tween.c ease.c urlcode.c filetypes.c httpserver.c array.c base64.c mysurface.c

LIB:=-L"/usr/local/lib/" -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -lSDL2 -ldl -lssl -lcrypto -lpthread -D test_ime # -lavformat -lavcodec -lavutil -lswscale 
ifeq ($(OS),Windows_NT)
	TARGET:= a.exe
	LIB:=$(LIB) -L"." -L"lib"  -lwsock32 -lgdi32 -liconv  -lopengl32 -lmingw32 -lSDL2main # lib/libssl.a lib/libcrypto.a lib/libeay32.dll.a lib/libssl32.dll.a#  -lavformat -lavcodec -lavutil -lswscale
	SRC:= $(SRC) regex.c
else
	TARGET:= ./a.out
ifeq ($(OS),Darwin)
#-framework OpenGL -framework GLUT -framework SDL2 -framework SDL2_mixer -framework SDL2_ttf -framework SDL2_image -framework Foundation
	LIB:=$(LIB) -liconv
endif
endif
#SRC:= $(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.c)))
OBJS:= $(patsubst %.c,%.o,$(SRC))

CFLAGS =  -g $(INCLUDE) $(DEFINES) $(LIB)

all:$(TARGET)
ifeq ($(OS),Windows_NT)
#	@echo $(shell set)
else	
ifeq ($(OS),Darwin)
	xcode-select -switch /Applications/Xcode.app/Contents/Developer
	#进入项目文件（xxx.xcodeproj）所在的文件夹
#编译
	xcodebuild clean -workspace  xxx.xcworkspace -scheme $targetName -configuration Release
#编译静态库文件
	xcodebuild -workspace xxx.xcworkspace -scheme $libraryproject -configuration Release
#编译静态库资源文件
	xcodebuild -workspace xxx.xcworkspace -scheme $resourceproject -configuration Release
#编译主题target文件
	xcodebuild -workspace xxx.xcworkspace -scheme $targetName -configuration Release

   #编译普通项目：
    xcodebuild -configuration "Release" -arch "armv7 armv7s" -target "Diaoser" -sdk iphoneos CODE_SIGN_IDENTITY="iPhone Distribution: Diao Ser" GCC_VERSION="com.apple.compilers.llvm.clang.1_0" -project Diaoser.xcodeproj
    #编译包含子项目的项目：
    xcodebuild -workspace Diaoser.xcodeproj/project.xcworkspace -scheme Diaoser  -configuration "Release" -arch "armv7 armv7s" -sdk iphoneos CODE_SIGN_IDENTITY="iPhone Distribution: Diao Ser" CONFIGURATION_BUILD_DIR='OUTPUT_DIRECTORY'
	

	xcodebuild clean
	xcodebuild -target $(APP_NAME) -configuration Release # CODE_SIGN_IDENTITY="iPhone Distribution:XXXXXX"
	xcrun -sdk iphoneos PackageApplication -v ~/$(APP_NAME)/build/Release-iphoneos/$(APP_NAME).app -o ~/Desktop/$(APP_NAME).ipa # --sign "iPhone Distribution:XXXXXX"
endif
#	@echo $(shell export)
#	@echo $(shell set)

endif
	echo $(OS)
	gdb $(TARGET)
	@echo "all ok"

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

#$(TARGET):$(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.h))) $(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.c))) Makefile
#	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)
#	$(TARGET) || gdb $(TARGET)

update:
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

.PHONY : clean
clean:
	$(RM) $(TARGET) *.o
	$(NDK_HOME)/ndk-build clean -C $(PROJ_PATH) && ant clean -f $(PROJ_PATH)/build.xml && rm -rf $(PROJ_PATH)/libs $(PROJ_PATH)/obj
