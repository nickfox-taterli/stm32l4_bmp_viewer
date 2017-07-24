#ifndef LCD_ILI9341_DEF
#define LCD_ILI9341_DEF

#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_gpio.h"

#include "stm32l4xx.h"
#include "fonts.h"

#define ILI9341_TFTWIDTH   240
#define ILI9341_TFTHEIGHT  320

#define ILI9341_NOP        0x00
#define ILI9341_SWRESET    0x01
#define ILI9341_RDDID      0x04
#define ILI9341_RDDST      0x09

#define ILI9341_SLPIN      0x10
#define ILI9341_SLPOUT     0x11
#define ILI9341_PTLON      0x12
#define ILI9341_NORON      0x13

#define ILI9341_RDMODE     0x0A
#define ILI9341_RDMADCTL   0x0B
#define ILI9341_RDPIXFMT   0x0C
#define ILI9341_RDIMGFMT   0x0D
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF     0x20
#define ILI9341_INVON      0x21
#define ILI9341_GAMMASET   0x26
#define ILI9341_DISPOFF    0x28
#define ILI9341_DISPON     0x29

#define ILI9341_CASET      0x2A
#define ILI9341_PASET      0x2B
#define ILI9341_RAMWR      0x2C
#define ILI9341_RAMRD      0x2E

#define ILI9341_PTLAR      0x30
#define ILI9341_MADCTL     0x36
#define ILI9341_VSCRSADD   0x37
#define ILI9341_PIXFMT     0x3A

#define ILI9341_FRMCTR1    0xB1
#define ILI9341_FRMCTR2    0xB2
#define ILI9341_FRMCTR3    0xB3
#define ILI9341_INVCTR     0xB4
#define ILI9341_DFUNCTR    0xB6

#define ILI9341_PWCTR1     0xC0
#define ILI9341_PWCTR2     0xC1
#define ILI9341_PWCTR3     0xC2
#define ILI9341_PWCTR4     0xC3
#define ILI9341_PWCTR5     0xC4
#define ILI9341_VMCTR1     0xC5
#define ILI9341_VMCTR2     0xC7

#define ILI9341_RDID1      0xDA
#define ILI9341_RDID2      0xDB
#define ILI9341_RDID3      0xDC
#define ILI9341_RDID4      0xDD

#define ILI9341_GMCTRP1    0xE0
#define ILI9341_GMCTRN1    0xE1

#define ILI9341_PWCTR6     0xFC

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

#define ILI9341_Y_POS				320
#define ILI9341_X_POS				240

#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_LIGHTBLUE     0x841F
#define LCD_COLOR_LIGHTGREEN    0x87F0
#define LCD_COLOR_LIGHTRED      0xFC10
#define LCD_COLOR_LIGHTCYAN     0x87FF
#define LCD_COLOR_LIGHTMAGENTA  0xFC1F
#define LCD_COLOR_LIGHTYELLOW   0xFFF0
#define LCD_COLOR_DARKBLUE      0x0010
#define LCD_COLOR_DARKGREEN     0x0400
#define LCD_COLOR_DARKRED       0x8000
#define LCD_COLOR_DARKCYAN      0x0410
#define LCD_COLOR_DARKMAGENTA   0x8010
#define LCD_COLOR_DARKYELLOW    0x8400
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_LIGHTGRAY     0xD69A
#define LCD_COLOR_GRAY          0x8410
#define LCD_COLOR_DARKGRAY      0x4208
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_BROWN         0xA145
#define LCD_COLOR_ORANGE        0xFD20

#define LCD_DEFAULT_FONT         Font8

#define ILI9341_CS_ENABLE() 	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_5)
#define ILI9341_CS_DISABLE() 	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_5);

#define ABS(X)  ((X) > 0 ? (X) : -(X))

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y))

typedef struct
{
    uint32_t TextColor;
    uint32_t BackColor;
    sFONT    *pFont;
} LCD_DrawPropTypeDef;

typedef struct
{
    int16_t X;
    int16_t Y;
} Point, * pPoint;

typedef enum
{
    CENTER_MODE          = 0x01,    /* Center mode */
    RIGHT_MODE           = 0x02,    /* Right mode  */
    LEFT_MODE            = 0x03     /* Left mode   */
} Line_ModeTypdef;

extern LCD_DrawPropTypeDef DrawProp;

void ILI9341_Send(uint8_t data);
void ILI9341_SendCommand(uint8_t cmd);

void ILI9341_Init(void);
void ILI9341_Reset(void);

void ILI9341_Clear(uint16_t Color);
void ILI9341_ClearStringLine(uint16_t Line);

void ILI9341_DisplayStringAtLine(uint16_t Line, uint8_t *ptr);
void ILI9341_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Line_ModeTypdef Mode);
void ILI9341_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color);

void ILI9341_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void ILI9341_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void ILI9341_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void ILI9341_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void ILI9341_DrawPolygon(pPoint Points, uint16_t PointCount);
void ILI9341_DrawEllipse(int16_t Xpos, int16_t Ypos, int16_t XRadius, int16_t YRadius);

void ILI9341_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void ILI9341_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void ILI9341_FillPolygon(pPoint Points, uint16_t PointCount);
void ILI9341_FillEllipse(int16_t Xpos, int16_t Ypos, int16_t XRadius, int16_t YRadius);

void ILI9341_DisplayOff(void);
void ILI9341_DisplayOn(void);

void ILI9341_SetCursor(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void ILI9341_LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pBmp);

#endif

