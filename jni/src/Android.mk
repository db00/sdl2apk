LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH :=../SDL2
SDL2image_PATH :=../SDL2_image
SDL2mixer_PATH :=../SDL2_mixer
SDL2ttf_PATH :=../SDL2_ttf
#libxml_PATH:= ../libxml
#libiconv_PATH:= ../libiconv
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/$(SDL2image_PATH) \
	$(LOCAL_PATH)/$(SDL2ttf_PATH) \
	$(LOCAL_PATH)/$(SDL2mixer_PATH) \
	$(ANDROID_NDK)/sources/ffmpeg\
	$(LOCAL_PATH)/include \
	#	$(LOCAL_PATH)/$(libxml_PATH)/include \
	$(LOCAL_PATH)/$(libiconv_PATH)/include \


# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	input.c utf8.c sprite.c matrix.c myregex.c kodi.c jsonrpc.c files.c sdlfiles.c httploader.c ipstring.c mystring.c cJSON.c  \
	urlcode.c filetypes.c httpserver.c array.c base64.c \
	dict.c \
	readbaidu.c update.c regex.c textfield.c pinyin.c read_card.c music.c \
	tween.c ease.c mysurface.c androidcam.c map.c\
	testime.c
#	video.c array.c tween.c ease.c base64.c ipstring.c sprite.c httploader.c mystring.c textfield.c files.c matrix.c 
#	player.c
#	glestest.c
#	testffplay.c
#gles2.c
#	glestest.c
#	testgles2.c SDL_test_common.c
#	testdraw2.c SDL_test_common.c
#androidcam.c
#	ffmpegplayger.c
#sqlite.c
# sprite.c matrix.c myregex.c kodi.c jsonrpc.c files.c httploader.c ipstring.c mystring.c cJSON.c  \
	urlcode.c filetypes.c httpserver.c base64.c \
	dict.c sqlite3.c sqlite.c
#urlcode.c filetypes.c httpserver.c base64.c httploader.c ipstring.c readbaidu.c update.c testime.c regex.c dict.c myregex.c files.c mystring.c sprite.c matrix.c textfield.c 
#	testgles2.c
#	ffmpegplayger.c
#dict.c sprite.c mystring.c textfield.c files.c matrix.c btn.c myregex.c regex.c # ic.c
#	playmus.c myttf.c
#	myttf.c tween.c
#	testrotozoom.c
#	testgfx.c
#	sdlgles3ds.c
#	showimage.c
#	Simple_Texture2D.c
#	earth.c
#	showtxt.c \
	testtimer.c
#	MultiTexture.c
#	gles2font.c
#	glsphere.c
#	sdlgles3ds.c
#	testime.c
#	testgesture.c
#	testkeys.c
#	testhittesting.c
#	main.c

#LOCAL_SHARED_LIBRARIES := SDL2 \

LOCAL_STATIC_LIBRARIES := libavdevice libavformat libavfilter libavcodec libwscale libavutil libswresample libswscale libpostproc

LOCAL_SHARED_LIBRARIES := \
	SDL2 \
	mikmod \
	smpeg2 \
	SDL2_image \
	SDL2_mixer \
	SDL2_ttf \
	#	SDL2_net \
	#	avformat avcodec avutil swscale swresample \
	#						  xml \
	iconv \
	cupfeatures \
	webp \


#LOCAL_JNI_SHARED_LIBRARIES := \
	avformat avcodec avutil swscale swresample \
	SDL2 \
	SDL2_image \
	SDL2_mixer \
	SDL2_net \
	SDL2_ttf \
	webp \
	mikmod \
	smpeg2 \
	cupfeatures \

#LOCAL_COPY_HEADERS_TO := libSDL2_ttf

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid -lm -lz -lssl -lcrypto -ljnigraphics\
	# -lsqlite
# -liconv  #-D debug_sprite
LOCAL_EXPORT_LDLIBS := -Wl,--undefined=Java_my_test_Test_nativeInit -lssl -lcrypto -ldl -lGLESv1_CM -lGLESv2 -llog -lz -landroid -lft2
LOCAL_CFLAGS += -D test_ime
LOCAL_CFLAGS += -D test_video
#LOCAL_CFLAGS += -D debug_sprite

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
$(call import-module,ffmpeg/android/arm)
