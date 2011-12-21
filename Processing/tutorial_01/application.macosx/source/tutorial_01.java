import processing.core.*; 
import processing.xml.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class tutorial_01 extends PApplet {

public void setup() {
  size(480, 120);
  smooth();
}

public void draw() { 
   if (mousePressed) { 
     fill(0);
   } else {
     fill(255);
   }
   ellipse(mouseX,mouseY,80, 80);
}
  static public void main(String args[]) {
    PApplet.main(new String[] { "--present", "--bgcolor=#666666", "--stop-color=#cccccc", "tutorial_01" });
  }
}
