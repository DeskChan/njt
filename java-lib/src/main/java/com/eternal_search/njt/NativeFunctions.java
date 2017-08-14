package com.eternal_search.njt;

import java.io.File;

final class NativeFunctions {
	
	private NativeFunctions() {
	}
	
	static void loadLibrary() {
		final String toolkitLibrary = PlatformInfo.getToolkitDirectory() + File.separator +
				"libtoolkit-" + PlatformInfo.getPlatformId() + PlatformInfo.getNativeLibrarySuffix();
		System.err.println("Loading native library: " + toolkitLibrary);
		System.load(toolkitLibrary);
	}
	
	static native void init();
	
	static native void deInit();
	
	static native Event waitForEvent();
	
	static native long createWindow(String title, int x, int y, int width, int height, int flags,
									int parent);
	
	static native void destroyWindow(long window);
	
	static native void showWindow(long window);
	
	static native void hideWindow(long window);
	
	static native void setWindowTitle(long window, String title);
	
	static native void moveWindow(long window, int x, int y, int width, int height);
	
	public static class Event {
		
		public Event() {
		}
		
	}
	
}
