/*
 *  ESATMath.h - Small, fast, limited-precision math functions for ESAT.
 *
 * This file is part of Theia Space's ESAT Util library.
 *
 * Theia Space's ESAT Util library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT Util library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef ESATMath_h
#define ESATMath_h

#include <Energia.h>

class ESATMath
{
  public:
    // Absolute value.
    float abs(float x);

    // Arc tangent function of two variables.
    float atan2(float y, float x);

    // Cosine function (argument in radians).
    float cos(float x);

    // Modulo operator: n mod d (same sign as d)
    float modulo(float n, float d);

    // Remainder of n/d.
    float remainder(float n, float d);

    // Sine function (argument in radians).
    float sin(float x);

    // Square root.
    float sqrt(float x);
};

extern ESATMath Math;

#endif
