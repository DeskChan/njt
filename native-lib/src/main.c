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

JNIEnv *javaEnv;
jclass javaWindowClass;
jmethodID javaWindowHandleCloseMethod;
jmethodID javaWindowHandleMouseMoveMethod;

#ifdef USE_XCB

#include <xcb/xcb.h>

xcb_connection_t *connection;
xcb_screen_t *screen;

void handleEvent(xcb_generic_event_t *event) {
	switch (event->response_type & ~0x80) {
		case XCB_DESTROY_NOTIFY:
			(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleCloseMethod,
											 (jlong) ((xcb_destroy_notify_event_t*) event)->window);
			break;
		case XCB_MOTION_NOTIFY: {
			xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t*) event;
			(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleMouseMoveMethod,
											 (jlong) e->event, (jint) e->event_x, (jint) e->event_y,
											 (jint) e->root_x, (jint) e->root_y);
			break;
		}
		default:;
	}
}

#endif

#ifdef USE_WINAPI

#include <windows.h>

#define WND_CLASS_NAME "NJT_WND"

HINSTANCE hInstance;
WNDCLASSEX wndClass;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	jlong window_id = (jlong) (size_t) hWnd;
	switch (uMsg) {
		case WM_CLOSE:
			(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleCloseMethod,
				window_id);
			break;
		case WM_MOUSEMOVE: {
			POINT pos;
			GetCursorPos(&pos);
			jint x = lParam & 0xFFFF;
			jint y = lParam >> 16;
			jint rootX = pos.x;
			jint rootY = pos.y;
			(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleMouseMoveMethod,
											 window_id, x, y, rootX, rootY);
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_init(JNIEnv *env, jclass cls) {
	javaEnv = env;
	javaWindowClass = (*env)->FindClass(env, "com/eternal_search/njt/Window");
	javaWindowHandleCloseMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleClose", "(J)V");
	javaWindowHandleMouseMoveMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleMouseMove",
																"(JIIII)V");
#ifdef USE_XCB
	fprintf(stderr, "NJT: Init (XCB backend)\n");
	connection = xcb_connect(NULL, NULL);
	const xcb_setup_t *setup = xcb_get_setup(connection);
	struct xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	screen = iter.data;
#endif
#ifdef USE_WINAPI
	fprintf(stderr, "NJT: Init (WinAPI backend)\n");
	hInstance = GetModuleHandle(NULL);
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_DBLCLKS | CS_GLOBALCLASS | CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WND_CLASS_NAME;
	wndClass.hIconSm = NULL;
	RegisterClassEx(&wndClass);
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_deInit(JNIEnv *env, jclass cls) {
	fprintf(stderr, "NJT: DeInit\n");
#ifdef USE_XCB
	xcb_disconnect(connection);
#endif
#ifdef USE_WINAPI
	UnregisterClass(WND_CLASS_NAME, hInstance);
#endif
	javaEnv = NULL;
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_runEventLoop(JNIEnv *env, jclass cls) {
#ifdef USE_XCB
	xcb_flush(connection);
	xcb_generic_event_t *event;
	while ((event = xcb_wait_for_event(connection)) != NULL) {
		handleEvent(event);
		free(event);
	}
#endif
#ifdef USE_WINAPI
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_quitEventLoop(JNIEnv *env, jclass cls) {
#ifdef USE_XCB
	// TODO
#endif
#ifdef USE_WINAPI
	PostQuitMessage(0);
#endif
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
	uint32_t values[] = { XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_MOTION |
			XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE };
	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, (xcb_window_t) parent, x, y,
					  (uint16_t) width, (uint16_t) height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
					  screen->root_visual, XCB_CW_EVENT_MASK, values);
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, (xcb_window_t) window,
						XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, (uint32_t) strlen(titleChars), titleChars);
	result = window;
#endif
#ifdef USE_WINAPI
	if (flags & WINDOW_FLAG_TOPLEVEL) {
		parent = (jlong) (size_t) HWND_DESKTOP;
	}
	HWND window = CreateWindowEx(0, WND_CLASS_NAME, titleChars, WS_OVERLAPPEDWINDOW, x, y, width, height,
			(HWND) (size_t) parent, NULL, hInstance, NULL);
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
