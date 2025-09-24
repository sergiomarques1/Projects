#pragma once

#include "ofMain.h"
#include "cg_extras.h"


//desenha ponto 3D na origem
inline void drawPoint() {
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();
}

//desenha axis 3D
inline void axis3d() {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glColor3f(1, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();
}

//função que desenha quadrado unitário 
//centrado na origem e preenchido
inline void rectFill_unit(GLint size) {
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3d(-0.5, -0.5, 0.);
	glTexCoord2f(0, size);
	glVertex3d(-0.5, 0.5, 0.);
	glTexCoord2f(size, size);
	glVertex3d(0.5, 0.5, 0.);
	glTexCoord2f(size, 0);
	glVertex3d(0.5, -0.5, 0.);
	glEnd();
}

//função que desenha malha unitária com resolução mxn
inline void malha_unit(GLint m, GLint n, GLint size) {
	GLfloat x_start = -0.5;
	GLfloat y_start = -0.5;
	GLfloat x_step = 1.0 / GLfloat(m);
	GLfloat y_step = 1.0 / GLfloat(n);
	GLfloat imgX = size / GLfloat(m);
	GLfloat imgY = size / GLfloat(n);
	glBegin(GL_QUADS);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			/*GLfloat x_min = i * imgX;
			GLfloat x_max = (i + 1) * imgX;
			GLfloat y_min = j * imgY;
			GLfloat y_max = (j + 1) * imgY;*/
			//glTexCoord2f(x_min, y_min);
			glTexCoord2f(size, size);
			glVertex3d(i * x_step + x_start, 0, j * y_step + y_start);
			//glTexCoord2f(x_min, y_max);
			glTexCoord2f(size, 0);
			glVertex3d(i * x_step + x_start, 0, (j + 1) * y_step + y_start);
			//glTexCoord2f(x_max, y_max);
			glTexCoord2f(0, 0);
			glVertex3d((i + 1) * x_step + x_start, 0, (j + 1) * y_step + y_start);
			//glTexCoord2f(x_max, y_min);
			glTexCoord2f(0, size);
			glVertex3d((i + 1) * x_step + x_start, 0, j * y_step + y_start);
		}
	}
	glEnd();
}


inline void cube_unit(GLint size) {
	GLfloat p = 0.5;
	glBegin(GL_QUADS);

	//frente
	//glColor3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-p, -p, p);
	glTexCoord2f(size, 0);
	glVertex3f(-p, p, p);
	glTexCoord2f(size, size);
	glVertex3f(p, p, p);
	glTexCoord2f(0, size);
	glVertex3f(p, -p, p);

	//tras
	//glColor3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-p, -p, -p);
	glTexCoord2f(size, 0);
	glVertex3f(p, -p, -p);
	glTexCoord2f(size, size);
	glVertex3f(p, p, -p);
	glTexCoord2f(0, size);
	glVertex3f(-p, p, -p);

	//glColor3f(1, 1, 1);
	//cima
	glTexCoord2f(0, 0);
	glVertex3f(-p, -p, -p);
	glTexCoord2f(size, 0);
	glVertex3f(-p, -p, p);
	glTexCoord2f(size, size);
	glVertex3f(p, -p, p);
	glTexCoord2f(0, size);
	glVertex3f(p, -p, -p);

	//baixo
	glTexCoord2f(0, 0);
	glVertex3f(-p, p, p);
	glTexCoord2f(size, 0);
	glVertex3f(-p, p, -p);
	glTexCoord2f(size, size);
	glVertex3f(p, p, -p);
	glTexCoord2f(0, size);
	glVertex3f(p, p, p);

	//esq
	glTexCoord2f(0, 0);
	glVertex3f(-p, -p, p);
	glTexCoord2f(size, 0);
	glVertex3f(-p, -p, -p);
	glTexCoord2f(size, size);
	glVertex3f(-p, p, -p);
	glTexCoord2f(0, size);
	glVertex3f(-p, p, p);

	//dir
	glTexCoord2f(0, 0);
	glVertex3f(p, -p, p);
	glTexCoord2f(size, 0);
	glVertex3f(p, p, p);
	glTexCoord2f(size, size);
	glVertex3f(p, p, -p);
	glTexCoord2f(0, size);
	glVertex3f(p, -p, -p);

	glEnd();

}
