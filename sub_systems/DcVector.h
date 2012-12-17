//
//  DcVector.h
//  Dc
//
//  Created by Nakajima Kent on 12/02/23.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef Dc_DcVector_h
#define Dc_DcVector_h

using DcFloat = double;

class DcTvector{
	
public:
	DcFloat x;
	DcFloat y;
	
	inline DcTvector(){
		x=0;
		y=0;
	}
	inline DcTvector(DcFloat xValue,DcFloat yValue){
		x=xValue;
		y=yValue;
	}
	
	inline void log(){
		printf("x:%1.5f  y:%1.5f\n",x,y);
	}
	inline DcFloat magnitude()const{
		return sqrt(x*x+y*y);
	}
	inline DcFloat sqarMagnitude() const{
		return x*x+y*y;
	}
	inline DcFloat sin()const{
		return y/magnitude();
	}
	inline DcFloat cos()const{
		return x/magnitude();
	}
	inline DcFloat tan()const{
		return y/x;
	}
	inline DcTvector rotateRightAngle()const{
		return DcTvector(y,-x);
	}
	inline DcTvector normalize()const{
		DcFloat Magnitude=magnitude();
		return DcTvector(x/Magnitude, y/Magnitude);
	}
	inline DcTvector setMagnitude(DcFloat NewMagnitude)const{
		DcFloat Magnitude=NewMagnitude/magnitude();
		return DcTvector(x*Magnitude, y*Magnitude);
	}
	inline DcTvector reverse() const{
		return DcTvector(-x,-y);
	}
};


inline DcTvector DcAngleTvector(DcFloat Angle,DcFloat Magnitude){
	return DcTvector(Magnitude*cos(Angle), Magnitude*sin(Angle));
}
inline DcTvector operator -	(const DcTvector &R){
	return DcTvector(-R.x, -R.y);
}
inline DcTvector operator +	(const DcTvector &R){
	return R;
}
inline DcTvector operator +	(const DcTvector &L,const DcTvector &R){
	return DcTvector(L.x+R.x, L.y+R.y);
}
inline DcTvector operator -	(const DcTvector &L,const DcTvector &R){
	return DcTvector(L.x-R.x, L.y-R.y);
}
inline DcFloat operator ^	(const DcTvector &L,const DcTvector &R){
	return L.x*R.y-L.y*R.x;
}
inline DcFloat operator *	(const DcTvector &L,const DcTvector &R){
	return L.x*R.x+L.y*R.y;
}
inline DcTvector operator *	(const DcTvector &L,DcFloat R){
	return DcTvector(L.x*R, L.y*R);
}
inline DcTvector operator *	(DcFloat L,const DcTvector &R){
	return DcTvector(R.x*L, R.y*L);
}
inline DcTvector operator ^	(const DcTvector &L,DcFloat R){
	return DcTvector(L.y*R, -L.x*R);
}
inline DcTvector operator ^	(DcFloat L,const DcTvector &R){
	return DcTvector(-L*R.y, L*R.x);
}
inline DcTvector operator /	(const DcTvector &L,DcFloat R){
	return DcTvector(L.x/R, L.y/R);
}
inline DcTvector operator +=	(DcTvector &L,const DcTvector &R){
	L.x+=R.x;
	L.y+=R.y;
	return L;
}
inline DcTvector operator -=	(DcTvector &L,const DcTvector &R){
	L.x-=R.x;
	L.y-=R.y;
	return L;
}
inline DcTvector operator *=	(DcTvector &L,DcFloat R){
	L.x*=R;
	L.y*=R;
	return L;
}
inline DcTvector operator /=	(DcTvector &L,DcFloat R){
	L.x/=R;
	L.y/=R;
	return L;
}
inline bool operator ==	(const DcTvector &L,const DcTvector &R){
	if (L.x==R.x && L.y==R.y)
		return true;
	return false;
}
inline bool operator  !=	(const DcTvector &L,const DcTvector &R){
	if (L.x==R.x && L.y==R.y)
		return false;
	return true;
}


class DcTmatrix{
	//
	//  |11 12|
	//  |21 22|
	//
	
public:
	DcFloat element11, element12;
	DcFloat element21, element22;
	
	inline DcTmatrix(){
		element11=1.0;
		element12=0.0;
		element21=0.0;
		element22=1.0;
	}
	inline DcTmatrix(DcTvector Vector1,DcTvector Vector2){
		element11=Vector1.x;
		element12=Vector2.x;
		element21=Vector1.y;
		element22=Vector2.y;
	}
	inline DcTmatrix(DcFloat Element11,DcFloat Element12, 
			DcFloat Element21, DcFloat Element22){
		element11=Element11;
		element12=Element12;
		element21=Element21;
		element22=Element22;
	}
	
	inline DcFloat det() const{
		return element11*element22-element21*element12;
	}
	inline DcTmatrix transpose() const{
		return DcTmatrix(element11,element21,
						 element12,element22);
	}
	
	DcTvector get1() const{
		return DcTvector(element11,element21);
	}
	DcTvector get2() const{
		return DcTvector(element12,element22);
	}
	void set1(DcTvector Vector1){
		element11=Vector1.x;
		element21=Vector1.y;
	}
	void set2(DcTvector Vector2){
		element12=Vector2.x;
		element22=Vector2.y;
	}
	DcTmatrix inverse() const{
		DcFloat det = 1.0f/(element11*element22-element12*element21);
		return DcTmatrix(element22*det, -element12*det,
						-element21*det, element11*det);
	}
};

inline DcTmatrix DcRotatedTmatrix(DcFloat Angle){
	DcFloat Sin=sin(Angle);
	DcFloat Cos=cos(Angle);
	return DcTmatrix(Cos, -Sin, Sin, Cos);
}
inline DcTmatrix DcScaleTmatrix(DcFloat Scale){
	return DcTmatrix(Scale, 1.0, 1.0, Scale);
}
inline DcTmatrix operator +	(const DcTmatrix &L,const DcTmatrix &R){
	return DcTmatrix(L.element11+R.element11, L.element12+R.element12,
					 L.element21+R.element21, L.element22+R.element22);
}
inline DcTmatrix operator -		(const DcTmatrix &L,const DcTmatrix &R){	
	return DcTmatrix(L.element11-R.element11, L.element12-R.element12,
					 L.element21-R.element21, L.element22-R.element22);
}
inline DcTmatrix operator *		(const DcTmatrix &L,const DcTmatrix &R){
	return DcTmatrix(L.element11*R.element11+L.element12*R.element21,
					 L.element11*R.element12+L.element12*R.element22,
					 L.element21*R.element11+L.element22*R.element21,
					 L.element21*R.element12+L.element22*R.element22);
}

inline DcTmatrix operator *		(const DcTmatrix &L,DcFloat R){
	return DcTmatrix(L.element11*R, L.element12*R,
					 L.element21*R, L.element22*R);
}
inline DcTmatrix operator *		(DcFloat L,const DcTmatrix &R){
	return DcTmatrix(R.element11*L, R.element12*L,
					 R.element21*L, R.element22*L);
}
inline DcTmatrix operator /		(const DcTmatrix &L,DcFloat R){
	return DcTmatrix(L.element11/R, L.element12/R,
					 L.element21/R, L.element22/R);
}
inline DcTmatrix operator +=	(DcTmatrix &L,const DcTmatrix &R){
	L.element11+=R.element11;	L.element12+=R.element12;
	L.element21+=R.element21;	L.element22+=R.element22;
	return L;
}
inline DcTmatrix operator -=	(DcTmatrix &L,const DcTmatrix &R){
	L.element11-=R.element11;	L.element12-=R.element12;
	L.element21-=R.element21;	L.element22-=R.element22;
	return L;
}
inline DcTmatrix operator *=	(DcTmatrix &L,const DcTmatrix &R){
	return L=DcTmatrix(L.element11*R.element11+L.element12*R.element21,
					   L.element11*R.element12+L.element12*R.element22,
					   L.element21*R.element11+L.element22*R.element21,
					   L.element21*R.element12+L.element22*R.element22);
}
inline DcTmatrix operator *=	(DcTmatrix &L,const DcFloat R){
	L.element11*=R;	L.element12*=R;
	L.element21*=R;	L.element22*=R;
	return L;
}
inline DcTmatrix operator/=	(DcTmatrix &L,const DcFloat R){
	L.element11/=R;	L.element12/=R;
	L.element21/=R;	L.element22/=R;
	return L;
}
inline DcTvector operator *		(const DcTmatrix &L,const DcTvector &R){
	return DcTvector(R.x*L.element11+R.y*L.element12,
					 R.x*L.element21+R.y*L.element22);
}
inline DcTvector operator *		(const DcTvector &L,const DcTmatrix &R){
	return DcTvector(L.x*R.element11+L.y*R.element12,
					 L.x*R.element21+L.y*R.element22);
}
inline DcTvector operator *=	(DcTvector &L,const DcTmatrix &R){
	return L=DcTvector(L.x*R.element11+L.y*R.element12,
					   L.x*R.element21+L.y*R.element22);
}
inline bool operator ==	(const DcTmatrix &L,const DcTmatrix &R){
	if( L.element11==R.element11 && L.element12==R.element12
	   && L.element21==R.element21 && L.element22==R.element22)
		return true;
	return false;
}
inline bool operator  !=	(const DcTmatrix &L,const DcTmatrix &R){
	if( L.element11==R.element11 && L.element12==R.element12
	   && L.element21==R.element21 && L.element22==R.element22)
		return false;
	return true;
}


class DcVector{
	
public:
	DcFloat x;
	DcFloat y;
	DcFloat z;
	
	DcVector(){
		x=0;
		y=0;
		z=0;
	}
	DcVector(DcFloat xValue,DcFloat yValue,DcFloat zValue){
		x=xValue;
		y=yValue;
		z=zValue;
	}
	DcVector(double *Value){
		x=Value[0];
		y=Value[1];
		z=Value[2];
	}
	DcVector(float *Value){
		x=Value[0];
		y=Value[1];
		z=Value[2];
	}
	DcVector(DcTvector V){
		x=V.x;
		y=V.y;
		z=0.0;
	}
	operator DcTvector(){
		return DcTvector(x,y);
	}
	
	inline void log(){
		printf("x: %1.5f\t\ty: %1.5f\t\tz: %1.5f\n",x,y,z);
	}
	inline DcFloat magnitude() const{
		return sqrt(x*x+y*y+z*z);
	}
	inline DcFloat sqarMagnitude() const{
		return x*x+y*y+z*z;
	}
	inline DcVector normalize() const{
		DcFloat Magnitude=magnitude();
		return DcVector(x/Magnitude, y/Magnitude, z/Magnitude);
	}
	inline DcVector setMagnitude(DcFloat NewMagnitude)const{
		DcFloat Magnitude=NewMagnitude/magnitude();
		return DcVector(x*Magnitude, y*Magnitude, z*Magnitude);
	}
	inline DcVector revert(){
		return DcVector(-x,-y,-z);
	}
};

inline DcVector DcXYVector(DcTvector XY,DcFloat Z){
	return DcVector(XY.x, XY.y, Z);
}
inline DcVector DcYZVector(DcTvector YZ,DcFloat X){
	return DcVector(X, YZ.x, YZ.y);
}
inline DcVector DcZXVector(DcTvector ZX,DcFloat Y){
	return DcVector(ZX.y, Y, ZX.x);
}
inline DcVector operator -	(const DcVector &R){
	return DcVector(-R.x, -R.y, -R.z);
}
inline DcVector operator +	(const DcVector &R){
	return R;
}
inline DcVector operator +	(const DcVector &L,const DcVector &R){
	return DcVector(L.x+R.x, L.y+R.y, L.z+R.z);
}
inline DcVector operator -	(const DcVector &L,const DcVector &R){
	return DcVector(L.x-R.x, L.y-R.y, L.z-R.z);
}
inline DcVector operator ^	(const DcVector &L,const DcVector &R){
	return DcVector(L.y*R.z-L.z*R.y, L.z*R.x-L.x*R.z, L.x*R.y-L.y*R.x);
}
inline DcFloat operator *	(const DcVector &L,const DcVector &R){
	return L.x*R.x+L.y*R.y+L.z*R.z;
}
inline DcVector operator *	(const DcVector &L,DcFloat R){
	return DcVector(L.x*R, L.y*R, L.z*R);
}
inline DcVector operator *	(DcFloat L,const DcVector &R){
	return DcVector(R.x*L, R.y*L, R.z*L);
}
inline DcVector operator /	(const DcVector &L,DcFloat R){
	return DcVector(L.x/R, L.y/R, L.z/R);
}
inline DcVector operator +=	(DcVector &L,const DcVector &R){
	L.x+=R.x;
	L.y+=R.y;
	L.z+=R.z;
	return L;
}
inline DcVector operator -=		(DcVector &L,const DcVector &R){
	L.x-=R.x;
	L.y-=R.y;
	L.z-=R.z;
	return L;
}
inline DcVector operator ^=		(DcVector &L,const DcVector &R){
	return L = DcVector(L.y*R.z-L.z*R.y, L.x*R.z-L.z*R.x, L.x*R.y-L.y*R.x);
}
inline DcVector operator *=		(DcVector &L,DcFloat R){
	L.x*=R;
	L.y*=R;
	L.z*=R;
	return L;
}
inline DcVector operator /=		(DcVector &L,DcFloat R){
	L.x/=R;
	L.y/=R;
	L.z/=R;
	return L;}
inline bool operator ==	(const DcVector &L,const DcVector &R){
	if (L.x==R.x && L.y==R.y && L.z==R.x )
		return true;
	return false;
}
inline bool operator  !=	(const DcVector &L,const DcVector &R){
	if (L.x==R.x && L.y==R.y && L.z==R.x )
		return false;
	return true;
}


class DcMatrix{
public:
	DcFloat element11,element12,element13;
	DcFloat element21,element22,element23;
	DcFloat element31,element32,element33;
	
	DcMatrix();
	DcMatrix(DcFloat e11,DcFloat e12,DcFloat e13,
			 DcFloat e21,DcFloat e22,DcFloat e23,
			 DcFloat e31,DcFloat e32,DcFloat e33);
	DcMatrix(DcVector vn1,DcVector vn2,DcVector vn3);
};
DcMatrix DcRotatedMatrix(DcFloat x,DcFloat  y,DcFloat z);
DcMatrix operator *(DcMatrix L,DcVector R);
DcMatrix operator *(DcVector L,DcMatrix R);

class DcQuaternion{
public:	
	DcFloat n;
	DcFloat x;
	DcFloat y;
	DcFloat z;
	
	//halfAngle
	DcQuaternion(){
		n=0;
		x=0;
		y=0;
		z=0;
	}
	DcQuaternion(DcFloat Number,DcVector Vector){
		n=Number;
		x=Vector.x;
		y=Vector.y;
		z=Vector.z;
	}
	DcQuaternion(DcFloat nValue,DcFloat xValue,DcFloat yValue,DcFloat zValue){
		n=nValue;
		x=xValue;
		y=yValue;
		z=zValue;
	}
	
	inline void log(){
		printf("n:　%1.5f  x:　%1.5f  y:　%1.5f  z:　%1.5f",n,x,y,z);
	}
	DcFloat magnitude(){
		return sqrt(n*n+x*x+y*y+z*z);
	}
	DcVector vector(){
		return DcVector(x,y,z);
	}
	DcVector rotate(DcVector v){
		DcFloat nn=n*n;
		DcFloat xx=x*x;
		DcFloat yy=y*y;
		DcFloat zz=z*z;
		return DcVector(
						2*(x*y*v.y +x*z*v.z +n*y*v.z -n*z*v.y)
						+v.x*(nn +xx -yy -zz) ,
						2*(y*z*v.z +y*x*v.x +n*z*v.x -n*x*v.z)
						+v.y*(nn +yy -zz -xx) ,
						2*(z*x*v.x +z*y*v.y +n*x*v.y -n*y*v.x)
						+v.z*(nn +zz -xx -yy)
						);
	}
	DcQuaternion rotate(DcQuaternion q){
		DcFloat nn=n*n;
		DcFloat xx=x*x;
		DcFloat yy=y*y;
		DcFloat zz=z*z;
		return DcQuaternion(
							q.n*(nn +xx +yy +zz) ,
							2*(x*y*q.y +x*z*q.z +n*y*q.z -n*z*q.y)
							+q.x*(nn +xx -yy -zz) ,
							2*(y*z*q.z +y*x*q.x +n*z*q.x -n*x*q.z)
							+q.y*(nn +yy -zz -xx) ,
							2*(z*x*q.x +z*y*q.y +n*x*q.y -n*y*q.x)
							+q.z*(nn +zz -xx -yy)							
							);
		
	}
};

inline DcQuaternion DcRotateionQuaternion(DcFloat Angle,DcVector Axis){
	return DcQuaternion(cos(Angle),Axis.setMagnitude(sin(Angle)));
}
inline DcQuaternion DcRotateionQuaternion(DcFloat Angle,DcFloat xValue,DcFloat yValue,DcFloat zValue){
	return DcQuaternion(cos(Angle),DcVector(xValue,yValue,zValue).
						setMagnitude(sin(Angle)));
}
inline DcQuaternion operator ~	(const DcQuaternion &R){
	return DcQuaternion(R.n, -R.x, -R.y, -R.z);
}
inline DcQuaternion operator +	(const DcQuaternion &L,const DcQuaternion &R){
	return DcQuaternion(L.n+R.n, L.x+R.x, L.y+R.y, L.z+R.z);
}
inline DcQuaternion operator -	(const DcQuaternion &L,const DcQuaternion &R){
	return DcQuaternion(L.n-R.n,  L.x-R.x, L.y-R.y,  L.z-R.z);
}
inline DcQuaternion operator *	(const DcQuaternion &L,const DcQuaternion &R){
	return DcQuaternion(L.n*R.n-L.x*R.x-L.y*R.y-L.z*R.z,
						L.n*R.x+L.x*R.n+L.y*R.z-L.z*R.y,
						L.n*R.y+L.y*R.n+L.z*R.x-L.x*R.z,
						L.n*R.z+L.z*R.n+L.x*R.y-L.y*R.x);
}

inline DcQuaternion operator *	(const DcQuaternion &L,DcFloat R){
	return DcQuaternion(L.n*R, L.x*R, L.y*R, L.z*R);
}
inline DcQuaternion operator *	(DcFloat L,const DcQuaternion &R){
	return DcQuaternion(R.n*L, R.x*L, R.y*L, R.z*L);
}
inline DcQuaternion operator /	(const DcQuaternion &L,DcFloat R){
	return DcQuaternion(L.n/R, L.x/R, L.y/R, L.z/R);
}

inline DcQuaternion operator +=	(DcQuaternion &L,const DcQuaternion &R){
	L.n+=R.n;
	L.x+=R.x;
	L.y+=R.y;
	L.z+=R.z;
	return L;
}
inline DcQuaternion operator -=	(DcQuaternion &L,const DcQuaternion &R){
	L.n-=R.n;
	L.x-=R.x;
	L.y-=R.y;
	L.z-=R.z;
	return L;
}
inline DcQuaternion operator *=	(DcQuaternion &L,const DcQuaternion &R){
	return DcQuaternion(L.n*R.n-L.x*R.x-L.y*R.y-L.z*R.z,
						L.n*R.x+L.x*R.n+L.y*R.z-L.z*R.y,
						L.n*R.y+L.y*R.n+L.z*R.x-L.x*R.z,
						L.n*R.z+L.z*R.n+L.x*R.y-L.y*R.x);
}

inline DcQuaternion operator *=	(DcQuaternion &L,DcFloat R){
	L.n*=R;
	L.x*=R;
	L.y*=R;
	L.z*=R;
	return L;
}
inline DcQuaternion operator /=	(DcQuaternion &L,DcFloat R){
	L.n/=R;
	L.x/=R;
	L.y/=R;
	L.z/=R;
	return L;
}

inline DcQuaternion operator *	(const DcQuaternion &L,DcVector &R){
	return DcQuaternion(-L.x*R.x-L.y*R.y-L.z*R.z,
						L.n*R.x+L.y*R.z-L.z*R.y,
						L.n*R.y+L.z*R.x-L.x*R.z,
						L.n*R.z+L.x*R.y-L.y*R.x);
}
inline DcQuaternion operator *	(DcVector &L,const DcQuaternion &R){
	return DcQuaternion(-L.x*R.x-L.y*R.y-L.z*R.z,
						L.x*R.n+L.y*R.z-L.z*R.y,
						L.y*R.n+L.z*R.x-L.x*R.z,
						L.z*R.n+L.x*R.y-L.y*R.x);
}

inline bool operator ==	(const DcQuaternion &L,const DcQuaternion &R){
	if (L.n==R.n && L.x==R.x && L.y==R.y && L.z==R.x )
		return true;
	return false;
}
inline bool operator  !=	(const DcQuaternion &L,const DcQuaternion &R){
	if (L.n==R.n && L.x==R.x && L.y==R.y && L.z==R.x )
		return false;
	return true;
}


#endif
