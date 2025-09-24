#pragma once

#include "ofMain.h"
#include "cg_extras.h"
#include "cg_drawing_extras.h"
#include "cg_cam_extras.h"


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void setupPista();
	void drawPista();
	void drawCarro();
	void drawObstaculo(float x, float y, float z, int cor);
	void drawExplosao();
	void checkColisao();
	void setupExplosao();
	void drawCeu();
	void drawSol();
	void drawNoite();
	void luzFarois();
	void luzSol();
	void luzCarro();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	int inicio;

	int camera;
	GLfloat lensAngle;
	GLfloat alpha, beta;
	GLfloat perfectDist;
	ofVec3f camPos;
	ofVec3f lookatPos;
	ofVec3f camUp;
	ofVec3f camTarget;

	ofTrueTypeFont fonte;

	int pontuacao;
	int recorde = 0; 

	float tempoDecorrido;
	float tempo;

	// variáveis para controlar o movimento da pista
	float velocidade;
	vector<ofVec3f> linhasPista;  // vetor de linhas de quadrados que formam a pista
	int numLinhas;		// número de linhas na pista
	int unit;
	int larguraPista;

	float zReset;

	GLfloat tamanho;
	GLint xCarro, yCarro;
	ofVec3f posCarro;

	GLfloat larguraLinha, alturaLinha;
	GLint xLinha, yLinha;

	vector<ofVec4f> obstaculo;

	int opcao;

	int dificuldade;
	std::string stringDif;

	string escolhaDificuldade;

	int resetTempo;

	int perdeu;

	int novoJogo;

	struct CuboExplosao {
		ofVec3f pos;
		ofVec3f velocidade;
		float tempo;
	};

	vector<CuboExplosao> cubos;

	int numCubos;

	int explosao;

	int voltarJogar;

	int sair;

	int setUp;
	int setUpExp;

	int amb, dif, spec;
	int farois;
	int sol;
	int carro;

	ofVec3f savePosCarro;

	ofImage imgEstrada;
	ofImage imgCarro;
	ofImage imgObstaculo1;
	ofImage imgObstaculo2;
	ofImage imgObstaculo3;
	ofImage imgObstaculo4;
	ofImage imgCeu;
	ofImage imgSol;
	ofImage imgNoite;

	GLfloat posFarol[4];
	GLfloat dirFarol[4];
	GLfloat ambFarol[4];
	GLfloat difFarol[4];
	GLfloat specFarol[4];

	GLfloat posFarol2[4];
	GLfloat dirFarol2[4];

	GLfloat posSol[4];
	GLfloat ambSol[4];
	GLfloat difSol[4];
	GLfloat specSol[4];

	GLfloat posLuzCarro[4];
	GLfloat dirLuzCarro[4];
	GLfloat ambLuzCarro[4];
	GLfloat difLuzCarro[4];
	GLfloat specLuzCarro[4];

};
