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
		frame.setContentPane(panel); // ���� ������ MyPanel ������Ʈ���
		frame.setVisible(true);// ���ϸ� â�� �ȶ�
	
//d		addKeyListener(new KeyListener());
	}

}
