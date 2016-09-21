PROJ_PATH=../..
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
INCLUDE_ROOT=..
INCLUDE:=-I"$(INCLUDE_ROOT)/SDL2/include/" -I"$(INCLUDE_ROOT)/SDL2_ttf/" -I"$(INCLUDE_ROOT)/SDL2_image/" -I"$(INCLUDE_ROOT)/SDL2_mixer/" -I"include"
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
#	video.c array.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c textfield.c files.c matrix.c \
#	ffplay.c cmdutils.c
#	ffplay.c cmdutils.c
#	-D test_zip zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c
#	pdf.c bytearray.c adler32.c inftrees.c inffast.c crc32.c zutil.c inflate.c
#	xls.c bytearray.c ole.c date.c mystring.c
#	doc.c bytearray.c ole.c
#	dict.c sqlite.c \
	readbaidu.c update.c testime.c regex.c textfield.c \
	tween.c ease.c 
else
	TARGET:= ./a.out
	LIB:=-lSDL2_image -lSDL2_ttf -lSDL2_mixer -LGLESv2 -lm -lSDL2 -ldl -lssl -lcrypto -lpthread -lsqlite3 -D test_ime -D debug_pdf -D test_video -lavformat -lavcodec -lavutil -lswscale 
	SRC:=\
		sprite.c matrix.c input.c myregex.c kodi.c utf8.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c \
		dict.c \
		readbaidu.c update.c testime.c textfield.c read_card.c pinyin.c music.c\
		tween.c ease.c \
		urlcode.c filetypes.c httpserver.c array.c base64.c mysurface.c
endif
#SRC:= $(foreach x,${SRC_DIR}, $(wildcard $(addprefix ${x}/*,.c)))
OBJS:= $(patsubst %.c,%.o,$(SRC))

CFLAGS =  -g $(INCLUDE) $(DEFINES) $(LIB)

all:$(TARGET)
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
#	export JAVA_HOME=$(JAVA_HOME)
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
