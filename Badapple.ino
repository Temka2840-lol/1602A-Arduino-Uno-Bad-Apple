#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

// Инициализация пинов для параллельного подключения: (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(8, 9, 2, 3, 4, 5);

const int chipSelect = 7;
File dataFile;

uint8_t frameBuffer[256];

void setup() {
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.noBlink();
  lcd.clear();

  if (!SD.begin(chipSelect)) {
    lcd.setCursor(0, 0);
    lcd.print("SD Init Fail!");
    while (1);
  }

  dataFile = SD.open("BADAPPLE.BIN");
  if (!dataFile) {
    lcd.setCursor(0, 0);
    lcd.print("File Not Found!");
    while (1);
  }

  // Создаем 8 текстурных (дизеринг) паттернов для четких силуэтов вместо полос
  uint8_t levels[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 0: полностью белый
    { 0x10, 0x00, 0x02, 0x00, 0x10, 0x00, 0x02, 0x00 }, // 1: редкие точки
    { 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00 }, // 2: легкая шахматка
    { 0x0A, 0x04, 0x0A, 0x04, 0x0A, 0x04, 0x0A, 0x04 }, // 3: средняя плотность
    { 0x1B, 0x0A, 0x1B, 0x0A, 0x1B, 0x0A, 0x1B, 0x0A }, // 4: плотная шахматка
    { 0x1F, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A }, // 5: густой узор
    { 0x1F, 0x1B, 0x1F, 0x1B, 0x1F, 0x1B, 0x1F, 0x1B }, // 6: почти черный
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }  // 7: полностью черный
  };

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, levels[i]);
  }
  
  lcd.clear();
}

void loop() {
  if (dataFile.available() >= 256) {
    dataFile.read(frameBuffer, 256);

    for (int row = 0; row < 2; row++) {
      lcd.setCursor(0, row);
      for (int col = 0; col < 16; col++) {
        int sum = 0;
        int blockIndex = (row * 16 + col);
        
        // Считаем белые пиксели в блоке 5x8 из буфера
        for (int line = 0; line < 8; line++) {
          uint8_t b = frameBuffer[blockIndex * 8 + line];
          for (int bit = 0; bit < 5; bit++) {
            if (b & (1 << (4 - bit))) {
              sum++;
            }
          }
        }

        // Превращаем плотность пикселей (0..40) в индекс узора (0..7)
        int charIndex = map(sum, 0, 40, 0, 7);
        lcd.write((uint8_t)charIndex);
      }
    }

    // Задержка для плавной анимации
    delay(30);

  } else {
    dataFile.seek(0); // Зацикливаем видео при окончании файла
  }
}