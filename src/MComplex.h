#pragma once
#ifndef __MUZI_MCOMPLEX_H__
#define __MUZI_MCOMPLEX_H__
#include<concepts>
#include<type_traits>
#include<cmath>
#include<iostream>
namespace MUZI
{
	template<typename T>
	concept __m_complex_inline_type__ = requires(T x)
	{
		std::integral<T> || std::floating_point<T>;
	};

	template<__m_complex_inline_type__ T = double>
	class MComplex
	{
	public:
		friend std::ostream& operator<< <T>(std::ostream&, const MComplex<T>&);
	public:
		MComplex(T re = 0, T im = 0) :re(re), im(im) {}
		MComplex(const MComplex<T>& other) :re(other.re), im(other.im) {}
		MComplex(MComplex<T>&& other) :re(other.re), im(other.im)
		{
			other.re = 0;
			other.im = 0;
		}
	public:
		MComplex<T> operator+(T& other)
		{
			return (MComplex(*this) += other);
		}
		MComplex<T> operator-(T& other)
		{
			return (MComplex(*this) -= other);
		}
		MComplex<T> operator*(T& other)
		{
			return MComplex(*this) *= other;
		}
		MComplex<T> operator/(T& other)
		{
			return MComplex(*this) /= other;
		}
		MComplex<T> operator+(const MComplex<T>& other)
		{
			return (MComplex(*this) += other);
		}
		MComplex<T> operator-(const MComplex<T>& other)
		{
			return (MComplex(*this) -= other);
		}
		MComplex<T> operator*(const MComplex<T>& other)
		{
			return MComplex(*this) *= other;
		}
		MComplex<T> operator/(const MComplex<T>& other)
		{
			return MComplex(*this) /= other;
		}
		MComplex<T> operator+=(T& other)
		{
			return (MComplex(*this) += MComplex<T>(other));
		}
		MComplex<T> operator-=(T& other)
		{
			return (MComplex(*this) -= MComplex<T>(other));
		}
		MComplex<T> operator*=(T& other)
		{
			return (MComplex(*this) *= MComplex<T>(other));
		}
		MComplex<T> operator/=(T& other)
		{
			return (MComplex(*this) /= MComplex<T>(other));
		}
		MComplex<T>& operator+=(const MComplex<T>& other)
		{
			this->re += other.re;
			this->im += other.im;
		}
		MComplex<T>& operator-=(const MComplex<T>& other)
		{
			this->re -= other.re;
			this->im -= other.im;
		}
		MComplex<T>& operator*=(const MComplex<T>& other)
		{
			// 乘法交叉计算的临时结果
			T re_tmp = this->re * other.re - this->im * other.im;
			T im_tmp = this->re * other.im + this->im * other.re;
			this->re = re_tmp;
			this->im = im_tmp;
		}
		MComplex<T>& operator/=(const MComplex<T>& other)
		{
			T sqrt_denominator = (other.re * other.re + other.im * other.im);
			T re_tmp = (this->re * other.re + this->im * other.im) / sqrt_denominator;
			T im_tmp = (this->re * other.im - this->im * other.re) / sqrt_denominator;
			this->re = re_tmp;
			this->im = im_tmp;
		}
	public:
		friend MComplex<T> operator+(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator-(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator*(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator/(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator+=(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator-=(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator*=(T& num, const MComplex<T>& complex);
		friend MComplex<T> operator/=(T& num, const MComplex<T>& complex);
	public:
		T real() const { return this->re; }
		T imag() const { return this->im; }
		T& real() { return this->re; }
		T& imag() { return this->im; }
		void setReal(T& re)
		{
			this->re = re;
		}
		void setImag(T& im)
		{
			this->im = im;
		}
	private:
		T re/*实部*/, im/*虚部*/;
	};
	template<__m_complex_inline_type__ T = double>
	MComplex<T> operator+(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) += complex;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T> operator-(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) -= complex;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T> operator*(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) *= complex;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T> operator/(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) /= complex;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T>& operator+=(T& num, const MComplex<T>& complex)
	{
		return MComplex(complex) += num;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T>& operator-=(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) -= complex;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T>& operator*=(T& num, const MComplex<T>& complex)
	{
		return MComplex(complex) *= num;
	}
	template<__m_complex_inline_type__ T = double>
	MComplex<T>& operator/=(T& num, const MComplex<T>& complex)
	{
		return MComplex(num) /= complex;
	}
	template<__m_complex_inline_type__ T = double>
	std::ostream& operator<< (std::ostream& os, const MComplex<T>& complex)
	{
		return os << complex.re << " + " << complex.im << "i ";
	}


};
#endif