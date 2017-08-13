package com.eternal_search.njt;

public class Main {
	
	public static void main(String[] args) {
		System.err.println("Platform ID: " + PlatformInfo.getPlatformId());
		NativeFunctions.loadLibrary();
		NativeFunctions.init();
		// TODO
		NativeFunctions.deInit();
	}
	
}
