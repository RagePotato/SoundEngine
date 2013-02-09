/*
 * =====================================================================================
 *
 *       Filename:  math_functions.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2013 01:43:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  RagePotato
 *        Company:  Personal
 *
 * =====================================================================================
 */

#include "math_functions.h"

#include <limits>

template <class I>
I gcd(const I& a,const I& b){
	while(true){
		if(a==0) return b;
		b%=a;
		if(b==0) return a;
		a%=b;
	}
}

template <class I>
I lcm(const I& a,const I& b){
	I div=gcd(a,b);
	return (a/div * b);
}


template <class N, class I>
ratio<N,I>::ratio(N in){
	I num1=((I)in);
	I den1=1;
	if(std::numeric_limits<N>::min()>0){
		I num2=value_type((in-(I)in)/(std::numeric_limits<N>::min()));
		I den2=value_type(1/(std::numeric_limits<N>::min();
		den=lcm(den1,den2);
		num= num1*(den/den1) + num2*(den/den2)
	}
	else{
		num=num1;
		den=den1;
	}
}

template <class N, class I>
void ratio<N,I>::operator=(const N& rhs){
	I num1=((I)rhs);
	I den1=1;
	if(std::numeric_limits<N>::min()>0){
		I num2=value_type((rhs-(I)rhs)/(std::numeric_limits<N>::min()));
		I den2=value_type(1/(std::numeric_limits<N>::min();
		den=lcm(den1,den2);
		num= num1*(den/den1) + num2*(den/den2)
	}
	else{
		num=num1;
		den=den1;
	}
}

template <class N, class I>
void ratio<N,I>::invert(){
	I temp=num;
	num=den;
	den=temp;
}

template <class N, class I>
ratio<N,I>::value_type ratio<N,I>::num(){
	return num;
}

template <class N, class I>
ratio<N,I>::value_type ratio<N,I>::den(){
	return den;
}



