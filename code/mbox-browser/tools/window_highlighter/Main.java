import java.awt.Color;
import java.lang.Thread;

public class Main
{
  public static void main(String[] args) throws Exception
  {
    int X_INDEX = 0;
    int Y_INDEX = 1;
    int W_INDEX = 2;
    int H_INDEX = 3;
    int A_INDEX = 4;
    int R_INDEX = 5;
    int G_INDEX = 6;
    int B_INDEX = 7;

    int x,y,w,h,a,r,g,b;
    x = Integer.parseInt(args[X_INDEX]);
    y = Integer.parseInt(args[Y_INDEX]);
    w = Integer.parseInt(args[W_INDEX]);
    h = Integer.parseInt(args[H_INDEX]);
    a = Integer.parseInt(args[A_INDEX]);
    r = Integer.parseInt(args[R_INDEX]);
    g = Integer.parseInt(args[G_INDEX]);
    b = Integer.parseInt(args[B_INDEX]);

    OverlayFrame overlayFrame = new OverlayFrame(x, y, w, h, a,
            new Color(r, g, b, 255));
    overlayFrame.showFrame();

    while(true){
      Thread.sleep(1048576);
    }
  }
}
