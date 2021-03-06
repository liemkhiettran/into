/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIIMATHIEEEFLOAT_H
#define _PIIMATHIEEEFLOAT_H

namespace Pii
{
  /**
   * A traits structure that specifies the bit counts and
   * corresponding integer types for a floating-point number.
   * Specialization are provided for `float` and `double`.
   */
  template <class Real> struct IeeeFloatTraits;

  /**
   * A traits structure that specifies the bit counts and
   * corresponding integer types for the `float` type.
   */
  template <> struct IeeeFloatTraits<float>
  {
    enum { fractionBits = 23 };
    enum { exponentBits = 8 };
    typedef int IntegerType;
    typedef unsigned int UnsignedType;
  };

  /**
   * A traits structure that specifies the bit counts and
   * corresponding integer types for the `double` type.
   */
  template <> struct IeeeFloatTraits<double>
  {
    enum { fractionBits = 52 };
    enum { exponentBits = 11 };
    typedef long long IntegerType;
    typedef unsigned long long UnsignedType;
  };

  /**
   * A class that wraps a floating-point number and allows one to
   * twiddle its bits. The class does not currently support `long`
   * `double`.
   *
   * ! Make yourself familiar with the IEEE 754-1985 standard
   * before using this class. Most importantly, learn the difference
   * between binary and base-10 exponents and note that the floating
   * point number format stores the binary exponent in a biased format.
   */
  template <class Real> class IeeeFloat
  {
  public:
    typedef IeeeFloatTraits<Real> Traits;
    typedef typename IeeeFloatTraits<Real>::IntegerType IntegerType;
    typedef typename IeeeFloatTraits<Real>::UnsignedType UnsignedType;

    /**
     * Create a wrapper for `value`.
     */
    explicit IeeeFloat(Real value = 0) : _value(value) {}

    /**
     * Returns the stored value.
     */
    operator Real () const { return _value; }

    /**
     * Get the unbiased exponent of the floating-point number.
     */
    IntegerType exponent() const { return _exponent - ((1 << (Traits::exponentBits-1)) - 1); }
    /**
     * Set the unbiased exponent of the floating-point number.
     */
    IntegerType setExponent(int exponent) { _exponent = exponent + ((1 << (Traits::exponentBits-1)) - 1); }

    /**
     * Get the unbiased exponent of the floating-point number.
     */
    UnsignedType rawExponent() const { return _exponent; }
    /**
     * Set the unbiased exponent of the floating-point number.
     */
    void setRawExponent(UnsignedType exponent) { _exponent = exponent; }

    /**
     * Returns the sign of the number: -1 for negative numbers and 1
     * for positive ones.
     */
    IntegerType sign() const { return 1 - (_sign << 1); }
    /**
     * Set the sign.
     */
    void setSign(IntegerType sign) { _sign = UnsignedType((1 - sign) >> 1); }

    /**
     * Get the sign bit. 0 means positive, 1 negative.
     */
    UnsignedType rawSign() const { return _sign; }
    /**
     * Set the sign bit.
     */
    void setRawSign(UnsignedType sign) { _sign = sign; }

    /**
     * Add a value to the exponent. This is a fast way to
     * multiply/divide the number by a power of two. Add one, and the
     * number will be multiplied by two. Add -2, and the number will
     * be divided by four.
     */
    void addExponent(IntegerType diff) { _exponent += diff; }

    /**
     * Set the floating point value.
     */
    void setValue(Real value) { _value = value; }
    /**
     * Get the floating point value.
     */
    Real value() const { return _value; }

  private:
    union
    {
      struct
      {
        UnsignedType _fraction : Traits::fractionBits;
        UnsignedType _exponent : Traits::exponentBits;
        UnsignedType _sign : 1;
      };
      Real _value;
    };
  };

};

#endif //_PIIMATHIEEEFLOAT_H
