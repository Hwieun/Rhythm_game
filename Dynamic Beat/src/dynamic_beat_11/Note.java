package dynamic_beat_11;

import java.awt.Graphics2D;
import java.awt.Image;

import javax.swing.ImageIcon;

import dynamic_beat_11.Main;

public class Note extends Thread {
	private Image noteBasicImage = new ImageIcon(Main.class.getResource("../images/noteBasic.png")).getImage();
	private int x, y = 580 - (1000 / dynamic_beat_11.Main.SLEEP_TIME * dynamic_beat_11.Main.NOTE_SPEED) * dynamic_beat_11.Main.REACH_TIME;
	private String noteType;
	private boolean proceeded = true;
	
	public String getNoteType(){
		return noteType;
	}
	
	public boolean isProceeded(){
		return proceeded;
	}
	
	public void close(){
		proceeded = false;
	}

	public Note(String noteType) {
		if (noteType.equals("S")) {
			x = 228;
		} else if (noteType.equals("D")) {
			x = 332;
		} else if (noteType.equals("F")) {
			x = 436;

		} else if (noteType.equals("Space")) {
			x = 540;

		} else if (noteType.equals("J")) {
			x = 744;

		} else if (noteType.equals("K")) {
			x = 848;

		} else if (noteType.equals("L")) {
			x = 952;

		}
		this.noteType = noteType;

	}

	public void screenDraw(Graphics2D g) {
		if (noteType.equals("Space")) {
			g.drawImage(noteBasicImage, x, y, null);
			g.drawImage(noteBasicImage, x + 100, y, null);
		} else {
			g.drawImage(noteBasicImage, x, y, null);
		}
	}

	public void drop() {
		y += dynamic_beat_11.Main.NOTE_SPEED;
		if(y > 620){
			System.out.println("Miss");
			close();
		}
	}

	@Override
	public void run() {
		try {
			while (true) {
				drop();
				if(proceeded){ //노트 작업이 끝나면 false로 바껴서 노트가 움직이지 않게 한다. 스레드 정지시킴
					
					Thread.sleep(dynamic_beat_11.Main.SLEEP_TIME);
				}
				else{
					interrupt();
					break;
				}
				Thread.sleep(dynamic_beat_11.Main.SLEEP_TIME);
			}
		} catch (Exception e) {
			System.err.println(e.getMessage());
		}
	}
	
	public String judge(){
		if(y >= 613){
			System.out.println("Late");
			close();
			return "Late";
		}
		else if(y >= 600){
			System.out.println("Good");
			close();
			return "Good";
		}
		else if(y >= 587){
			System.out.println("Great");
			close();
			return "Great";
		}
		else if(y >= 573){
			System.out.println("Perfet");
			close();
			return "Perfect";
		}
		else if(y >= 565){
			System.out.println("Great");
			close();
			return "Great";
		}
		else if(y >= 558){
			System.out.println("Good");
			close();
			return "Good";
		}
		else if(y >= 535){
			System.out.println("Early");
			close();
			return "Early";
		}
		return "None";

	}
	
	public int getY(){
		return y;
	}
}
