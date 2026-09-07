/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

#include "A_config.h"

#ifdef DUAL_OLED_ENABLED

#include <Wire.h>
#include "src/esp8266-oled-ssd1306-4.1.0/SSD1306Wire.h"

/**
 * Gestore per due display OLED 0.96" I2C unificati
 * Crea una superficie virtuale di 256x64 pixel su due display fisici di 128x64
 * 
 * Layout:
 * [OLED 0x3C - 128x64] | [OLED 0x3D - 128x64]
 * 
 * Coordinate virtuali: (0-255, 0-63)
 * Mapping automatico ai display fisici
 */
class DualOLED {
public:
    DualOLED(uint8_t sda = I2C_SDA, uint8_t scl = I2C_SCL,
             uint8_t addr1 = I2C_ADDR, uint8_t addr2 = I2C_ADDR_SECONDARY)
        : display1(addr1, sda, scl),
          display2(addr2, sda, scl),
          sda_pin(sda),
          scl_pin(scl) {}

    /**
     * Inizializza i due display OLED
     */
    void init() {
        display1.init();
        display2.init();
        
        // Configurazione comune
        display1.setFont(DejaVu_Sans_Mono_12);
        display2.setFont(DejaVu_Sans_Mono_12);
        
        display1.setContrast(255);
        display2.setContrast(255);
        
        display1.flipScreenVertically();
        display2.flipScreenVertically();
        
        display1.clear();
        display2.clear();
        display1.display();
        display2.display();
    }

    /**
     * Accende entrambi i display
     */
    void displayOn() {
        display1.displayOn();
        display2.displayOn();
    }

    /**
     * Spegne entrambi i display
     */
    void displayOff() {
        display1.displayOff();
        display2.displayOff();
    }

    /**
     * Pulisce entrambi i display
     */
    void clear() {
        display1.clear();
        display2.clear();
    }

    /**
     * Mostra il contenuto su entrambi i display
     */
    void display() {
        display1.display();
        display2.display();
    }

    /**
     * Imposta il font per entrambi i display
     */
    void setFont(const uint8_t* font) {
        display1.setFont(font);
        display2.setFont(font);
    }

    /**
     * Disegna una stringa sulla superficie virtuale unificata
     * x: 0-255 (0-127 su OLED sinistra, 128-255 su OLED destra)
     * y: 0-63
     */
    void drawString(int x, int y, const String& str) {
        if (y < 0 || y >= DUAL_OLED_HEIGHT) return;

        if (x < 0 || x >= DUAL_OLED_TOTAL_WIDTH) return;

        // OLED sinistra (0x3C): x da 0 a 127
        if (x < DUAL_OLED_WIDTH) {
            display1.drawString(x, y, str);
        }
        // OLED destra (0x3D): x da 128 a 255 (mappato a 0-127)
        else if (x >= DUAL_OLED_WIDTH) {
            int mapped_x = x - DUAL_OLED_WIDTH;
            display2.drawString(mapped_x, y, str);
        }
    }

    /**
     * Disegna una riga sulla superficie virtuale
     */
    void drawLine(int x1, int y1, int x2, int y2) {
        // Linea completamente a sinistra
        if (x2 < DUAL_OLED_WIDTH) {
            display1.drawLine(x1, y1, x2, y2);
        }
        // Linea completamente a destra
        else if (x1 >= DUAL_OLED_WIDTH) {
            int mapped_x1 = x1 - DUAL_OLED_WIDTH;
            int mapped_x2 = x2 - DUAL_OLED_WIDTH;
            display2.drawLine(mapped_x1, y1, mapped_x2, y2);
        }
        // Linea che attraversa entrambi (caso raro)
        else {
            // Disegna parte sinistra
            display1.drawLine(x1, y1, DUAL_OLED_WIDTH - 1, y2);
            // Disegna parte destra (mappata)
            display2.drawLine(0, y1, (x2 - DUAL_OLED_WIDTH), y2);
        }
    }

    /**
     * Disegna un pixel sulla superficie virtuale
     */
    void setPixel(int x, int y) {
        if (y < 0 || y >= DUAL_OLED_HEIGHT) return;
        if (x < 0 || x >= DUAL_OLED_TOTAL_WIDTH) return;

        if (x < DUAL_OLED_WIDTH) {
            display1.setPixel(x, y);
        } else {
            display2.setPixel(x - DUAL_OLED_WIDTH, y);
        }
    }

    /**
     * Imposta l'allineamento del testo per entrambi
     */
    void setTextAlignment(OLEDDISPLAY_TEXT_ALIGNMENT alignment) {
        display1.setTextAlignment(alignment);
        display2.setTextAlignment(alignment);
    }

    /**
     * Restituisce il display sinistro (address 0x3C)
     */
    SSD1306Wire& getDisplay1() {
        return display1;
    }

    /**
     * Restituisce il display destro (address 0x3D)
     */
    SSD1306Wire& getDisplay2() {
        return display2;
    }

private:
    SSD1306Wire display1;
    SSD1306Wire display2;
    uint8_t sda_pin;
    uint8_t scl_pin;
};

#endif // DUAL_OLED_ENABLED
