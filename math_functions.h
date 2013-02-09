/*
 * =====================================================================================
 *
 *       Filename:  math_functions.h
 *
 *    Description:  Converts other numeric types into ratios. The ratio class should 
 *    		    should then reduce the fraction.
 *
 *        Version:  1.0
 *        Created:  02/09/2013 01:21:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  RagePotato
 *        Company:  Personal
 *
 * =====================================================================================
 */

template <class I>
I gcd(const I& a, const I& b);

template <class I>
I lcm(const I& a, const I& b);

template <class N, class I=int>
class ratio{
	public:
		typedef I value_type; 

		ratio(N in);
		//~ratio();
		value_type num();
		value_type den();
		void invert();
		void operator=(const N& rhs);
	private:
		I num,den;
};
