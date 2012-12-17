/*
 *  BnFFT.h
 *  SzoneIV
 *
 *  Created by czel on 09/10/28.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

class BnImaginary{
public:
	double val;
	
	BnImaginary();
	BnImaginary(double value);
private:
	operator double ();
};

class BnComplex{
public:
	double re;
	double im;
	
	BnComplex();
	BnComplex(double reValue);
	BnComplex(double reValue,double imValue);
	
	inline BnComplex conjugate(){
		return BnComplex(re,-im);
	}
	inline void log(){
		printf("%f + i %f\n",re,im);
	}
};

inline BnImaginary::BnImaginary(){
	val=0.0;
}
inline BnImaginary::BnImaginary(double value){
	val=value;
}
inline BnImaginary:: operator double (){
	return val;
}


BnComplex operator + (const BnComplex &R);
BnComplex operator - (const BnComplex &R);

BnComplex operator + (const BnComplex &L,const BnComplex &R);
BnComplex operator - (const BnComplex &L,const BnComplex &R);
BnComplex operator * (const BnComplex &L,const BnComplex &R);

BnComplex operator * (const BnComplex &L,const double &R);
BnComplex operator * (const BnComplex &L,const BnImaginary &R);
BnComplex operator * (const double &L,const BnComplex &R);
BnComplex operator * (const BnImaginary &L,const BnComplex &R);

BnComplex operator +=	(BnComplex &L,const BnComplex &R);
BnComplex operator -=	(BnComplex &L,const BnComplex &R);

inline BnComplex::BnComplex(){
	re=0.0;
	im=0.0;
}
inline BnComplex::BnComplex(double reValue){
	re=reValue;
	im=0.0;
}
inline BnComplex::BnComplex(double reValue,double imValue){
	re=reValue;
	im=imValue;
}

inline BnComplex operator -	(const BnComplex &R){
	return BnComplex(-R.re, -R.im);
}
inline BnComplex operator +	(const BnComplex &R){
	return R;
}

inline BnComplex operator +	(const BnComplex &L,const BnComplex &R){
	return BnComplex(L.re+R.re, L.im+R.im);
}
inline BnComplex operator -	(const BnComplex &L,const BnComplex &R){
	return BnComplex(L.re-R.re, L.im-R.im);
}
inline BnComplex operator *	(const BnComplex &L,const BnComplex &R){
	return BnComplex(L.re*R.re-L.im*R.im, L.re*R.im+L.im*R.re);
}

inline BnComplex operator * (const BnComplex &L,const double &R){
	return BnComplex(L.re*R, L.im*R);
}
inline BnComplex operator * (const BnComplex &L,const BnImaginary &R){
	return BnComplex(-L.im*R.val, L.re*R.val);
}

inline BnComplex operator * (const double &L,const BnComplex &R){
	return BnComplex(L*R.re, L*R.im);
}
inline BnComplex operator * (const BnImaginary &L,const BnComplex &R){
	return BnComplex(-L.val*R.im, L.val*R.re);
}


inline BnComplex operator +=	(BnComplex &L,const BnComplex &R){
	L.re+=R.re;
	L.im+=R.im;
	return L;
}
inline BnComplex operator -=	(BnComplex &L,const BnComplex &R){
	L.re-=R.re;
	L.im-=R.im;
	return L;
}
inline BnComplex operator *=	(BnComplex &L,const BnComplex &R){
	L = L*R;
	return L;
}

extern "C" void cdft(int, int, double *, int *, double *);

template <int depth>
class OouraFFT2n{
	
public:
	static const int length=1<<depth;
	int ip[(1<<((depth>>1)+1)) + 2];
	double  w[length * 5 / 4];
	BnComplex value[length];
	OouraFFT2n(){
		int i;
		ip[0]=0.0;
		//make dummy data
		for (i=0;i<length;i++){
			value[i]=BnComplex(cos(19.5*i*2*M_PI/length));
		}
	}
	void execute(){
		cdft(length,1,(double*)value,ip,w);
	}
};

template <int depth, bool Inverse = true>
class BnComplexFFT2n{
public:
	static const int length=1<<depth;
	
	BnComplex table[length];
	BnComplexFFT2n(){
		int i;
		//make sin,cos table
		if ( not Inverse){
			for (i=0;i<length;i++){
				table[i]=BnComplex(cos(i*2*M_PI/length),sin(i*2*M_PI/length));
			}
		}
		else {
			for (i=0;i<length;i++){
				table[i]=BnComplex(cos(i*2*M_PI/length),-sin(i*2*M_PI/length));
			}
		}
	}
	void execute(BnComplex* value){
		const int length=1<<depth;
		
		
		BnComplex tmp;
		int i,j,k;
		int subCount;
		int offset;
		int idush;
		int ioffset;
		int joffset;
		
		//BnComplex value[length];
		
		
		//make dummy data
		//for (i=0;i<length;i++){
		//	value[i]=BnComplex(cos(19.5*i*2*M_PI/length));
		//}
		
		//time(sample) culling
		
		for (k=0;k<depth;k++){
			offset=length>>(k+1);
			subCount=1<<k;
			for (j=0;j<subCount;j++){
				joffset=j*offset*2;
				
				//i=0
				{
					idush=joffset;
					ioffset=idush+offset;
					tmp=value[idush]+value[ioffset];
					value[ioffset]=value[idush]-value[ioffset];
					value[idush]=tmp;
				}
				for (i=1;i<offset;i++){
					idush=i+joffset;
					ioffset=idush+offset;
					tmp=value[idush]+value[ioffset];
					value[ioffset]=(value[idush]-value[ioffset])*table[i<<k];
					value[idush]=tmp;
				}
			}
		}
		
		int n=length;
		i = 0;
		for (j = 1; j < n - 1; j++) {
			for (k = n >> 1; k > (i ^= k); k >>= 1);
			if (j < i) {
				tmp = value[j];
				value[j] = value[i];
				value[i] = tmp;
			}
		}
		
		//for (i=0;i<length;i++){
		//	printf("%f\t%f\n",value[i].re,value[i].im);
		//}
		
	}
};
template <int depth>
class BnComplexFFT2n3{
	
public:
	//static const int length=1<<depth;
	static const int N1=3;
	static const int N2=(1<<depth);
	static const int length=N1*N2;
	
	BnComplex table[N2];
	
	BnComplexFFT2n3(bool inverse=false){
		int i;
		//make sin,cos table
		if (inverse){
			//inverse fft
			for (i=0;i<N2;i++){
				table[i]=BnComplex(cos(i*2*M_PI/N2),-sin(i*2*M_PI/N2));
			}
		}
		else{
			//fft
			for (i=0;i<N2;i++){
				table[i]=BnComplex(cos(i*2*M_PI/N2),sin(i*2*M_PI/N2));
			}
		}
	}
	
	void execute(BnComplex* value){
		
		//BnComplex value[length];
		
		//3 2^n FFT
		//PrimeFactor
		
		//N1 = 3
		//N2 = 2^n
		//N  = N1 x N2
		
		//t1 = 171 ,t2 = 1
		//t1 N1 + t2 N2 = N +1
		
		//const int N1=3;
		//const int N2=(1<<depth);
		
		int i;
		
		const int t1=(((2-(depth&1))*N2)+1)/3;
		const int t2=1+(depth&1);
		//printf ("%d = %d \n",N1*N2+1,t1*N1+t2*N2);
		//printf ("%d = %d \n",t1,t2);
		//const int length=N1*N2;
		
		//BnComplex value[length];
		
		
		
		//make dummy data
		//for (i=0;i<length;i++){
		//	value[i]=BnComplex(cos(19.5*i*2*M_PI/length));
		//}
		
		
		//3 sample fft
		{
			int i1,i2,i3;
			BnComplex tmp1,tmp2,tmp3;
			const BnImaginary W31=sqrt(3)*0.5;
			if (t2==1){
				for (i=0;i<N2;i++){
					i1=(N1*t1*i)%length;
					i2=(i1+N2*t2)%length;
					i3=(i2+N2*t2)%length;
					
					tmp1=value[i2]+value[i3];
					tmp2=value[i1]-tmp1*0.5;
					tmp3=(value[i2]-value[i3])*W31;
					
					value[i2]=tmp2+tmp3;
					value[i3]=tmp2-tmp3;
					value[i1]+=tmp1;
				}
			}
			else{
				for (i=0;i<N2;i++){
					i1=(N1*t1*i)%length;
					i2=(i1+N2*t2)%length;
					i3=(i2+N2*t2)%length;
					
					tmp1=value[i2]+value[i3];
					tmp2=value[i1]-tmp1*0.5;
					tmp3=(value[i3]-value[i2])*W31;
					
					value[i2]=tmp2+tmp3;
					value[i3]=tmp2-tmp3;
					value[i1]+=tmp1;
				}
			}
		}	
		//2^n sample fft
		{
			
			//BnComplex table[N2];
			
			BnComplex tmp;
			int i,j,k,l;
			int subCount;
			int offset;
			int idush;
			int jdush;
			int ioffset;
			int joffset;
			
			//time(sample) culling
			
			for (l=0;l<N1;l++){	
				for (k=0;k<depth;k++){
					offset=N2>>(k+1);
					subCount=1<<k;
					for (j=0;j<subCount;j++){
						joffset=j*offset*2;
						
						//i=0
						{
							idush=joffset;
							ioffset=idush+offset;
							idush=(idush*N1*t1+N2*t2*l)%length;
							ioffset=(ioffset*N1*t1+N2*t2*l)%length;
							tmp=value[idush]+value[ioffset];
							value[ioffset]=value[idush]-value[ioffset];
							value[idush]=tmp;
						}
						for (i=1;i<offset;i++){
							idush=i+joffset;
							ioffset=idush+offset;
							idush=(idush*N1*t1+N2*t2*l)%length;
							ioffset=(ioffset*N1*t1+N2*t2*l)%length;
							tmp=value[idush]+value[ioffset];
							value[ioffset]=(value[idush]-value[ioffset])*table[((i<<k)*t1)%N2];
							value[idush]=tmp;
						}
					}
				}
			}
			
			int n=N2;
			i = 0;
			for (j = 1; j < n - 1; j++) {
				for (k = n >> 1; k > (i ^= k); k >>= 1);
				if (j < i) {
					for (l=0;l<N1;l++){			
						idush=(i*N1*t1+N2*t2*l)%length;
						jdush=(j*N1*t1+N2*t2*l)%length;
						
						tmp = value[jdush];
						value[jdush] = value[idush];
						value[idush] = tmp;
					}
				}
			}
		}
		
		
		//for (i=0;i<length;i++){
		//	printf("%f\t%f\n",value[i].re,value[i].im);
		//}
		
	}
};

template <int depth, bool Inverse = false, typename baseFFT = BnComplexFFT2n<depth-1, Inverse> >
class BnRealFFT{
	baseFFT cFFT;

public:
	static const int length=1<<depth;	
	BnComplex table[length];

	BnRealFFT(){
		int i;
		//make sin,cos table
		if ( not Inverse){
			for (i=0;i<length;i++){
				table[i]=BnComplex(cos(i*2*M_PI/length),sin(i*2*M_PI/length));
			}
		}
		else {
			for (i=0;i<length;i++){
				table[i]=BnComplex(cos(i*2*M_PI/length),-sin(i*2*M_PI/length));
			}
		}
	}
	void execute(double* value){
		BnComplex* cValue = (BnComplex*)value;
		cFFT.execute(cValue);
		const int halfLength = length/2;
		
		for (int i = 1; i<halfLength/2; i++){
			BnComplex c0 = cValue[i];
			BnComplex c1 = cValue[halfLength-i];
			
			cValue[i]=c0-(0.5+BnImaginary(0.5)*table[i])*(c0-c1.conjugate());
			cValue[halfLength-i]=c1-(0.5+BnImaginary(0.5)*table[halfLength-i])*(c1-c0.conjugate());
		}
		{
			BnComplex c0 = cValue[0];
			value[0]=c0.re+c0.im;
			value[1]=c0.re-c0.im;
		}
	}
	
};

template <typename baseFFT>
class BnComplex2DFFT{
	baseFFT FFT;
public:
	void execute(BnComplex* value,unsigned int width){
		int i,j;
		for (i=0;i<FFT.length;i++){
			FFT.execute(value+i*width);
		}
		
		//transpose
		BnComplex tmp;
		for (i=0;i<FFT.length;i++){
			for (j=0;j<i;j++){
				tmp=value[i+j*width];
				value[i+j*width]=value[j+i*width];
				value[j+i*width]=tmp;
			}
		}
		
		for (i=0;i<FFT.length;i++){
			FFT.execute(value+i*width);
		}
		
		//inv transpose(option)
		if (0){
			BnComplex tmp;
			for (i=0;i<FFT.length;i++){
				for (j=0;j<i;j++){
					tmp=value[i+j*width];
					value[i+j*width]=value[j+i*width];
					value[j+i*width]=tmp;
				}
			}
		}
	}
};

//int main (int argc, char * const argv[]) {
//	return 0;
//}
