/*!
[config]
name: Scalar and vector operators
clc_version_min: 10
!*/

void test() {
	int a;
	int4 b;

	b = a+b;
	b = a-b;
	b = a*b;
	b = a/b;
	b = a%b;
	b = a>b;
	b = a<b;
	b = a>=b;
	b = a<=b;
	b = a==b;
	b = a!=b;
	b = a&b;
	b = a|b;
	b = a^b;
	b = a&&b;
	b = a||b;
	b = b<<a;
	b = b>>a;
	b = a;

	b+=a;
	b-=a;
	b*=a;
	b/=a;
	b%=a;
	b&=a;
	b|=a;
	b^=a;
	b<<=a;
	b>>=a;
	b+=b,a;
}
