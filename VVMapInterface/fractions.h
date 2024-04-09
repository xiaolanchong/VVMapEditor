
#ifndef _FRACTIONS_H_included
#define _FRACTIONS_H_included

#include <windows.h>

	typedef __int64 i64;
	
	struct KFraction
	{
		i64 numerator;
		i64 denumerator;

		KFraction(i64 num) : numerator(num), denumerator (1) {}
		KFraction(i64 num, i64 denum) : numerator(num), denumerator (denum) {}
		KFraction() : numerator(0), denumerator (1) {}

		bool operator==	( const KFraction& f) const { return diff_sign(f) == 0; }
		bool operator< 	( const KFraction& f) const { return diff_sign(f) <  0; }
		bool operator>	( const KFraction& f) const { return diff_sign(f) >  0; }
		bool operator<=	( const KFraction& f) const { return diff_sign(f) <= 0; }
		bool operator>=	( const KFraction& f) const { return diff_sign(f) >= 0; }

		double float_res() const { return double(numerator) / double(denumerator);}
	private:
		i64 diff_num(const KFraction& f) const 
		{ return (numerator*f.denumerator - denumerator*f.numerator); }
		
		i64 diff_denum(const KFraction& f) const 
		{	return (denumerator*f.denumerator); }

		int sign(i64 n) const 
		{ 
			if (n>0) return +1;
			if (n<0) return -1;
			return 0;
		}
/*
		int diff_sign(const KFraction& f) const	
		{return sign(diff_num(f)) * sign(diff_denum(f));	}
*/
//
		int diff_sign(const KFraction& f) const	
		{
			return CompareFraction( numerator, denumerator, f.numerator, f.denumerator);
		}

		long CompareFraction(i64 a, i64 b, i64 c, i64 d) const
		{
			if ((b==0) && (d==0)) {
				return (lsign(lsign(a)-lsign(c)));
			}
			if (b==0) return lsign(a);
			if (d==0) return -lsign(c);
			
			i64 k1,r1,k2,r2;
			
			long s1=lsign(a)*lsign(b), s2=lsign(c)*lsign(d);
			if (s1>s2) return 1;
			if (s1<s2) return -1;
			if ((a==0) && (c==0)) return 0;
			
			long lRes=s1;
			a=a*lsign(a); b=b*lsign(b); c=c*lsign(c); d=d*lsign(d);
			
			for(;;)
			{
				divr(a,b,&k1,&r1); divr(c,d,&k2,&r2);
				if (k1>k2) return lRes;
				if (k1<k2) return -lRes;
				if ((r1==0) && (r2==0)) return 0;
				if (r1==0) return -lRes;
				if (r2==0) return lRes;
				a=b; c=d;
				b=r1; d=r2;
				lRes=-lRes;
			}
		}

		long lsign(i64 x)  const
		{
			if (x==0) return 0;
			return (x>0)?1:(-1);
		}

		void divr(i64 a, i64 b, i64 *pd, i64 *pr) const 
		{
			*pd=a/b;
			*pr=a%b;	
		}
	};

#endif