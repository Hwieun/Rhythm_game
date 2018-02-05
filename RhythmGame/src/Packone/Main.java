package Packone;

import java.awt.Image;

import org.opencv.core.Core;

public class Main {
	static {
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
	}
//	private static Video video;
	public static int SCREEN_WIDTH = 1280;
	public static int SCREEN_HEIGHT = 720;
	
	public static void main(String[] args) {
		new Rhythm();
//		video = new Video(); //camera read start
//		MyFrame m_frame = new MyFrame();
//		m_frame.setSize(1280, 720); // pack¿∫ æ»µ 
//		Image img = null;
//		m_frame.addKeyListener(new KeyListener());
		
//		while(true){
//			img = video.readCam();
//			m_frame.render(img);
//		}
	}
}
