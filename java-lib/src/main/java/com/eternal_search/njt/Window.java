package com.eternal_search.njt;

public class Window {
	
	private final long id;
	
	public Window(long id) {
		this.id = id;
	}
	
	public Window(String title, int x, int y, int width, int height, int flags) {
		this(NativeFunctions.createWindow(title, x, y, width, height, flags));
	}
	
	public void destroy() {
		NativeFunctions.destroyWindow(id);
	}
	
	public void setTitle(String title) {
		NativeFunctions.setWindowTitle(id, title);
	}
	
	public void move(int x, int y, int width, int height) {
		NativeFunctions.moveWindow(id, x, y, width, height);
	}
	
	public void show() {
		NativeFunctions.showWindow(id);
	}
	
	public void hide() {
		NativeFunctions.hideWindow(id);
	}
	
}
