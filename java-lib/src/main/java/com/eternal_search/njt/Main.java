package com.eternal_search.njt;

import org.lwjgl.*;
import org.lwjgl.opengl.*;
import static org.lwjgl.opengl.GL11.*;

import com.eternal_search.njt.geom.Point;
import com.eternal_search.njt.geom.Rect;

public final class Main {
	
	public static void main(String[] args) {
		System.err.println("Platform ID: " + PlatformInfo.getPlatformId());
		NativeFunctions.loadLibrary();
		if (!NativeFunctions.init()) {
			System.err.println("Failed to initialize native backend!");
			return;
		}
		Window window = new Window("Java Native Toolkit Demo", 0, 0,
				300, 200, 0) {
			
			private Point lastMousePos;
			
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
					setBounds(bounds.move(bounds.getX() + deltaX, bounds.getY() + deltaY));
					lastMousePos = new Point(rootX, rootY);
				}
			}
			
			@Override
			protected void onKeyDown(int keyCode) {
				if (keyCode == KeyCode.ESCAPE) {
					NativeFunctions.quitEventLoop();
				}
			}
			
			@Override
			protected void paint() {
				GL.createCapabilities();
				glClearColor(0.2f, 0.4f, 0.9f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			}
			
		};
		window.show();
		window.setBounds(100, 100, 400, 300);
		NativeFunctions.runEventLoop();
		window.destroy();
		NativeFunctions.deInit();
	}
	
}
