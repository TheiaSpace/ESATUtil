/*
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

#include "ESATMath.h"

float ESATMath::abs(float x)
{
  return (x >= 0.0f) ? x : -x;
}

// Fast atan2, after
// http://www.dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization
float ESATMath::atan2(float y, float x)
{
  if ((y == 0.0f) && (x == 0.0f))
  {
    return 0.0f;
  }
  const float yabs = this->abs(y);
  const float sign = ((y >= 0.0f) ? 1.0f : -1.0f);
  const float r = ((x >= 0.0f)
                       ? (x - yabs) / (x + yabs)
                       : (x + yabs) / (yabs - x));
  const float theta0 = ((x >= 0.0f) ? PI / 4.0f : 3.0f * PI / 4.0f);
  const float r2 = r * r;
  const float a =
    sign * (theta0 + r * (-0.995631f + r2 * (0.289219f - 0.078986f * r2)));
  return this->modulo(a, 2 * PI);
}

// Fast cosine as seen in
// http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
float ESATMath::cos(float x)
{
  const float y = (x > PI / 2.0f) ? x - 3.0f * PI / 2.0f : x + PI / 2.0f;
  return this->sin(y);
}

float ESATMath::modulo(float n, float d)
{
  const float r = this->remainder(n, d);
  const float m = (n * d >= 0) ? r : (r + d);
  return m;
}

float ESATMath::remainder(float n, float d)
{
  const long quotient = static_cast<long>(n / d);
  return n - d * quotient;
}

// Fast sine as seen in
// http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
float ESATMath::sin(float x)
{
  const float s = ((x >= 0.0f) ? 1.0f : -1.0f);
  const float xnorm = s * (this->remainder (s * x + PI, 2.0f * PI) - PI);
  const float sin1 = (4.0f / PI) * xnorm * (1.0f - (1.0f / PI) * abs(xnorm));
  return sin1 * (0.775f + 0.225f * this->abs(sin1));
}

// Fast square root
float ESATMath::sqrt(float x)
{
  const long x_bits = *(reinterpret_cast<const long*>(&x));
  const long s_bits = (1l << 29) + (x_bits >> 1) - (1l << 22);
  const float s = *(reinterpret_cast<const float*>(&s_bits));
  return 0.5f * (s + x / s);
}

ESATMath Math;
