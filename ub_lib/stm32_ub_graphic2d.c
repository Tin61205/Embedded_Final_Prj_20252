//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_graphic2d.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
int16_t P_Graphic2D_sgn(int16_t x);



//--------------------------------------------------------------
// l�scht den Bildschirm mit einer Farbe (per DMA2D)
//--------------------------------------------------------------
void UB_Graphic2D_ClearSreenDMA(uint16_t c)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;

  uint32_t  Xaddress = 0;
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

  Red_Value = (0xF800 & c) >> 11;
  Blue_Value = 0x001F & c;
  Green_Value = (0x07E0 & c) >> 5;

  Xaddress = LCD_CurrentFrameBuffer;

  DMA2D_DeInit();
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
  DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
  DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
  DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;
  DMA2D_InitStruct.DMA2D_OutputOffset = 0;
  DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_MAXY;
  DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_MAXX;
  DMA2D_Init(&DMA2D_InitStruct);

  DMA2D_StartTransfer();

  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}


//--------------------------------------------------------------
// Zeichnet ein Pixel mit einer Farbe an x,y Position
// [x=0...LCD_MAXX, y=0...LCD_MAXY]
//--------------------------------------------------------------
void UB_Graphic2D_DrawPixelNormal(uint16_t xp, uint16_t yp, uint16_t c)
{
  // check auf Limit
  if(xp>=LCD_MAXX) return;
  if(yp>=LCD_MAXY) return;

  // Cursor setzen
  UB_LCD_SetCursor2Draw(xp,yp);
  // Pixel zeichnen
  UB_LCD_DrawPixel(c);
}


//--------------------------------------------------------------
// Zeichnet eine Linie mit einer Farbe
// von x1,y1 nach x2,y2  [x=0...LCD_MAXX, y=0...LCD_MAXY]
// (benutzt wird der Bresenham-Algorithmus)
//--------------------------------------------------------------
void UB_Graphic2D_DrawLineNormal(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  int16_t x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

  // check auf Limit
  if(x1>=LCD_MAXX) x1=LCD_MAXX-1;
  if(y1>=LCD_MAXY) y1=LCD_MAXY-1;

  // check auf Limit
  if(x2>=LCD_MAXX) x2=LCD_MAXX-1;
  if(y2>=LCD_MAXY) y2=LCD_MAXY-1;

  // Entfernung in beiden Dimensionen berechnen
  dx = x2 - x1;
  dy = y2 - y1;

  // Vorzeichen des Inkrements bestimmen
  incx = P_Graphic2D_sgn(dx);
  incy = P_Graphic2D_sgn(dy);
  if(dx<0) dx = -dx;
  if(dy<0) dy = -dy;

  // feststellen, welche Entfernung gr��er ist
  if (dx>dy) {
    // x ist schnelle Richtung
    pdx=incx; pdy=0;    // pd. ist Parallelschritt
    ddx=incx; ddy=incy; // dd. ist Diagonalschritt
    es =dy;   el =dx;   // Fehlerschritte schnell, langsam
  }
  else {
    // y ist schnelle Richtung
    pdx=0;    pdy=incy; // pd. ist Parallelschritt
    ddx=incx; ddy=incy; // dd. ist Diagonalschritt
    es =dx;   el =dy;   // Fehlerschritte schnell, langsam
  }

  // Initialisierungen vor Schleifenbeginn
  x = x1;
  y = y1;
  err = (el>>1);
  UB_Graphic2D_DrawPixelNormal(x,y,color);

  // Pixel berechnen
  for(t=0; t<el; ++t) // t zaehlt die Pixel, el ist auch Anzahl
  {
    // Aktualisierung Fehlerterm
    err -= es;
    if(err<0) {
      // Fehlerterm wieder positiv (>=0) machen
      err += el;
      // Schritt in langsame Richtung, Diagonalschritt
      x += ddx;
      y += ddy;
    }
    else {
      // Schritt in schnelle Richtung, Parallelschritt
      x += pdx;
      y += pdy;
    }
    UB_Graphic2D_DrawPixelNormal(x,y,color);
  }
}


//--------------------------------------------------------------
// Zeichnet eine gerade mit einer Farbe (per DMA2D)
// Start = xp,yp [x=0...LCD_MAXX, y=0...LCD_MAXY]
// Laenge = l (wird event auf Displaygrenze begrenzt)
// Richtung = d [LCD_DIR_HORIZONTAL, LCD_DIR_VERTICAL]
//--------------------------------------------------------------
void UB_Graphic2D_DrawStraightDMA(uint16_t xp, uint16_t yp, uint16_t l, LCD_DIR_t d, uint16_t c)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;

  uint32_t  Xaddress = 0;
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

  // check auf Limit
  if(xp>=LCD_MAXX) xp=LCD_MAXX-1;
  if(yp>=LCD_MAXY) yp=LCD_MAXY-1;
  if(l==0) return;

  Xaddress = LCD_CurrentFrameBuffer + 2*(LCD_MAXX*yp + xp);

  Red_Value = (0xF800 & c) >> 11;
  Blue_Value = 0x001F & c;
  Green_Value = (0x07E0 & c) >> 5;

  DMA2D_DeInit();
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
  DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
  DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
  DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;

  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    // richtung drehen
    if(d==LCD_DIR_HORIZONTAL) {
      d=LCD_DIR_VERTICAL;
    }
    else {
      d=LCD_DIR_HORIZONTAL;
    }
  }

  if(d == LCD_DIR_HORIZONTAL) {
    // check auf Limit
    if((xp+l)>LCD_MAXX) l=LCD_MAXX-xp;
    DMA2D_InitStruct.DMA2D_OutputOffset = 0;
    DMA2D_InitStruct.DMA2D_NumberOfLine = 1;
    DMA2D_InitStruct.DMA2D_PixelPerLine = l;
  }
  else {
    // check auf Limit
    if((yp+l)>LCD_MAXY) l=LCD_MAXY-yp;
    DMA2D_InitStruct.DMA2D_OutputOffset = LCD_MAXX - 1;
    DMA2D_InitStruct.DMA2D_NumberOfLine = l;
    DMA2D_InitStruct.DMA2D_PixelPerLine = 1;
  }

  DMA2D_Init(&DMA2D_InitStruct);

  DMA2D_StartTransfer();

  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}


//--------------------------------------------------------------
// Zeichnet ein Rechteck mit einer Farbe (per DMA2D)
// Ecke   = xp,yp [x=0...LCD_MAXX, y=0...LCD_MAXY]
// Breite = w
// Hoehe  = h
//--------------------------------------------------------------
void UB_Graphic2D_DrawRectDMA(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint16_t c)
{
  int16_t d;

  // check auf Limit
  if(xp>=LCD_MAXX) xp=LCD_MAXX-1;
  if(yp>=LCD_MAXY) yp=LCD_MAXY-1;
  if(w==0) return;
  if(h==0) return;

  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    // richtung drehen
    d=w;
    w=h;
    h=d;
  }

  // check auf Limit
  if((xp+w)>LCD_MAXX) w=LCD_MAXX-xp;
  if((yp+h)>LCD_MAXY) h=LCD_MAXY-yp;

  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    UB_Graphic2D_DrawStraightDMA(xp, yp, h, LCD_DIR_HORIZONTAL, c);
    UB_Graphic2D_DrawStraightDMA((xp+w-1), yp, h, LCD_DIR_HORIZONTAL, c);
    UB_Graphic2D_DrawStraightDMA(xp, yp, w, LCD_DIR_VERTICAL, c);
    UB_Graphic2D_DrawStraightDMA(xp, yp+h-1, w, LCD_DIR_VERTICAL, c);
  }
  else {
    UB_Graphic2D_DrawStraightDMA(xp, yp, w, LCD_DIR_HORIZONTAL, c);
    UB_Graphic2D_DrawStraightDMA(xp, (yp+ h-1), w, LCD_DIR_HORIZONTAL, c);
    UB_Graphic2D_DrawStraightDMA(xp, yp, h, LCD_DIR_VERTICAL, c);
    UB_Graphic2D_DrawStraightDMA((xp + w-1), yp, h, LCD_DIR_VERTICAL, c);
  }
}


//--------------------------------------------------------------
// Zeichnet ein gefuelltes Rechteck mit einer Farbe (per DMA2D)
// Ecke   = xp,yp [x=0...LCD_MAXX, y=0...LCD_MAXY]
// Breite = w
// Hoehe  = h
//--------------------------------------------------------------
void UB_Graphic2D_DrawFullRectDMA(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint16_t c)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;

  uint32_t  Xaddress = 0;
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;
  int16_t d;

  // check auf Limit
  if(xp>=LCD_MAXX) xp=LCD_MAXX-1;
  if(yp>=LCD_MAXY) yp=LCD_MAXY-1;
  if(w==0) return;
  if(h==0) return;

  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    // richtung drehen
    d=w;
    w=h;
    h=d;
  }

  // check auf Limit
  if((xp+w)>LCD_MAXX) w=LCD_MAXX-xp;
  if((yp+h)>LCD_MAXY) h=LCD_MAXY-yp;

  Red_Value = (0xF800 & c) >> 11;
  Blue_Value = 0x001F & c;
  Green_Value = (0x07E0 & c) >> 5;

  Xaddress = LCD_CurrentFrameBuffer + 2*(LCD_MAXX*yp + xp);

  DMA2D_DeInit();
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
  DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
  DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
  DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;
  DMA2D_InitStruct.DMA2D_OutputOffset = (LCD_MAXX - w);
  DMA2D_InitStruct.DMA2D_NumberOfLine = h;
  DMA2D_InitStruct.DMA2D_PixelPerLine = w;
  DMA2D_Init(&DMA2D_InitStruct);

  DMA2D_StartTransfer();

  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}


//--------------------------------------------------------------
// Zeichnet einen Kreis mit einer Farbe
// Mittelpunkt   = xp,yp [x=0...LCD_MAXX, y=0...LCD_MAXY]
// Radius = r
// (benutzt wird der Bresenham-Algorithmus)
//--------------------------------------------------------------
void UB_Graphic2D_DrawCircleNormal(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c)
{
  int16_t f=1-r, ddF_x=0, ddF_y=-2*r, x=0, y=r;

  // check auf Limit
  if(xp>=LCD_MAXX) xp=LCD_MAXX-1;
  if(yp>=LCD_MAXY) yp=LCD_MAXY-1;
  if(r==0) return;

  UB_Graphic2D_DrawPixelNormal(xp, yp + r, c);
  UB_Graphic2D_DrawPixelNormal(xp, yp - r, c);
  UB_Graphic2D_DrawPixelNormal(xp + r, yp, c);
  UB_Graphic2D_DrawPixelNormal(xp - r, yp, c);

  while(x < y) {
    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;

    UB_Graphic2D_DrawPixelNormal(xp + x, yp + y, c);
    UB_Graphic2D_DrawPixelNormal(xp - x, yp + y, c);
    UB_Graphic2D_DrawPixelNormal(xp + x, yp - y, c);
    UB_Graphic2D_DrawPixelNormal(xp - x, yp - y, c);
    UB_Graphic2D_DrawPixelNormal(xp + y, yp + x, c);
    UB_Graphic2D_DrawPixelNormal(xp - y, yp + x, c);
    UB_Graphic2D_DrawPixelNormal(xp + y, yp - x, c);
    UB_Graphic2D_DrawPixelNormal(xp - y, yp - x, c);
  }
}


//--------------------------------------------------------------
// Zeichnet einen gefuellten Kreis mit einer Farbe (per DMA2D)
// Mittelpunkt   = xp,yp [x=0...LCD_MAXX, y=0...LCD_MAXY]
// Radius = r
//--------------------------------------------------------------
void UB_Graphic2D_DrawFullCircleDMA(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c)
{
  int32_t  D;    
  uint32_t  CurX;
  uint32_t  CurY; 
  int16_t x,y,l;
  LCD_DIR_t m=LCD_DIR_VERTICAL;
  
  // check auf Limit
  if(xp>=LCD_MAXX) xp=LCD_MAXX-1;
  if(yp>=LCD_MAXY) yp=LCD_MAXY-1;
  if(r==0) return;

  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    // richtung drehen
    m=LCD_DIR_HORIZONTAL;
  }

  D = 3 - (r << 1);
  
  CurX = 0;
  CurY = r;
  
  while (CurX <= CurY) {
    if(CurY > 0) {
      x=xp - CurX;
      y=yp - CurY;
      l=2*CurY;
      if(y<0) {
       	l+=y;
       	y=0;
      }
      UB_Graphic2D_DrawStraightDMA(x, y, l, m, c);
      x=xp + CurX;
      y=yp - CurY;
      l=2*CurY;
      if(y<0) {
       	l+=y;
       	y=0;
      }
      UB_Graphic2D_DrawStraightDMA(x,y ,l , m, c);
    }
    
    if(CurX > 0) {
      x=xp - CurY;
      y=yp - CurX;
      l=2*CurX;
      if(y<0) {
        l+=y;
        y=0;
      }
      UB_Graphic2D_DrawStraightDMA(x,y , l, m, c);
      x=xp + CurY;
      y=yp - CurX;
      l=2*CurX;
      if(y<0) {
        l+=y;
        y=0;
      }
      UB_Graphic2D_DrawStraightDMA(x,y , l, m, c);
    }

    if (D < 0) { 
      D += (CurX << 2) + 6;
    }
    else {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
  
  UB_Graphic2D_DrawCircleNormal(xp, yp, r, c);
}


//--------------------------------------------------------------
// zeichnet einen Teilbereich
// von einem Image (aus dem Flash)
// auf das LCD
//--------------------------------------------------------------
void UB_Graphic2D_DrawImageRect(Image2LCD_t koord)
{

	uint32_t x,y;
	uint16_t c;
	uint32_t  Yaddress = 0;
	uint32_t xp,yp;
	uint32_t h,w;
	uint32_t xs,ys,ws;

	xp=koord.dest_xp;
	yp=koord.dest_yp;
	h=koord.h;
	w=koord.w;

	xs=koord.source_xp;
	ys=koord.source_yp;
	ws=Skin1.width;

	for(y=0;y<h;y++) {
		UB_LCD_SetCursor2Draw(xp,yp+y);
		for(x=0;x<w;x++) {
			Yaddress = ((ys+y)*ws)+xs+x;
			c=Skin1.table[Yaddress];
			UB_LCD_DrawPixel(c);
		}
	}

}


//--------------------------------------------------------------
// zeichnet einen Teilbereich
// von einem Image (aus dem Flash)
// auf das LCD (drehbar in 90Grad schritten)
//--------------------------------------------------------------
void UB_Graphic2D_DrawImageRectRotate(Image2LCD_t koord, uint32_t m)
{

	uint32_t x,y;
	uint16_t c;
	uint32_t  Yaddress = 0;
	uint32_t xp,yp;
	uint32_t h,w;
	uint32_t xs,ys,ws;

	xp=koord.dest_xp;
	yp=koord.dest_yp;
	h=koord.h;
	w=koord.w;

	xs=koord.source_xp;
	ys=koord.source_yp;
	ws=Skin1.width;

	if(m==0) {
		UB_Graphic2D_DrawImageRect(koord);
	}
	else if(m==1) {
		for(y=0;y<h;y++) {
			UB_LCD_SetCursor2Draw(xp,yp+y);
			for(x=0;x<w;x++) {
				Yaddress = ((ys+y)*ws)+xs+(w-x);
				c=Skin1.table[Yaddress];
				UB_LCD_DrawPixel(c);
			}
		}
	}
	else if(m==2) {
		for(y=0;y<h;y++) {
			UB_LCD_SetCursor2Draw(xp,yp+y);
			for(x=0;x<w;x++) {
				Yaddress = ((ys+x)*ws)+xs+y;
				c=Skin1.table[Yaddress];
				UB_LCD_DrawPixel(c);
			}
		}
	}
	else {
		for(y=0;y<h;y++) {
			UB_LCD_SetCursor2Draw(xp,yp+y);
			for(x=0;x<w;x++) {
				Yaddress = ((ys+x)*ws)+xs+(w-y);
				c=Skin1.table[Yaddress];
				UB_LCD_DrawPixel(c);
			}
		}
	}
}

//--------------------------------------------------------------
// interne Funktion
// Signum funktion
//  Return_Wert
//    1 ,wenn x > 0
//    0 ,wenn x = 0
//   -1 ,wenn x < 0
//--------------------------------------------------------------
int16_t P_Graphic2D_sgn(int16_t x)
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}




