/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ESAT_Util.h>

// ESAT_Util example program.
// Type conversions.

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
}

void loop()
{
  (void) Serial.println(String("Conversions example program."));
  // Binary coded decimal conversions.
  const word fourDigitNumber = 1729;
  (void) Serial.print(String("Four-digit number: "));
  (void) Serial.println(String(fourDigitNumber, DEC));
  (void) Serial.print("Binary coded decimal bits: ");
  const word binaryCodedDecimalBits =
    ESAT_Util.encodeBinaryCodedDecimalWord(fourDigitNumber);
  (void) Serial.println(ESAT_Util.pad(String(binaryCodedDecimalBits, BIN),
                                      '0',
                                      DEC));
  (void) Serial.print(String("Four-digit number encoded by the bits: "));
  const word recoveredFourDigitNumber =
    ESAT_Util.decodeBinaryCodedDecimalWord(binaryCodedDecimalBits);
  (void) Serial.println(String(recoveredFourDigitNumber, DEC));
  // Integer number conversions.
  const long integerNumber = -123456789;
  (void) Serial.print(String("Integer number: "));
  (void) Serial.println(String(integerNumber, DEC));
  (void) Serial.print(String("Bits of the integer number: "));
  const unsigned long integerNumberBits =
    ESAT_Util.longToUnsignedLong(integerNumber);
  (void) Serial.println(ESAT_Util.pad(String(integerNumberBits, BIN),
                                      '0',
                                      32));
  (void) Serial.print(String("Integer number encoded by the bits: "));
  (void) Serial.println(ESAT_Util.unsignedLongToLong(integerNumberBits), DEC);
  // Floating-point number conversions.
  const float floatingPointNumber = 3.1416;
  (void) Serial.print(String("Floating-point number: "));
  (void) Serial.println(String(floatingPointNumber, 4));
  (void) Serial.print(String("Bits of the floating-point number: "));
  const unsigned long floatingPointNumberBits =
    ESAT_Util.floatToUnsignedLong(floatingPointNumber);
  (void) Serial.println(ESAT_Util.pad(String(floatingPointNumberBits, BIN),
                                      '0',
                                      32));
  (void) Serial.print(String("Floating-point number encoded by the bits: "));
  const float recoveredFloatingPointNumber =
    ESAT_Util.unsignedLongToFloat(floatingPointNumberBits);
  (void) Serial.println(String(recoveredFloatingPointNumber, 4));
  // End.
  (void) Serial.println(String("End."));
  delay(1000);
}
