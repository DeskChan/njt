package com.eternal_search.njt;

import com.eternal_search.njt.geom.Point;
import com.eternal_search.njt.geom.Rect;

public final class Main {
	
	public static void main(String[] args) {
		System.err.println("Platform ID: " + PlatformInfo.getPlatformId());
		NativeFunctions.loadLibrary();
		NativeFunctions.init();
		Window window = new Window("Java Native Toolkit Demo", 0, 0,
				400, 300, 0) {
			
			Point lastMousePos;
			
			@Override
			protected void onClose() {
				System.err.println("onClose()");
				NativeFunctions.quitEventLoop();
			}
			
			@Override
			protected void onMouseDown(int x, int y, int rootX, int rootY, int buttons) {
				lastMousePos = new Point(rootX, rootY);
			}
			
			@Override
			protected void onMouseUp(int x, int y, int rootX, int rootY, int buttons) {
				lastMousePos = null;
			}
			
			@Override
			protected void onMouseMove(int x, int y, int rootX, int rootY, int buttons) {
				if ((lastMousePos != null) && (buttons != 0)) {
					int deltaX = rootX - lastMousePos.x;
					int deltaY = rootY - lastMousePos.y;
					Rect bounds = getBounds();
					//System.out.println("Before: " + bounds);
					bounds.adjustPos(deltaX, deltaY);
					//System.out.println("After: " + bounds);
					move(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
					lastMousePos.x = rootX;
					lastMousePos.y = rootY;
				}
			}
			
		};
		window.show();
		window.move(100, 100, 400, 300);
		System.out.println(window.getBounds());
		NativeFunctions.runEventLoop();
		window.destroy();
		NativeFunctions.deInit();
	}
	
}
