#include <stdio.h>
#include "jni.h"

#ifdef _WIN32
#undef JNICALL
#define JNICALL __cdecl
#endif

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_init(JNIEnv *env, jclass cls) {
	fprintf(stderr, "NJT: Init\n");
}

JNIEXPORT void JNICALL Java_com_eternal_1search_njt_NativeFunctions_deInit(JNIEnv *env, jclass cls) {
	fprintf(stderr, "NJT: DeInit\n");
}
