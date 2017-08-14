package com.eternal_search.njt;

public final class Main {
	
	public static void main(String[] args) {
		System.err.println("Platform ID: " + PlatformInfo.getPlatformId());
		NativeFunctions.loadLibrary();
		NativeFunctions.init();
		Window window = new Window("Java Native Toolkit Demo", 0, 0,
				400, 300, 0);
		window.show();
		runEventLoop();
		window.destroy();
		NativeFunctions.deInit();
	}
	
	public static void runEventLoop() {
		NativeFunctions.Event event;
		while ((event = NativeFunctions.waitForEvent()) != null) {
			// TODO
		}
	}
	
}
