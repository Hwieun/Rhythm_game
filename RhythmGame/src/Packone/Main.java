package Packone;

	import java.awt.Image;

import org.opencv.core.Core;

public class Main {
	static {
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
	}

	public static int SCREEN_WIDTH = 1280;
	public static int SCREEN_HEIGHT = 720;
	public static Rhythm rhythm;
	
	public static void main(String[] args) {
		rhythm = new Rhythm();
	}
}
////////////클래스 수 줄이기////////
///////////RW보다 IO가 더 빠름(byte)