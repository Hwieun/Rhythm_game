package dynamic_beat_11;

import org.opencv.core.Core;

public class Main {

	public static final int SCREEN_WIDTH = 1280;
	public static final int SCREEN_HEIGHT = 720;
	public static final int NOTE_SPEED = 3;
	public static final int SLEEP_TIME = 10;
	public static final int REACH_TIME = 2;

	static {
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
	}
	
	public static void main(String[] args) {
		System.out.println("Main start\n");
		
		new DynamicBeat();
	} 

}
     