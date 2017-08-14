package com.eternal_search.njt;

import java.util.HashMap;
import java.util.Map;

public class Window {
	
	private static final Map<Long, Window> windows = new HashMap<Long, Window>();
	
	private final long id;
	
	public Window(long id) {
		this.id = id;
		assert !windows.containsKey(id);
		windows.put(id, this);
	}
	
	public Window(String title, int x, int y, int width, int height, int flags, int parent) {
		this(NativeFunctions.createWindow(title, x, y, width, height, flags, parent));
	}
	
	public Window(String title, int x, int y, int width, int height, int flags) {
		this(title, x, y, width, height, flags | Flags.TOPLEVEL, -1);
	}
	
	public void destroy() {
		NativeFunctions.destroyWindow(id);
		windows.remove(id);
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
	
	public static Window getInstance(long id, boolean create) {
		Window window = windows.getOrDefault(id, null);
		if ((window == null) && create) {
			window = new Window(id);
		}
		return window;
	}
	
	public static Window getInstance(long id) {
		return getInstance(id, false);
	}
	
	public static final class Flags {
		
		private Flags() {
		}
		
		public static final int TOPLEVEL = 1 << 0;
		
	}
	
}
