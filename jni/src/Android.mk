LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH :=../SDL2
SDL2image_PATH :=../SDL2_image
SDL2mixer_PATH :=../SDL2_mixer
SDL2ttf_PATH :=../SDL2_ttf
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/$(SDL2image_PATH) \
	$(LOCAL_PATH)/$(SDL2ttf_PATH) \
	$(LOCAL_PATH)/$(SDL2mixer_PATH) \
	$(LOCAL_PATH)/include \
#.$(ANDROID_NDK)/sources/ffmpeg\


# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	input.c utf8.c sprite.c matrix.c myregex.c kodi.c jsonrpc.c files.c sdlfiles.c httploader.c ipstring.c mystring.c cJSON.c  \
	urlcode.c filetypes.c httpserver.c array.c base64.c \
	dict.c datas.c sqlite.c \
	readbaidu.c update.c regex.c textfield.c myttf.c pinyin.c read_card.c music.c searhdict.c\
	tween.c ease.c mysurface.c androidcam.c map.c\
	testime.c besier.c sdlstring.c\
	zip.c myfont.c bytearray.c loading.c testwords.c pictures.c explain.c  btnlist.c sidebtns.c wordinput.c mylist.c alert.c\
	freetype.c
#sqlite3.c



#LOCAL_STATIC_LIBRARIES := libavdevice libavformat libavfilter libavcodec libwscale libavutil libswresample libswscale libpostproc

LOCAL_SHARED_LIBRARIES := \
	SDL2 \
	mikmod \
	smpeg2 \
	SDL2_image \
	SDL2_mixer \
	SDL2_ttf \

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid -lsqlite -lm -lz -lssl -lcrypto -ljnigraphics
LOCAL_EXPORT_LDLIBS := -Wl,--undefined=Java_my_test_Test_nativeInit -ldl -lGLESv1_CM -lGLESv2 -llog -lz -landroid -lft2
LOCAL_CFLAGS += -D test_ime
#LOCAL_CFLAGS += -D debug_freetyp

include $(BUILD_SHARED_LIBRARY)

#$(call import-module,ffmpeg/android/arm)
