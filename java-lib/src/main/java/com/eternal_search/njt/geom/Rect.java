package com.eternal_search.njt.geom;

public class Rect {
	
	public Point topLeft;
	public Point rightBottom;
	
	public Rect(Point topLeft, Point rightBottom) {
		this.topLeft = topLeft;
		this.rightBottom = rightBottom;
	}
	
	public Rect(int x, int y, int width, int height) {
		this(new Point(x, y), new Point(x + width, y + height));
	}
	
	@Override
	public String toString() {
		return "Rect(" + topLeft.x + ", " + topLeft.y + ", " + rightBottom.x + ", " +
				rightBottom.y + ")";
	}
	
	public int getLeft() {
		return topLeft.x;
	}
	
	public void setLeft(int value) {
		topLeft.x = value;
	}
	
	public int getTop() {
		return topLeft.y;
	}
	
	public void setTop(int value) {
		topLeft.y = value;
	}
	
	public int getRight() {
		return rightBottom.x;
	}
	
	public void setRight(int value) {
		rightBottom.x = value;
	}
	
	public int getBottom() {
		return rightBottom.y;
	}
	
	public void setBottom(int value) {
		rightBottom.y = value;
	}
	
	public int getX() {
		return getLeft();
	}
	
	public void setX(int value) {
		setRight(getRight() - (getLeft() - value));
		setLeft(value);
	}
	
	public int getY() {
		return getTop();
	}
	
	public void setY(int value) {
		setBottom(getBottom() - (getTop() - value));
		setTop(value);
	}
	
	public int getWidth() {
		return getRight() - getLeft();
	}
	
	public void setWidth(int value) {
		setRight(getX() + value);
	}
	
	public int getHeight() {
		return getBottom() - getTop();
	}
	
	public void setHeight(int value) {
		setBottom(getY() + value);
	}
	
	public void setTopLeft(Point value) {
		this.topLeft.x = value.x;
		this.topLeft.y = value.y;
	}
	
	public void setRightBottom(Point value) {
		this.rightBottom.x = value.x;
		this.rightBottom.y = value.y;
	}
	
	public void setXY(int x, int y) {
		setX(x);
		setY(y);
	}
	
	public void setXY(Point value) {
		setXY(value.x, value.y);
	}
	
	public void setSize(int width, int height) {
		setWidth(width);
		setHeight(height);
	}
	
	public void adjustPos(int dx, int dy) {
		setXY(getX() + dx, getY() + dy);
	}
	
	public void adjustPos(Point point) {
		adjustPos(point.x, point.y);
	}
	
	public void adjustSize(int dx, int dy) {
		setSize(getWidth() + dx, getHeight() + dy);
	}
	
}
