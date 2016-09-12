
#ifdef __ANDROID__

#include <android/bitmap.h>
#include "SDL.h"
#include "sprite.h"
#include <jni.h>

int startCam()
{
	// retrieve the JNI environment.
	JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

	// retrieve the Java instance of the SDLActivity
	jobject activity = (jobject)SDL_AndroidGetActivity();

	// find the Java class of the activity. It should be SDLActivity or a subclass of it.
	//jclass clazz(env->GetObjectClass(activity));
	jclass clazz = (*env)->GetObjectClass(env, activity);

	// find the identifier of the method to call
	jmethodID method_id = (*env)->GetMethodID(env,clazz, "startCam", "()V");
	//jmethodID method_id = (*env)->GetMethodID(env,clazz, "openAlbum", "()V");

	// effectively call the Java method
	(*env)->CallVoidMethod(env,activity, method_id);

	// clean up the local references.
	(*env)->DeleteLocalRef(env,activity);
	(*env)->DeleteLocalRef(env,clazz);
	return 1;
}

/* called by java onCamareOk */
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_onCamareOk(JNIEnv *env, jclass clazz, jobject zBitmap) {  
	SDL_Log("--------------------------------------------------\n");  
	JNIEnv J = *env;  

	if (zBitmap == NULL) {  
		SDL_Log("bitmap is null\n");  
		return;  
	}  

	// Get bitmap info  
	AndroidBitmapInfo info;  
	memset(&info, 0, sizeof(info));  
	AndroidBitmap_getInfo(env, zBitmap, &info);  
	// Check format, only RGB565 & RGBA are supported  
	if (info.width <= 0 || info.height <= 0   
			|| (info.format == ANDROID_BITMAP_FORMAT_NONE)
	   ) {  
		SDL_Log("invalid bitmap\n");  
		J->ThrowNew(env, J->FindClass(env, "java/io/IOException"), "invalid bitmap");  
		return;  
	}  

	// Lock the bitmap to get the buffer  
	void * pixels = NULL;  
	int res = AndroidBitmap_lockPixels(env, zBitmap, &pixels);  
	if (pixels == NULL) {  
		SDL_Log("fail to lock bitmap: %d\n", res);  
		J->ThrowNew(env, J->FindClass(env, "java/io/IOException"), "fail to open bitmap");  
		return;
	}  

	Uint32 rmask, gmask, bmask, amask=0;
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
	SDL_Log("Effect: %dx%d, %d\n", info.width, info.height, info.format);  
	int depth=0;
	switch(info.format)
	{
		case ANDROID_BITMAP_FORMAT_RGBA_8888:
			depth=32;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;
#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0xff000000;
#endif
			break;
		case ANDROID_BITMAP_FORMAT_RGB_565:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0x1f<<11;
			gmask = 0x3f<<5;
			bmask = 0x1f;
#else
			bmask = 0x1f<<11;
			gmask = 0x3f<<5;
			rmask = 0x1f;
#endif
			depth=16;
			break;
		case ANDROID_BITMAP_FORMAT_RGBA_4444:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xf000;
			gmask = 0x0f00;
			bmask = 0x00f0;
			amask = 0x000f;
#else
			rmask = 0x000f;
			gmask = 0x00f0;
			bmask = 0x0f00;
			amask = 0xf000;
#endif
			depth=16;
			break;
		case ANDROID_BITMAP_FORMAT_A_8:
			break;
		case ANDROID_BITMAP_FORMAT_NONE:
			break;
	}
	SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(pixels,
			info.width,
			info.height,
			depth,
			info.stride,
			rmask,
			gmask,
			bmask,
			amask
			);

	SDL_Surface * image = Surface_new(info.width,info.height);
	SDL_BlitSurface(surface, NULL, image, NULL);
	Sprite * sprite = Sprite_getChildByIndex(stage,0);
	Sprite_setSurface(sprite,image);
	image->refcount--;
	sprite->w = info.width;
	sprite->h = info.height;
	SDL_FreeSurface(surface);

	AndroidBitmap_unlockPixels(env, zBitmap);  
}  

#endif
