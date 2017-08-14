#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jni.h"

#ifdef _WIN32
#undef JNICALL
#define JNICALL __cdecl
#define USE_WINAPI
#else
#define USE_XCB
#endif

#define WINDOW_FLAG_TOPLEVEL (1 << 0)

#ifdef USE_XCB

#include <xcb/xcb.h>

xcb_connection_t *connection;
xcb_screen_t *screen;

#endif

#ifdef USE_WINAPI

#include <windows.h>

#endif

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_init(JNIEnv *env, jclass cls) {
#ifdef USE_XCB
	fprintf(stderr, "NJT: Init (XCB backend)\n");
	connection = xcb_connect(NULL, NULL);
	const xcb_setup_t *setup = xcb_get_setup(connection);
	struct xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	screen = iter.data;
#endif
#ifdef USE_WINAPI
	fprintf(stderr, "NJT: Init (WinAPI backend)\n");
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_deInit(JNIEnv *env, jclass cls) {
	fprintf(stderr, "NJT: DeInit\n");
#ifdef USE_XCB
	xcb_disconnect(connection);
#endif
}

JNIEXPORT jobject JNICALL Java_com_eternal_1search_njt_NativeFunctions_waitForEvent(JNIEnv *env, jclass cls) {
	jobject result = 0;
#ifdef USE_XCB
	xcb_flush(connection);
	xcb_generic_event_t *event = xcb_wait_for_event(connection);
	if (event) {
		jclass eventCls = (*env)->FindClass(env, "com/eternal_search/njt/NativeFunctions$Event");
		jmethodID constructor = (*env)->GetMethodID(env, eventCls, "<init>", "()V");
		jobject eventObj = (*env)->NewObject(env, eventCls, constructor);
		free(event);
		result = eventObj;
	}
#endif
#ifdef USE_WINAPI
	MSG msg;
	if (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		jclass eventCls = (*env)->FindClass(env, "com/eternal_search/njt/NativeFunctions$Event");
		jmethodID constructor = (*env)->GetMethodID(env, eventCls, "<init>", "()V");
		jobject eventObj = (*env)->NewObject(env, eventCls, constructor);
		result = eventObj;
	}
#endif
	return result;
}

JNIEXPORT jlong JNICALL Java_com_eternal_1search_njt_NativeFunctions_createWindow(JNIEnv *env, jclass cls,
		jstring title, jint x, jint y, jint width, jint height, jint flags, jlong parent) {
	jlong result = -1;
	const char *titleChars = (*env)->GetStringUTFChars(env, title, 0);
#ifdef USE_XCB
	if (flags & WINDOW_FLAG_TOPLEVEL) {
		parent = screen->root;
	}
	xcb_window_t window = xcb_generate_id(connection);
	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, (xcb_window_t) parent, x, y,
					  (uint16_t) width, (uint16_t) height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
					  screen->root_visual, 0, NULL);
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, (xcb_window_t) window,
						XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, (uint32_t) strlen(titleChars), titleChars);
	result = window;
#endif
#ifdef USE_WINAPI
	if (flags & WINDOW_FLAG_TOPLEVEL) {
		parent = (jlong) (size_t) HWND_DESKTOP;
	}
	HWND window = CreateWindowEx(0, "Static", titleChars, WS_OVERLAPPEDWINDOW, x, y, width, height,
			(HWND) (size_t) parent, NULL, GetModuleHandle(NULL), NULL);
	result = (jlong) (size_t) window;
#endif
	(*env)->ReleaseStringChars(env, title, (const jchar*) titleChars);
	return result;
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_destroyWindow(JNIEnv *env, jclass cls,
		jlong window) {
#ifdef USE_XCB
	xcb_destroy_window(connection, (xcb_window_t) window);
#endif
#ifdef USE_WINAPI
	DestroyWindow((HWND) (size_t) window);
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_showWindow(JNIEnv *env, jclass cls,
																				  jlong window) {
#ifdef USE_XCB
	xcb_map_window(connection, (xcb_window_t) window);
#endif
#ifdef USE_WINAPI
	ShowWindow((HWND) (size_t) window, SW_SHOW);
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_hideWindow(JNIEnv *env, jclass cls,
																				  jlong window) {
#ifdef USE_XCB
	xcb_unmap_window(connection, (xcb_window_t) window);
#endif
#ifdef USE_WINAPI
	ShowWindow((HWND) (size_t) window, SW_HIDE);
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_setWindowTitle(JNIEnv *env, jclass cls,
																				   jlong window,
																				   jstring title) {
	const char *titleChars = (*env)->GetStringUTFChars(env, title, 0);
#ifdef USE_XCB
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, (xcb_window_t) window,
						XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, (uint32_t) strlen(titleChars), titleChars);
#endif
#ifdef USE_WINAPI
	SetWindowText((HWND) (size_t) window, titleChars);
#endif
	(*env)->ReleaseStringChars(env, title, (const jchar*) titleChars);
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_moveWindow(JNIEnv *env, jclass cls,
																				  jlong window, jint x,
																			   jint y, jint width,
																			   jint height) {
#ifdef USE_XCB
	const uint32_t values[] = { (uint32_t) x, (uint32_t) y, (uint32_t) width, (uint32_t) height };
	xcb_configure_window(connection, (xcb_window_t) window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
#endif
#ifdef USE_WINAPI
	MoveWindow((HWND) (size_t) window, x, y, width, height, TRUE);
#endif
}
