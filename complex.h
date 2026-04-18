#ifndef COMPLEX_H
#define COMPLEX_H

#include <iostream>
#include <cmath>
#include "exceptions.h"

class Complex
{
public:
    double re;
    double im;

    Complex() : re(0.0), im(0.0) {}
    Complex(double re, double im = 0.0) : re(re), im(im) {}

    double Abs() const 
    { 
        return std::sqrt(re * re + im * im); 
    }

    Complex operator+(const Complex& o) const 
    { 
        return Complex(re + o.re, im + o.im); 
    }

    Complex operator-(const Complex& o) const
    {
        return Complex(re - o.re, im - o.im);
    }

    Complex operator-(double s) const
    {
        return Complex(re - s, im);
    }

    Complex operator*(const Complex& o) const
    {
        return Complex(re * o.re - im * o.im, re * o.im + im * o.re);
    }

    Complex operator*(double s) const 
    { 
        return Complex(re * s, im * s); 
    }

    bool operator==(const Complex& o) const 
    { 
        return re == o.re && im == o.im; 
    }

    bool operator!=(const Complex& o) const 
    { 
        return !(*this == o); 
    }

    //Деление на комплексное число
    Complex operator/(const Complex& o) const
    {
        double denom = o.re * o.re + o.im * o.im;
        if (denom == 0.0)
            throw InvalidArgumentException("Complex: division by zero");
        return Complex(
            (re * o.re + im * o.im) / denom,
            (im * o.re - re * o.im) / denom
        );
    }

    //Деление на скаляр
    Complex operator/(double s) const
    {
        if (s == 0.0)
            throw InvalidArgumentException("Complex: division by zero");
        return Complex(re / s, im / s);
    }

    friend std::ostream& operator<<(std::ostream& os, const Complex& c)
    {
        os << c.re;
        if (c.im >= 0) os << "+";
        os << c.im << "i";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Complex& c)
    {
        char sign, i_char;
        is >> c.re >> sign >> c.im >> i_char;
        if (sign == '-') c.im = -c.im;
        return is;
    }
};

// Скаляр / комплексное
inline Complex operator/(double s, const Complex& c)
{
    double denom = c.re * c.re + c.im * c.im;
    if (denom == 0.0)
        throw InvalidArgumentException("Complex: division by zero");
    return Complex(s * c.re / denom, -s * c.im / denom);
}

//Перегрузки elemAbs
inline double elemAbs(int x)
{
    return (x < 0) ? -static_cast<double>(x) : static_cast<double>(x);
}

inline double elemAbs(double x)
{
    return (x < 0) ? -x : x;
}

inline double elemAbs(const Complex& x)
{
    return x.Abs();
}

#endif // COMPLEX_H