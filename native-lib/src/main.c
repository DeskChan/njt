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
jmethodID javaWindowHandleMouseDownMethod;
jmethodID javaWindowHandleMouseUpMethod;
jmethodID javaWindowHandleKeyDownMethod;
jmethodID javaWindowHandleKeyUpMethod;
jmethodID javaWindowHandleBoundsChanged;
jmethodID javaWindowHandlePaint;
jclass javaRectClass;
jmethodID javaRectConstructor;

void handleCloseEvent(jlong window) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleCloseMethod, window);
}

void handleMouseMoveEvent(jlong window, jint x, jint y, jint rootX, jint rootY, jint buttons) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleMouseMoveMethod,
									 window, x, y, rootX, rootY, buttons);
}

void handleMouseDownEvent(jlong window, jint x, jint y, jint rootX, jint rootY, jint buttons) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleMouseDownMethod,
									 window, x, y, rootX, rootY, buttons);
}

void handleMouseUpEvent(jlong window, jint x, jint y, jint rootX, jint rootY, jint buttons) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleMouseUpMethod,
									 window, x, y, rootX, rootY, buttons);
}

void handleKeyDownEvent(jlong window, jint keyCode) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleKeyDownMethod, window, keyCode);
}

void handleKeyUpEvent(jlong window, jint keyCode) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleKeyUpMethod, window, keyCode);
}

void handleWindowBoundsChangedEvent(jlong window, jint x, jint y, jint width, jint height) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandleBoundsChanged,
									 window, x, y, width, height);
}

void handlePaintEvent(jlong window) {
	(*javaEnv)->CallStaticVoidMethod(javaEnv, javaWindowClass, javaWindowHandlePaint, window);
}

#ifdef USE_XCB

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

xcb_connection_t *connection;
xcb_screen_t *screen;
int quit = 0;

void handleEvent(xcb_generic_event_t *event) {
	switch (event->response_type & ~0x80) {
		case XCB_DESTROY_NOTIFY:
			handleCloseEvent((jlong) ((xcb_destroy_notify_event_t*) event)->window);
			break;
		case XCB_CONFIGURE_NOTIFY: {
			xcb_configure_notify_event_t *e = (xcb_configure_notify_event_t*) event;
			handleWindowBoundsChangedEvent((jlong) e->window, e->x, e->y, e->width, e->height);
			break;
		}
		case XCB_MOTION_NOTIFY: {
			xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t*) event;
			jint buttons = 0;
			if (e->state & XCB_BUTTON_MASK_1) {
				buttons |= 1 << 0;
			}
			if (e->state & XCB_BUTTON_MASK_2) {
				buttons |= 1 << 1;
			}
			if (e->state & XCB_BUTTON_MASK_3) {
				buttons |= 1 << 2;
			}
			handleMouseMoveEvent((jlong) e->event, (jint) e->event_x, (jint) e->event_y,
											 (jint) e->root_x, (jint) e->root_y, buttons);
			break;
		}
		case XCB_BUTTON_PRESS: {
			xcb_button_press_event_t *e = (xcb_button_press_event_t*) event;
			handleMouseDownEvent((jlong) e->event, (jint) e->event_x, (jint) e->event_y,
								 (jint) e->root_x, (jint) e->root_y, 1 << (e->detail - 1));
			break;
		}
		case XCB_BUTTON_RELEASE: {
			xcb_button_release_event_t *e = (xcb_button_release_event_t*) event;
			handleMouseUpEvent((jlong) e->event, (jint) e->event_x, (jint) e->event_y,
								 (jint) e->root_x, (jint) e->root_y, 1 << (e->detail - 1));
			break;
		}
		case XCB_KEY_PRESS: {
			xcb_key_press_event_t *e = (xcb_key_press_event_t*) event;
			xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(connection);
			if (keysyms) {
				xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, e->detail, 0);
				xcb_key_symbols_free(keysyms);
				handleKeyDownEvent((jlong) e->event, keysym);
			}
			break;
		}
		case XCB_KEY_RELEASE: {
			xcb_key_press_event_t *e = (xcb_key_press_event_t*) event;
			xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(connection);
			if (keysyms) {
				xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, e->detail, 0);
				xcb_key_symbols_free(keysyms);
				handleKeyUpEvent((jlong) e->event, keysym);
			}
			break;
		}
		case XCB_EXPOSE: {
			xcb_expose_event_t *e = (xcb_expose_event_t*) event;
			handlePaintEvent((jlong) e->window);
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

static void handleMouseEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	jlong wnd = (jlong) (size_t) hWnd;
	jint x = lParam & 0xFFFF;
	jint y = lParam >> 16;
	POINT rootPos;
	GetCursorPos(&rootPos);
	switch (uMsg) {
		case WM_LBUTTONDOWN:
			handleMouseDownEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 0);
			break;
		case WM_LBUTTONUP:
			handleMouseUpEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 0);
			break;
		case WM_MBUTTONDOWN:
			handleMouseDownEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 1);
			break;
		case WM_MBUTTONUP:
			handleMouseUpEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 1);
			break;
		case WM_RBUTTONDOWN:
			handleMouseDownEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 2);
			break;
		case WM_RBUTTONUP:
			handleMouseUpEvent(wnd, x, y, rootPos.x, rootPos.y, 1 << 2);
			break;
		case WM_MOUSEMOVE: {
			jint buttons = 0;
			if (wParam & MK_LBUTTON) {
				buttons |= 1 << 0;
			}
			if (wParam & MK_MBUTTON) {
				buttons |= 1 << 1;
			}
			if (wParam & MK_RBUTTON) {
				buttons |= 1 << 2;
			}
			handleMouseMoveEvent(wnd, x, y, rootPos.x, rootPos.y, buttons);
			break;
		}
		default:;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			handleCloseEvent((jlong) (size_t) hWnd);
			break;
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			handleMouseEvent(hWnd, uMsg, wParam, lParam);
			return 0;
		case WM_KEYDOWN:
			handleKeyDownEvent((jlong) (size_t) hWnd, wParam);
			return 0;
		case WM_KEYUP:
			handleKeyUpEvent((jlong) (size_t) hWnd, wParam);
			return 0;
		case WM_WINDOWPOSCHANGED: {
			WINDOWPOS *pos = (WINDOWPOS*) lParam;
			handleWindowBoundsChangedEvent((jlong) (size_t) hWnd, pos->x, pos->y, pos->cx, pos->cy);
			break;
		}
		case WM_PAINT:
			handlePaintEvent((jlong) (size_t) hWnd);
			return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_init(JNIEnv *env, jclass cls) {
	javaEnv = env;
	jclass tmpClassRef = (*env)->FindClass(env, "com/eternal_search/njt/Window");
	javaWindowClass = (*env)->NewGlobalRef(env, tmpClassRef);
	(*env)->DeleteLocalRef(env, tmpClassRef);
	javaWindowHandleCloseMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleClose", "(J)V");
	javaWindowHandleMouseMoveMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleMouseMove",
																"(JIIIII)V");
	javaWindowHandleMouseDownMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleMouseDown",
																"(JIIIII)V");
	javaWindowHandleMouseUpMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleMouseUp",
																"(JIIIII)V");
	javaWindowHandleKeyDownMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleKeyDown",
																"(JI)V");
	javaWindowHandleKeyUpMethod = (*env)->GetStaticMethodID(env, javaWindowClass, "handleKeyUp",
															  "(JI)V");
	javaWindowHandleBoundsChanged = (*env)->GetStaticMethodID(env, javaWindowClass, "handleBoundsChanged",
																"(JIIII)V");
	javaWindowHandlePaint = (*env)->GetStaticMethodID(env, javaWindowClass, "handlePaint", "(J)V");
	tmpClassRef = (*env)->FindClass(env, "com/eternal_search/njt/geom/Rect");
	javaRectClass = (*env)->NewGlobalRef(env, tmpClassRef);
	(*env)->DeleteLocalRef(env, tmpClassRef);
	javaRectConstructor = (*env)->GetMethodID(env, javaRectClass, "<init>", "(IIII)V");
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
	(*env)->DeleteGlobalRef(env, javaWindowClass);
	(*env)->DeleteGlobalRef(env, javaRectClass);
	javaEnv = NULL;
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_runEventLoop(JNIEnv *env, jclass cls) {
#ifdef USE_XCB
	xcb_flush(connection);
	xcb_generic_event_t *event;
	while (!quit && ((event = xcb_wait_for_event(connection)) != NULL)) {
		handleEvent(event);
		free(event);
		xcb_flush(connection);
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
	quit = 1;
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
	uint32_t values[] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY |  XCB_EVENT_MASK_POINTER_MOTION |
								  XCB_EVENT_MASK_BUTTON_MOTION |
			XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE };
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
	xcb_size_hints_t hints = {};
	xcb_icccm_size_hints_set_win_gravity(&hints, XCB_GRAVITY_STATIC);
	xcb_icccm_size_hints_set_position(&hints, 1, x, y);
	xcb_icccm_size_hints_set_size(&hints, 1, width, height);
	xcb_icccm_set_wm_normal_hints(connection, (xcb_window_t) window, &hints);
	const uint32_t values[] = { (uint32_t) x, (uint32_t) y, (uint32_t) width, (uint32_t) height };
	xcb_configure_window(connection, (xcb_window_t) window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
#endif
#ifdef USE_WINAPI
	MoveWindow((HWND) (size_t) window, x, y, width, height, TRUE);
#endif
}

JNIEXPORT jclass JNICALL Java_com_eternal_1search_njt_NativeFunctions_getWindowRect(JNIEnv *env,
																					jclass cls,
																					jlong window) {
	jclass result = 0;
#ifdef USE_XCB
	xcb_flush(connection);
	xcb_get_geometry_cookie_t cookie1 = xcb_get_geometry(connection, (xcb_drawable_t) window);
	xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(connection, cookie1, NULL);
	if (geom) {
		xcb_translate_coordinates_cookie_t cookie2 =
				xcb_translate_coordinates(connection, (xcb_window_t) window, screen->root,
										  geom->x, geom->y);
		xcb_translate_coordinates_reply_t *trans =
				xcb_translate_coordinates_reply(connection, cookie2, NULL);
		if (trans) {
			result = (*env)->NewObject(env, javaRectClass, javaRectConstructor,
									   (jint) trans->dst_x, (jint) trans->dst_y,
									   (jint) geom->width, (jint) geom->height);
			free(trans);
		}
		free(geom);
	}
#endif
#ifdef USE_WINAPI
	RECT rect;
	if (GetWindowRect((HWND) (size_t) window, &rect)) {
		result = (*env)->NewObject(env, javaRectClass, javaRectConstructor,
				(jint) rect.left, (jint) rect.top, (jint) (rect.right - rect.left),
				(jint) (rect.bottom - rect.top));
	}
#endif
	return result;
}
