package com.eternal_search.njt;

public final class Main {
	
	public static void main(String[] args) {
		System.err.println("Platform ID: " + PlatformInfo.getPlatformId());
		NativeFunctions.loadLibrary();
		NativeFunctions.init();
		Window window = new Window("Java Native Toolkit Demo", 0, 0,
				400, 300, 0) {
			
			@Override
			protected void onClose() {
				System.err.println("onClose()");
				NativeFunctions.quitEventLoop();
			}
			
			@Override
			protected void onMouseMove(int x, int y, int rootX, int rootY) {
				System.err.println("onMouseMove(" + x + ", " + y + ", " + rootX + ", " + rootY + ")");
			}
			
		};
		window.show();
		NativeFunctions.runEventLoop();
		window.destroy();
		NativeFunctions.deInit();
	}
	
}
