package com.eternal_search.njt;

public class KeyCode {
	
	public static final int ENTER = 13;
	public static final int ESCAPE = 27;
	
	static void loadKeyCodeMap() {
		if (PlatformInfo.getOsId().equals("linux")) {
			loadLinuxKeyCodeMap();
		}
	}
	
	private static void loadLinuxKeyCodeMap() {
		Window.keyCodeMap.put(65293, ENTER);
		Window.keyCodeMap.put(65307, ESCAPE);
	}
	
}
