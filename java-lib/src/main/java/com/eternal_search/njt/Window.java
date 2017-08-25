package com.eternal_search.njt;

import com.eternal_search.njt.geom.Rect;

import java.util.HashMap;
import java.util.Map;

public class Window {
	
	private static final Map<Long, Window> windows = new HashMap<Long, Window>();
	
	private final long id;
	private final boolean managedByApplication;
	private Rect cachedBounds = null;
	
	private Window(long id, boolean managedByApplication) {
		this.id = id;
		this.managedByApplication = managedByApplication;
		assert !windows.containsKey(id);
		windows.put(id, this);
	}
	
	public Window(long id) {
		this(id, false);
	}
	
	public Window(String title, int x, int y, int width, int height, int flags, int parent) {
		this(NativeFunctions.createWindow(title, x, y, width, height, flags, parent), true);
		cachedBounds = new Rect(x, y, width, height);
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
	
	public void show() {
		NativeFunctions.showWindow(id);
	}
	
	public void hide() {
		NativeFunctions.hideWindow(id);
	}
	
	public Rect getBounds() {
		return managedByApplication ? cachedBounds : NativeFunctions.getWindowRect(id);
	}
	
	public void setBounds(Rect bounds) {
		NativeFunctions.moveWindow(id, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
		if (managedByApplication) {
			cachedBounds = bounds;
		}
	}
	
	public void setBounds(int x, int y, int width, int height) {
		setBounds(new Rect(x, y, width, height));
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
	
	protected void onClose() {
	}
	
	protected void onMouseMove(int x, int y, int rootX, int rootY, int buttons) {
	}
	
	protected void onMouseDown(int x, int y, int rootX, int rootY, int buttons) {
	}
	
	protected void onMouseUp(int x, int y, int rootX, int rootY, int buttons) {
	}
	
	protected void onBoundsChanged(Rect newBounds) {
		this.cachedBounds = newBounds;
	}
	
	private static void handleClose(long id) {
		Window window = getInstance(id);
		if (window != null) {
			window.onClose();
		}
	}
	
	private static void handleMouseMove(long id, int x, int y, int rootX, int rootY, int buttons) {
		Window window = getInstance(id);
		if (window != null) {
			window.onMouseMove(x, y, rootX, rootY, buttons);
		}
	}
	
	private static void handleMouseDown(long id, int x, int y, int rootX, int rootY, int buttons) {
		Window window = getInstance(id);
		if (window != null) {
			window.onMouseDown(x, y, rootX, rootY, buttons);
		}
	}
	
	private static void handleMouseUp(long id, int x, int y, int rootX, int rootY, int buttons) {
		Window window = getInstance(id);
		if (window != null) {
			window.onMouseUp(x, y, rootX, rootY, buttons);
		}
	}
	
	private static void handleBoundsChanged(long id, int x, int y, int width, int height) {
		Window window = getInstance(id);
		if (window != null) {
			window.onBoundsChanged(new Rect(x, y, width, height));
		}
	}
	
	public static final class Flags {
		
		private Flags() {
		}
		
		public static final int TOPLEVEL = 1 << 0;
		
	}
	
}
