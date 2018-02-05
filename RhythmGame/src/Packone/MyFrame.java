package Packone;

import java.awt.Image;

import javax.swing.JFrame;

public class MyFrame extends JFrame {

	private final JFrame frame;
	public static MyPanel panel;

	public MyFrame() {
		frame = new JFrame("TEST");
		panel = new MyPanel();
		frame.setResizable(false);
		frame.setLayout(null);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setContentPane(panel); // 새로 정의한 MyPanel 컴포넌트등록
		frame.setVisible(true);// 안하면 창이 안뜸
	
//d		addKeyListener(new KeyListener());
	}

}
