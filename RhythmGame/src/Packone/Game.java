package Packone;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;

import javax.swing.ImageIcon;

public class Game extends Thread {
	private Image gameInfoImage = new ImageIcon(Main.class.getResource("/images/gameInfo.png")).getImage();
	
	private String titleName;
	private String difficulty;
	private String musicTitle;
	private Video video;
	private Image img = null;

	
	// private Music gameMusic;

	// ArrayList<Note> noteList = new ArrayList<Note>();

	public Game(String titleName, String difficulty, String musicTitle) {
		video = new Video(); // camera read start
		this.titleName = titleName;
		this.difficulty = difficulty;
		this.musicTitle = musicTitle;
		// gameMusic = new Music(this.musicTitle, false);

	}

	public void screenDraw(Graphics2D g) {
		img = video.readCam();
		g.drawImage(img, 0, 0, null);
		g.drawImage(gameInfoImage, 0, 660, null);
		/*
		 * for (int i = 0; i < noteList.size(); i++) { Note note =
		 * noteList.get(i); if (note.getY() > 620) { judgeImage = new
		 * ImageIcon(Main.class.getResource("/images/miss.png")).getImage();
		 * 
		 * } if (!note.isProceeded()) { // noteList.remove(i); i--; } else {
		 * note.screenDraw(g); } note.screenDraw(g); }
		 */
		g.setColor(Color.white);
		g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
		g.drawString(titleName, 20, 702);
		g.drawString(difficulty, 1190, 702);
		g.setFont(new Font("Aeial", Font.PLAIN, 28));
		g.setColor(Color.LIGHT_GRAY);
		g.setFont(new Font("Elephant", Font.BOLD, 30));
		g.drawString("000000", 565, 702);

	}


	@Override
	public void run() {
		// dropNotes(this.titleName);
	}

	public void close() {
		// gameMusic.close();
		video.close();
		this.interrupt();
	}

	/*
	 * public void dropNotes(String titleName) { Beat[] beats = null; if
	 * (titleName.equals("Jolly_Old_St_Nicholas_Instrumental") &&
	 * difficulty.equals("Easy")) { int startTime = 4400 - Main.REACH_TIME *
	 * 1000; int gap = 125; beats = new Beat[] { new Beat(startTime + gap * 1,
	 * "S"), new Beat(startTime + gap * 3, "D"), new Beat(startTime + gap * 5,
	 * "S"), new Beat(startTime + gap * 7, "D"), new Beat(startTime + gap * 9,
	 * "S"), new Beat(startTime + gap * 11, "D"), new Beat(startTime + gap * 13,
	 * "S"), new Beat(startTime + gap * 15, "D"), new Beat(startTime + gap * 18,
	 * "J"), new Beat(startTime + gap * 20, "K"), new Beat(startTime + gap * 22,
	 * "J"), new Beat(startTime + gap * 24, "K"), new Beat(startTime + gap * 26,
	 * "J"), new Beat(startTime + gap * 28, "K"), new Beat(startTime + gap * 30,
	 * "J"), new Beat(startTime + gap * 32, "K"), new Beat(startTime + gap * 35,
	 * "S"), new Beat(startTime + gap * 37, "D"), new Beat(startTime + gap * 39,
	 * "S"), }; } else if
	 * (titleName.equals("Jolly_Old_St_Nicholas_Instrumental") &&
	 * difficulty.equals("Hard")) { int startTime = 1000 - Main.REACH_TIME *
	 * 1000; beats = new Beat[] { new Beat(startTime, "Space"), }; } else if
	 * (titleName.equals("7th_Floor_Tango") && difficulty.equals("Easy")) { int
	 * startTime = 1000 - Main.REACH_TIME * 1000; beats = new Beat[] { new
	 * Beat(startTime, "Space"), }; } else if
	 * (titleName.equals("7th_Floor_Tango") && difficulty.equals("Hard")) { int
	 * startTime = 1000 - Main.REACH_TIME * 1000; beats = new Beat[] { new
	 * Beat(startTime, "Space"), }; } else if (titleName.equals("Up_Above") &&
	 * difficulty.equals("Easy")) { int startTime = 1000 - Main.REACH_TIME *
	 * 1000; beats = new Beat[] { new Beat(startTime, "Space"), }; } else if
	 * (titleName.equals("Up_Above") && difficulty.equals("Hard")) { int
	 * startTime = 1000 - Main.REACH_TIME * 1000; beats = new Beat[] { new
	 * Beat(startTime, "Space"), }; }
	 * 
	 * int i = 0; gameMusic.start(); while (i < beats.length &&
	 * !isInterrupted()) { boolean dropped = false; if (beats[i].getTime() <=
	 * gameMusic.getTime()) { Note note = new Note(beats[i].getNoteName());
	 * note.start(); noteList.add(note); i++; } if (!dropped) { try {
	 * Thread.sleep(5); } catch (Exception e) { e.printStackTrace(); } } } }
	 * 
	 * public void judge(String input) { for (int i = 0; i < noteList.size();
	 * i++) { // 큐처럼 구현 Note note = noteList.get(i); if
	 * (input.equals(note.getNoteType())) { judgeEvent(note.judge()); break; } }
	 * }
	 * 
	 * public void judgeEvent(String judge) { if (!judge.equals("None")) {
	 * blueFlareImage = new
	 * ImageIcon(Main.class.getResource("/images/blueFlare.png")).getImage(); }
	 * if (judge.equals("Miss")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/miss.png")).getImage(); } else
	 * if (judge.equals("Late")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/late.png")).getImage(); } else
	 * if (judge.equals("Good")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/good.png")).getImage(); } else
	 * if (judge.equals("Great")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/great.png")).getImage(); } else
	 * if (judge.equals("Perfect")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/perfect.png")).getImage(); }
	 * else if (judge.equals("Early")) { judgeImage = new
	 * ImageIcon(Main.class.getResource("/images/early.png")).getImage(); }
	 * 
	 * }
	 */
}
