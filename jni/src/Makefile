PROJ_PATH=../..
ifneq ($(OS),Windows_NT)
#Linux Darwin Windows_NT
OS:=$(shell uname)
endif
#JAVA_HOME:=/usr/lib/jvm/java-8-openjdk-i386/jre
NDK_HOME:= $(HOME)/android-ndk-r10
SDK_HOME:= $(HOME)/android-sdk-linux/tools
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
#xcodebuild -project Xcode/SDL/SDL.xcodeproj/ -target Framework
#~/SDL2-x.x.x/Xcode/SDL/build/Debug/SDL2.framework
else
INCLUDE_ROOT=..
INCLUDE:=-I"$(INCLUDE_ROOT)/SDL2/include/" -I"$(INCLUDE_ROOT)/SDL2_ttf/" -I"$(INCLUDE_ROOT)/SDL2_image/" -I"$(INCLUDE_ROOT)/SDL2_mixer/" 
endif
# -I"$(ANDROID_NDK_HOME)/sources/ffmpeg/android/arm/include/"
DEFINES:= -DSTDC_HEADERS
# -D debug_sprite
KEY:=$(PROJ_PATH)/my.keystore
ifeq ($(OS),Windows_NT)
	TARGET:= a.exe
	LIB:=-L"." -L"lib"  -lssl -lcrypto -lwsock32 -lgdi32 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -liconv -lpthread -D test_ime -lavformat -lavcodec -lavutil -lswscale -lopengl32 -lmingw32 -lSDL2main -lSDL2
	SRC:=\
		sprite.c matrix.c myregex.c kodi.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c \
		dict.c \
		readbaidu.c update.c testime.c textfield.c \
		tween.c ease.c \
		urlcode.c filetypes.c httpserver.c array.c base64.c mysurface.c regex.c lib/libssl.a lib/libcrypto.a lib/libeay32.dll.a lib/libssl32.dll.a# sprite.c sqlite3.c 
else
	TARGET:= ./a.out
	LIB:=-L"/usr/local/lib/" -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -lSDL2 -ldl -lssl -lcrypto -lpthread -D test_ime # -lavformat -lavcodec -lavutil -lswscale 
#-framework OpenGL -framework GLUT -framework SDL2 -framework SDL2_mixer -framework SDL2_ttf -framework SDL2_image -framework Foundation
	SRC:=\
		sprite.c matrix.c input.c myregex.c kodi.c utf8.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c \
		dict.c \
		readbaidu.c update.c testime.c textfield.c read_card.c pinyin.c music.c searhdict.c\
		tween.c ease.c \
		urlcode.c filetypes.c httpserver.c array.c base64.c mysurface.c
endif
#SRC:= $(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.c)))
OBJS:= $(patsubst %.c,%.o,$(SRC))

CFLAGS =  -g $(INCLUDE) $(DEFINES) $(LIB)

all:$(TARGET)
ifeq ($(OS),Windows_NT)
#	@echo $(shell set)
else	
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
