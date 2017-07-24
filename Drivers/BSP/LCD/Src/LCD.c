#include "LCD.h"

LCD_DrawPropTypeDef DrawProp;

/*

PA7 - MOSI
PB3 - SCK
PC5 - CS
PB1 - DC
PB13 - RST
PB0 - TCS

MISO UNCONNECT
TP_INT UNCONNECT

DC = LOW COMMAND
DC = HIGH DATA

*/

static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);

static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
    uint32_t i = 0, j = 0;
    uint16_t height, width;
    uint8_t offset;
    uint8_t *pchar;
    uint32_t line;

    height = DrawProp.pFont->Height;
    width  = DrawProp.pFont->Width;

    offset =  8 * ((width + 7) / 8) -  width ;

    for(i = 0; i < height; i++)
    {
        pchar = ((uint8_t *)c + (width + 7) / 8 * i);

        switch(((width + 7) / 8))
        {
        case 1:
            line =  pchar[0];
            break;

        case 2:
            line =  (pchar[0] << 8) | pchar[1];
            break;

        case 3:
        default:
            line =  (pchar[0] << 16) | (pchar[1] << 8) | pchar[2];
            break;
        }

        for (j = 0; j < width; j++)
        {
            if(line & (1 << (width - j + offset - 1)))
            {
                ILI9341_WritePixel((Xpos + j), Ypos, DrawProp.TextColor);
            }
            else
            {
                ILI9341_WritePixel((Xpos + j), Ypos, DrawProp.BackColor);
            }
        }
        Ypos++;
    }
}

static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);        /* The difference between the x's */
    deltay = ABS(y2 - y1);        /* The difference between the y's */
    x = x1;                       /* Start x off at the first pixel */
    y = y1;                       /* Start y off at the first pixel */

    if (x2 >= x1)                 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else                          /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)                 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else                          /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)         /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
        yinc2 = 0;                  /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         /* There are more x-values than y-values */
    }
    else                          /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;                  /* Don't change the x for every iteration */
        yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         /* There are more y-values than x-values */
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        ILI9341_DrawLine(x, y, x3, y3);

        num += numadd;              /* Increase the numerator by the top of the fraction */
        if (num >= den)             /* Check if numerator >= denominator */
        {
            num -= den;               /* Calculate the new numerator value */
            x += xinc1;               /* Change the x as appropriate */
            y += yinc1;               /* Change the y as appropriate */
        }
        x += xinc2;                 /* Change the x as appropriate */
        y += yinc2;                 /* Change the y as appropriate */
    }
}


void ILI9341_Send(uint8_t data)
{
    while(LL_SPI_IsActiveFlag_BSY(SPI2));
    LL_SPI_TransmitData8(SPI2, data);
    while(LL_SPI_IsActiveFlag_BSY(SPI2));
}

void ILI9341_SendCommand(uint8_t cmd)
{
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
    ILI9341_Send(cmd);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
}

void ILI9341_Init(void)
{
    LL_SPI_InitTypeDef SPI_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(RCC_APB1ENR1_SPI2EN);
    /* GPIO Ports Clock Enable */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_5);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_12);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);	

    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_12;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /**SPI2 GPIO Configuration
    PA7   ------> SPI2_MOSI
    PB3 (JTDO-TRACESWO)   ------> SPI2_SCK
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;//256
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI2, &SPI_InitStruct);

    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);

    //LL_SPI_EnableNSSPulseMgt(SPI2);

    LL_SPI_Enable(SPI2);

    ILI9341_Reset();

    ILI9341_SendCommand(0xEF);
    ILI9341_Send(0x03);
    ILI9341_Send(0x80);
    ILI9341_Send(0x02);

    ILI9341_SendCommand(0xCF);
    ILI9341_Send(0x00);
    ILI9341_Send(0XC1);
    ILI9341_Send(0X30);

    ILI9341_SendCommand(0xED);
    ILI9341_Send(0x64);
    ILI9341_Send(0x03);
    ILI9341_Send(0X12);
    ILI9341_Send(0X81);

    ILI9341_SendCommand(0xE8);
    ILI9341_Send(0x85);
    ILI9341_Send(0x00);
    ILI9341_Send(0x78);

    ILI9341_SendCommand(0xCB);
    ILI9341_Send(0x39);
    ILI9341_Send(0x2C);
    ILI9341_Send(0x00);
    ILI9341_Send(0x34);
    ILI9341_Send(0x02);

    ILI9341_SendCommand(0xF7);
    ILI9341_Send(0x20);

    ILI9341_SendCommand(0xEA);
    ILI9341_Send(0x00);
    ILI9341_Send(0x00);

    ILI9341_SendCommand(ILI9341_PWCTR1);    //Power control
    ILI9341_Send(0x23);   //VRH[5:0]

    ILI9341_SendCommand(ILI9341_PWCTR2);    //Power control
    ILI9341_Send(0x10);   //SAP[2:0];BT[3:0]

    ILI9341_SendCommand(ILI9341_VMCTR1);    //VCM control
    ILI9341_Send(0x3e);
    ILI9341_Send(0x28);

    ILI9341_SendCommand(ILI9341_VMCTR2);    //VCM control2
    ILI9341_Send(0x86);  //--

    ILI9341_SendCommand(ILI9341_MADCTL);    // Memory Access Control
    ILI9341_Send(0x48);

    ILI9341_SendCommand(ILI9341_VSCRSADD); // Vertical scroll
    ILI9341_Send(0x00);
    ILI9341_Send(0x00);		// Zero

    ILI9341_SendCommand(ILI9341_PIXFMT);
    ILI9341_Send(0x55);

    ILI9341_SendCommand(ILI9341_FRMCTR1);
    ILI9341_Send(0x00);
    ILI9341_Send(0x18);

    ILI9341_SendCommand(ILI9341_DFUNCTR);    // Display Function Control
    ILI9341_Send(0x08);
    ILI9341_Send(0x82);
    ILI9341_Send(0x27);

    ILI9341_SendCommand(0xF2);    // 3Gamma Function Disable
    ILI9341_Send(0x00);

    ILI9341_SendCommand(ILI9341_GAMMASET);    //Gamma curve selected
    ILI9341_Send(0x01);

    ILI9341_SendCommand(ILI9341_GMCTRP1);    //Set Gamma
    ILI9341_Send(0x0F);
    ILI9341_Send(0x31);
    ILI9341_Send(0x2B);
    ILI9341_Send(0x0C);
    ILI9341_Send(0x0E);
    ILI9341_Send(0x08);
    ILI9341_Send(0x4E);
    ILI9341_Send(0xF1);
    ILI9341_Send(0x37);
    ILI9341_Send(0x07);
    ILI9341_Send(0x10);
    ILI9341_Send(0x03);
    ILI9341_Send(0x0E);
    ILI9341_Send(0x09);
    ILI9341_Send(0x00);

    ILI9341_SendCommand(ILI9341_GMCTRN1);    //Set Gamma
    ILI9341_Send(0x00);
    ILI9341_Send(0x0E);
    ILI9341_Send(0x14);
    ILI9341_Send(0x03);
    ILI9341_Send(0x11);
    ILI9341_Send(0x07);
    ILI9341_Send(0x31);
    ILI9341_Send(0xC1);
    ILI9341_Send(0x48);
    ILI9341_Send(0x08);
    ILI9341_Send(0x0F);
    ILI9341_Send(0x0C);
    ILI9341_Send(0x31);
    ILI9341_Send(0x36);
    ILI9341_Send(0x0F);

    ILI9341_DisplayOn();

    DrawProp.pFont = &LCD_DEFAULT_FONT;
    DrawProp.TextColor = 0x0000;
    DrawProp.BackColor = 0xFFFF;

}


void ILI9341_DisplayOn(void)
{
    ILI9341_SendCommand(ILI9341_SLPOUT);    //Exit Sleep
    LL_mDelay(120);
    ILI9341_SendCommand(ILI9341_DISPON);    //Display on
    LL_mDelay(120);

}

void ILI9341_DisplayOff(void)
{
    ILI9341_SendCommand(ILI9341_DISPOFF);    //Display on
    LL_mDelay(120);
    ILI9341_SendCommand(ILI9341_SLPIN);    //Exit Sleep
    LL_mDelay(120);
}

void ILI9341_Reset(void)
{
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
    LL_mDelay(100);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
    LL_mDelay(100);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
    LL_mDelay(100);
}

void ILI9341_SetCursor(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ILI9341_SendCommand(ILI9341_CASET);

    ILI9341_Send(x >> 8);

    ILI9341_Send(x - ((x >> 8) << 8));

    ILI9341_Send((x + w - 1) >> 8);

    ILI9341_Send((x + w - 1) - (((x + w - 1) >> 8) << 8));

    ILI9341_SendCommand(ILI9341_PASET);

    ILI9341_Send(y >> 8);

    ILI9341_Send(y - ((y >> 8) << 8));

    ILI9341_Send((y + h - 1) >> 8);

    ILI9341_Send((y + h - 1) - (((y + h - 1) >> 8) << 8));

    ILI9341_SendCommand(ILI9341_RAMWR);

}

void ILI9341_Clear(uint16_t Color)
{
    uint32_t counter = 0;

    ILI9341_SetCursor(0, 0, ILI9341_X_POS, ILI9341_Y_POS);

    for(counter = 0; counter < ILI9341_X_POS * ILI9341_Y_POS; counter++)
    {
        ILI9341_Send(Color >> 8);
        ILI9341_Send(Color);
    }

}

void ILI9341_ClearStringLine(uint16_t Line)
{
    uint32_t color_backup = DrawProp.TextColor;

    DrawProp.TextColor = DrawProp.BackColor;

    /* Draw a rectangle with background color */
    ILI9341_FillRect(0, (Line * DrawProp.pFont->Height), ILI9341_X_POS, DrawProp.pFont->Height);

    DrawProp.TextColor = color_backup;
}

void ILI9341_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    do
    {
        ILI9341_DrawHLine(Xpos, Ypos++, Width);
    }
    while(Height--);
}

void ILI9341_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    uint32_t counter = 0;

    for(counter = 0; counter < Length; counter++)
    {
        ILI9341_WritePixel((Xpos + counter), Ypos, DrawProp.TextColor);
    }
}

void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
    ILI9341_SetCursor(x, y, 1, 1);	
    ILI9341_Send(color >> 8);
    ILI9341_Send(color);	
}

void ILI9341_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp.pFont->table[(Ascii - ' ') *\
             DrawProp.pFont->Height * ((DrawProp.pFont->Width + 7) / 8)]);
}

void ILI9341_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Line_ModeTypdef Mode)
{
    uint16_t refcolumn = 1, i = 0;
    uint32_t size = 0, xsize = 0;
    uint8_t  *ptr = Text;

    /* Get the text size */
    while (*ptr++) size ++ ;

    /* Characters number per line */
    xsize = (ILI9341_X_POS / DrawProp.pFont->Width);

    switch (Mode)
    {
    case CENTER_MODE:
    {
        refcolumn = Xpos + ((xsize - size) * DrawProp.pFont->Width) / 2;
        break;
    }
    case LEFT_MODE:
    {
        refcolumn = Xpos;
        break;
    }
    case RIGHT_MODE:
    {
        refcolumn =  - Xpos + ((xsize - size) * DrawProp.pFont->Width);
        break;
    }
    default:
    {
        refcolumn = Xpos;
        break;
    }
    }

    /* Check that the Start column is located in the screen */
    if ((refcolumn < 1) || (refcolumn >= 0x8000))
    {
        refcolumn = 1;
    }

    /* Send the string character by character on lCD */
    while ((*Text != 0) & (((ILI9341_X_POS - (i * DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width))
    {
        /* Display one character on LCD */
        ILI9341_DisplayChar(refcolumn, Ypos, *Text);
        /* Decrement the column position by 16 */
        refcolumn += DrawProp.pFont->Width;
        /* Point on the next character */
        Text++;
        i++;
    }
}

void ILI9341_DisplayStringAtLine(uint16_t Line, uint8_t *ptr)
{
    ILI9341_DisplayStringAt(0, (Line) * (uint16_t)((DrawProp.pFont)->Height) , ptr, LEFT_MODE);
}

void ILI9341_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{

    uint32_t counter = 0;

    for(counter = 0; counter < Length; counter++)
    {
        ILI9341_WritePixel(Xpos, Ypos + counter, DrawProp.TextColor);
    }
}

void ILI9341_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);        /* The difference between the x's */
    deltay = ABS(y2 - y1);        /* The difference between the y's */
    x = x1;                       /* Start x off at the first pixel */
    y = y1;                       /* Start y off at the first pixel */

    if (x2 >= x1)                 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else                          /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)                 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else                          /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)         /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
        yinc2 = 0;                  /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         /* There are more x-values than y-values */
    }
    else                          /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;                  /* Don't change the x for every iteration */
        yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         /* There are more y-values than x-values */
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        ILI9341_WritePixel(x, y, DrawProp.TextColor);  /* Draw the current pixel */
        num += numadd;                            /* Increase the numerator by the top of the fraction */
        if (num >= den)                           /* Check if numerator >= denominator */
        {
            num -= den;                             /* Calculate the new numerator value */
            x += xinc1;                             /* Change the x as appropriate */
            y += yinc1;                             /* Change the y as appropriate */
        }
        x += xinc2;                               /* Change the x as appropriate */
        y += yinc2;                               /* Change the y as appropriate */
    }
}

void ILI9341_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    /* Draw horizontal lines */
    ILI9341_DrawHLine(Xpos, Ypos, Width);
    ILI9341_DrawHLine(Xpos, (Ypos + Height), Width);

    /* Draw vertical lines */
    ILI9341_DrawVLine(Xpos, Ypos, Height);
    ILI9341_DrawVLine((Xpos + Width), Ypos, Height);
}

void ILI9341_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t  decision;       /* Decision Variable */
    uint32_t  current_x;   /* Current X Value */
    uint32_t  current_y;   /* Current Y Value */

    decision = 3 - (Radius << 1);
    current_x = 0;
    current_y = Radius;

    while (current_x <= current_y)
    {
        ILI9341_WritePixel((Xpos + current_x), (Ypos - current_y), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos - current_x), (Ypos - current_y), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos + current_y), (Ypos - current_x), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos - current_y), (Ypos - current_x), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos + current_x), (Ypos + current_y), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos - current_x), (Ypos + current_y), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos + current_y), (Ypos + current_x), DrawProp.TextColor);

        ILI9341_WritePixel((Xpos - current_y), (Ypos + current_x), DrawProp.TextColor);

        /* Initialize the font */
        DrawProp.pFont = &LCD_DEFAULT_FONT;

        if (decision < 0)
        {
            decision += (current_x << 2) + 6;
        }
        else
        {
            decision += ((current_x - current_y) << 2) + 10;
            current_y--;
        }
        current_x++;
    }
}

void ILI9341_DrawPolygon(pPoint Points, uint16_t PointCount)
{
    int16_t x = 0, y = 0;

    if(PointCount < 2)
    {
        return;
    }

    ILI9341_DrawLine(Points->X, Points->Y, (Points + PointCount - 1)->X, (Points + PointCount - 1)->Y);

    while(--PointCount)
    {
        x = Points->X;
        y = Points->Y;
        Points++;
        ILI9341_DrawLine(x, y, Points->X, Points->Y);
    }
}

void ILI9341_DrawEllipse(int16_t Xpos, int16_t Ypos, int16_t XRadius, int16_t YRadius)
{
    int16_t x = 0, y = -YRadius, err = 2 - 2 * XRadius, e2;
    float k = 0, rad1 = 0, rad2 = 0;

    rad1 = XRadius;
    rad2 = YRadius;

    k = (float)(rad2 / rad1);

    do
    {
        ILI9341_WritePixel((Xpos - (uint16_t)(x / k)), (Ypos + y), DrawProp.TextColor);
        ILI9341_WritePixel((Xpos + (uint16_t)(x / k)), (Ypos + y), DrawProp.TextColor);
        ILI9341_WritePixel((Xpos + (uint16_t)(x / k)), (Ypos - y), DrawProp.TextColor);
        ILI9341_WritePixel((Xpos - (uint16_t)(x / k)), (Ypos - y), DrawProp.TextColor);

        e2 = err;
        if (e2 <= x)
        {
            err += ++x * 2 + 1;
            if (-y == x && e2 <= y) e2 = 0;
        }
        if (e2 > y) err += ++y * 2 + 1;
    }
    while (y <= 0);
}


void ILI9341_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t  decision;        /* Decision Variable */
    uint32_t  current_x;    /* Current X Value */
    uint32_t  current_y;    /* Current Y Value */

    decision = 3 - (Radius << 1);

    current_x = 0;
    current_y = Radius;

    while (current_x <= current_y)
    {
        if(current_y > 0)
        {
            ILI9341_DrawHLine(Xpos - current_y, Ypos + current_x, 2 * current_y);
            ILI9341_DrawHLine(Xpos - current_y, Ypos - current_x, 2 * current_y);
        }

        if(current_x > 0)
        {
            ILI9341_DrawHLine(Xpos - current_x, Ypos - current_y, 2 * current_x);
            ILI9341_DrawHLine(Xpos - current_x, Ypos + current_y, 2 * current_x);
        }
        if (decision < 0)
        {
            decision += (current_x << 2) + 6;
        }
        else
        {
            decision += ((current_x - current_y) << 2) + 10;
            current_y--;
        }
        current_x++;
    }

    ILI9341_DrawCircle(Xpos, Ypos, Radius);
}

void ILI9341_FillPolygon(pPoint Points, uint16_t PointCount)
{
    int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
    uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

    IMAGE_LEFT = IMAGE_RIGHT = Points->X;
    IMAGE_TOP = IMAGE_BOTTOM = Points->Y;

    for(counter = 1; counter < PointCount; counter++)
    {
        pixelX = POLY_X(counter);
        if(pixelX < IMAGE_LEFT)
        {
            IMAGE_LEFT = pixelX;
        }
        if(pixelX > IMAGE_RIGHT)
        {
            IMAGE_RIGHT = pixelX;
        }

        pixelY = POLY_Y(counter);
        if(pixelY < IMAGE_TOP)
        {
            IMAGE_TOP = pixelY;
        }
        if(pixelY > IMAGE_BOTTOM)
        {
            IMAGE_BOTTOM = pixelY;
        }
    }

    if(PointCount < 2)
    {
        return;
    }

    X_center = (IMAGE_LEFT + IMAGE_RIGHT) / 2;
    Y_center = (IMAGE_BOTTOM + IMAGE_TOP) / 2;

    X_first = Points->X;
    Y_first = Points->Y;

    while(--PointCount)
    {
        X = Points->X;
        Y = Points->Y;
        Points++;
        X2 = Points->X;
        Y2 = Points->Y;

        FillTriangle(X, X2, X_center, Y, Y2, Y_center);
        FillTriangle(X, X_center, X2, Y, Y_center, Y2);
        FillTriangle(X_center, X2, X, Y_center, Y2, Y);
    }

    FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
    FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
    FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
}


void ILI9341_FillEllipse(int16_t Xpos, int16_t Ypos, int16_t XRadius, int16_t YRadius)
{
    int16_t x = 0, y = -YRadius, err = 2 - 2 * XRadius, e2;
    float k = 0, rad1 = 0, rad2 = 0;

    rad1 = XRadius;
    rad2 = YRadius;

    k = (float)(rad2 / rad1);

    do
    {
        ILI9341_DrawHLine((Xpos - (uint16_t)(x / k)), (Ypos + y), (2 * (uint16_t)(x / k) + 1));
        ILI9341_DrawHLine((Xpos - (uint16_t)(x / k)), (Ypos - y), (2 * (uint16_t)(x / k) + 1));

        e2 = err;
        if (e2 <= x)
        {
            err += ++x * 2 + 1;
            if (-y == x && e2 <= y) e2 = 0;
        }
        if (e2 > y) err += ++y * 2 + 1;
    }
    while (y <= 0);
}

/**
  * @brief  Draws a bitmap picture loaded in the STM32 MCU internal memory.
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD
  * @param  pBmp: Pointer to Bmp picture address
  * @retval None
  */
void ILI9341_LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pBmp)
{
  uint32_t height = 0, width  = 0;
	uint32_t index = 0, size = 0;
	uint8_t bpp = 24;
	
  /* Read bitmap depth */
  bpp = *(uint8_t *) (pBmp + 28);
	
  /* Read bitmap width */
  width = *(uint16_t *) (pBmp + 18);
  width |= (*(uint16_t *) (pBmp + 20)) << 16;
  
  /* Read bitmap height */
  height = *(uint16_t *) (pBmp + 22);
  height |= (*(uint16_t *) (pBmp + 24)) << 16; 
  
  ILI9341_SetCursor(Xpos, Ypos, width, height);
  
  /* Read bitmap size */
  size = *(volatile uint16_t *) (pBmp + 2);
  size |= (*(volatile uint16_t *) (pBmp + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(volatile uint16_t *) (pBmp + 10);
  index |= (*(volatile uint16_t *) (pBmp + 12)) << 16;
  size = (size - index)/(bpp/8);
  pBmp += index;
	
    for (index = size; index != 0; index--)
    {
			if(bpp == 16){
				ILI9341_Send(*(pBmp+1));
				ILI9341_Send(*pBmp);
				pBmp += 2;
			}else if(bpp == 24){
				
				ILI9341_Send(((((uint16_t)((*(pBmp+2)) >> 3) << 11) | ((uint16_t)((*(pBmp+1)) >> 2) << 5)) >> 8) & 0xFF);
				ILI9341_Send(((uint16_t)((*(pBmp+1)) >> 2) << 5) | ((uint16_t)(*(pBmp)) >> 3));

				pBmp += 3;
			}
    }	
}
