import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;
import java.lang.*;

public class OverlayPanel extends JPanel
{
  public BufferedImage textImage;
  public int _top, _left, _width, _height;

  public Color  lineColor;
  public int    backgroundAlpha;

  public OverlayPanel(int x, int y, int width, int height, int alpha, Color line)
  {
    this.setBackground(new Color(255, 255, 255, alpha));
    this.adjustLocation(x, y, width, height);
    this.lineColor = line;
    this.backgroundAlpha = alpha;
    this.drawLine();
  }

  public void adjustLocation(int x, int y, int width, int height) {
    this._top = y;
    this._left = x;
    this._width = width;
    this._height = height;
    this.setSize(_width, _height);
  }
  public void drawLine() {
    int DRAW_TIMES = 5;
    BufferedImage image = new BufferedImage(this._width, this._height, BufferedImage.TYPE_INT_ARGB);
    Graphics2D g2d = image.createGraphics();
    g2d.setBackground(new Color(255, 255, 255, this.backgroundAlpha));
    g2d.clearRect(0, 0, this._width, this._height);

    g2d.setPaint(this.lineColor);

    for(int i=0; i<DRAW_TIMES; ++i) {
        g2d.drawRect(i, i, this._width-i*2-1, this._height-i*2-1);
    }
    g2d.dispose();
    this.textImage = image;
  }

  @Override
  protected void paintComponent(Graphics g)
  {
    super.paintComponent(g);

    // draw text image here..
    g.drawImage(this.textImage, 0, 0, null);
  }


  public static void main(String[] args)
  {
  }
}
