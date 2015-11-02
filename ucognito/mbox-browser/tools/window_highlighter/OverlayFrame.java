import java.awt.event.*;
import java.awt.*;
import java.awt.image.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.border.*;

import java.io.*;
import java.util.*;



public class OverlayFrame extends JFrame
{
  int left, top, width, height;
  private int frameX, frameY, frameWidth, frameHeight;
  private OverlayPanel textPanel = null;

  private final int vncXpos = 0;
  private final int vncYpos = 0;

  private final int XMargin = 0;
  private final int YMargin = 0;


  // do not call default constructor
  private OverlayFrame(){}

  public OverlayFrame(int x, int y, int width, int height, int alpha, Color line)
  {
    this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    this.setUndecorated(true);
    this.setBackground(new Color(255,255,255,0));
    this.setFocusable(false);
    this.setFocusableWindowState(false);
    this.setAlwaysOnTop(true);

    this.textPanel = new OverlayPanel(x, y, width, height, alpha, line);
    this.adjustSizeLocation(x, y, width, height);
    this.getContentPane().add(this.textPanel);

 }

  public void adjustSizeLocation(int x, int y, int width, int height)
  {
    this.left = x;
    this.top = y;
    this.width = width;
    this.height = height;
    this.frameX = left + XMargin;
    this.frameY = top + YMargin;
    this.frameWidth = width - XMargin;
    this.frameHeight = height - YMargin;

    this.setSize(this.frameWidth, this.frameHeight);
    this.setLocation(this.frameX + vncXpos, this.frameY + vncYpos);
    this.textPanel.adjustLocation(x, y, width, height);
  }

  public void showFrame()
  {
    this.setVisible(true);
  }

  public void hideFrame()
  {
    this.setVisible(false);
  }

}
