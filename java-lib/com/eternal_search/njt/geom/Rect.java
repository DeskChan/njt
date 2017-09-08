package com.eternal_search.njt.geom;

public class Rect {
	
	public final Point topLeft;
	public final Point rightBottom;
	
	public Rect(Point topLeft, Point rightBottom) {
		this.topLeft = topLeft;
		this.rightBottom = rightBottom;
	}
	
	public Rect(int x, int y, int width, int height) {
		this(new Point(x, y), new Point(x + width, y + height));
	}
	
	public Rect(Rect rect) {
		this(rect.topLeft, rect.rightBottom);
	}
	
	@Override
	public String toString() {
		return "Rect(" + topLeft.x + ", " + topLeft.y + ", " + rightBottom.x + ", " +
				rightBottom.y + ")";
	}
	
	public int getLeft() {
		return topLeft.x;
	}
	
	public int getTop() {
		return topLeft.y;
	}
	
	public int getRight() {
		return rightBottom.x;
	}
	
	public int getBottom() {
		return rightBottom.y;
	}
	
	public int getX() {
		return getLeft();
	}
	
	public int getY() {
		return getTop();
	}
	
	public int getWidth() {
		return getRight() - getLeft();
	}
	
	public int getHeight() {
		return getBottom() - getTop();
	}
	
	public Rect move(int x, int y) {
		return new Rect(x, y, getWidth(), getHeight());
	}
	
	public Rect resize(int width, int height) {
		return new Rect(getX(), getY(), width, height);
	}
	
}
