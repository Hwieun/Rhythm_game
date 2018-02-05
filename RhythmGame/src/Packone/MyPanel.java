package Packone;

import java.awt.Cursor;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;

public class MyPanel extends JPanel {
	private Image cam = null;
	private JButton startBt;
	public static int WIDTH = 600;
	public static int HEIGHT = 320;
	public static int check = 0;

	MyPanel() {
		check = 0;
		startBt = new JButton(getImageIcon());
		startBt.setVisible(true);
		startBt.setLocation(25, 25);
		startBt.setBounds(25, 25, 60, 60);
		startBt.setBorderPainted(false);
		startBt.setContentAreaFilled(false);
		startBt.setFocusPainted(false);
		startBt.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseEntered(MouseEvent e) {
				check = 1;
				ImageIcon bf = getImageIcon();
				startBt.setIcon(bf);
				startBt.setLocation(25, 25);
				startBt.setCursor(new Cursor(Cursor.HAND_CURSOR));
			}

			@Override
			public void mouseExited(MouseEvent e) {
				check = 0;
				ImageIcon bf = getImageIcon();
				startBt.setIcon(bf);
				startBt.setLocation(25, 25);
				startBt.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			}

			@Override
			public void mousePressed(MouseEvent e) {
				System.out.println("press");
			}
		});
		add(startBt);
	}

	public void setImage(Image img) {
		this.cam = img;
	}

	public ImageIcon getImageIcon() {
		if (check == 0)
			return 	new ImageIcon(Main.class.getResource("/images/backButtonBasic.png"));

		else if (check == 1)
			return 	new ImageIcon(Main.class.getResource("/images/backButtonEntered.png"));
		return null;
	}

	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
//		System.out.println("paintComponent");
//		Image bf = getImageIcon().getImage();
		g.drawImage(cam, 0, 0, null);
	//	g.drawImage(bf, WIDTH, HEIGHT, null);
	}
}
