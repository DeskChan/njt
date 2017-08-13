package com.eternal_search.njt;

import java.io.File;

public class PlatformInfo {
	
	private static String toolkitDirectory;
	private static String nativeLibrarySuffix;
	private static String platformId;
	private static String osId;
	private static String archId;
	
	public static String getToolkitDirectory() {
		return toolkitDirectory;
	}
	
	public static String getNativeLibrarySuffix() {
		return nativeLibrarySuffix;
	}
	
	public static String getPlatformId() {
		return platformId;
	}
	
	public static String getOsId() {
		return osId;
	}
	
	public static String getArchId() {
		return archId;
	}
	
	private static void detectOS() {
		final String osName = System.getProperty("os.name").toLowerCase();
		if (osName.contains("win")) {
			osId = "windows";
		} else if (osName.contains("linux")) {
			osId = "linux";
		} else {
			osId = "unknown";
		}
	}
	
	private static void detectArch() {
		final String archName = System.getProperty("os.arch").toLowerCase();
		if (archName.contains("64")) {
			archId = "x86_64";
		} else if (archName.contains("86")) {
			archId = "x86";
		} else {
			archId = "unknown";
		}
	}
	
	private static void detect() {
		toolkitDirectory = new File(Main.class.getProtectionDomain().getCodeSource().getLocation().getPath()).getParent();
		detectOS();
		detectArch();
		if (osId.equals("windows")) {
			nativeLibrarySuffix = ".dll";
		} else if (osId.equals("linux")) {
			nativeLibrarySuffix = ".so";
		} else {
			nativeLibrarySuffix = ".bin";
		}
		platformId = osId + "-" + archId;
	}
	
	static {
		detect();
	}
	
}
