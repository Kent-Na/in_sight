//
//  DcVector+GL.h
//  Dc
//
//  Created by Nakajima Kent on 12/02/23.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef Dc_DcVector_GL_h
#define Dc_DcVector_GL_h

inline void DcVertex(const DcTvector &Point){
	glVertex2fv((DcFloat*)&Point);
}
inline void DcVertex(const DcVector &Point){
	glVertex3fv((DcFloat*)&Point);
}
inline void DcTexCoord(const DcTvector &Point){
	glTexCoord2f(Point.x,Point.y);
}
inline void DcNormal(const DcVector &Point){
	glNormal3f(Point.x,Point.y,Point.z);
}
inline void DcColor(const DcVector &Color){
	glColor3f(Color.x,Color.y,Color.z);
}
inline void DcColor(const DcVector &Color, const DcFloat alpha){
	glColor3f(Color.x,Color.y,Color.z);
}


#endif
