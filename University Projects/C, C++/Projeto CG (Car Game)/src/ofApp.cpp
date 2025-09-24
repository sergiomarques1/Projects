#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	inicio = 0;

	ofSetFrameRate(60);
	glEnable(GL_DEPTH_TEST);

	ofSetBackgroundColor(0, 150, 0);

	//mostra o estado inicial das matrizes
	coutModelviewMatrix();
	coutProjectionMatrix();

	glPointSize(200);
	glLineWidth(3);

	imgEstrada.load("estrada.png");
	imgCarro.load("carro.png");
	imgObstaculo1.load("obstaculo1.png");
	imgObstaculo2.load("obstaculo2.png");
	imgObstaculo3.load("obstaculo3.png");
	imgObstaculo4.load("obstaculo4.png");
	imgCeu.load("ceu.jpg");
	imgSol.load("sol.png");
	imgNoite.load("noite.jpg");

	lensAngle = 90;
	alpha = 10;
	beta = 1000;
	perfectDist = (gh() / 2 / tan(lensAngle / 2 * PI / 180));

	velocidade = 10.0;

	opcao = 0;

	dificuldade = 95;

	camera = 0;
	pontuacao = 0;
	tempoDecorrido = 0.;
	tempo = 0.;

	numLinhas = 1000;
	larguraPista = gw() * 0.25;
	unit = 10;
	zReset = 600;

	resetTempo = 0;
	perdeu = 0;

	numCubos = 30;

	explosao = 0;
	setUp = 0;
	setUpExp = 0;
	voltarJogar = 0;
	sair = 0;

	amb = 1;
	dif = 1;
	spec = 1;
	farois = 0;
	sol = 1;
	carro = 0;

	linhasPista.clear();
	obstaculo.clear();
}

//--------------------------------------------------------------
void ofApp::update() {

	if (inicio == 1 && setUp == 1) {

		savePosCarro = posCarro;

		if (resetTempo == 0) {
			ofResetElapsedTimeCounter();
			resetTempo = 1;
		}

		if (perdeu == 0) {

			tempoDecorrido = ofGetElapsedTimeMillis();

			checkColisao();

			if (tempoDecorrido - tempo >= 10000) {
				if (sol == 1) {
					sol = 0;
				}
				else {
					sol = 1;
				}
				tempo = tempoDecorrido;
			}

			pontuacao = static_cast<int>(tempoDecorrido / 150);
			 
			if (pontuacao > recorde) {
				recorde = pontuacao;
			}

			for (int i = 0; i < linhasPista.size(); i++) {
				linhasPista[i].z += velocidade;

				if (linhasPista[i].z > zReset) {
					linhasPista[i].z = linhasPista[linhasPista.size() - 1].z - unit;
					ofVec3f aux = linhasPista[i];
					linhasPista.erase(linhasPista.begin());
					linhasPista.push_back(aux);
				}
			}

			int diferenca = linhasPista.size() - obstaculo.size();

			for (int i = 0; i < obstaculo.size(); i++) {
				obstaculo[i].z += velocidade;

				if (obstaculo[i].z > zReset) {
					obstaculo[i].z = obstaculo[obstaculo.size() - 1].z - unit;
					ofVec3f aux = obstaculo[i];
					obstaculo.erase(obstaculo.begin());

					int corObstaculo = int(ofRandom(0, 4));
					int rand = int(ofRandom(0, 4));
					float posExata;

					if (rand == 0) {	
						posExata = larguraPista / 2.5;
					}
					else if(rand == 1){		
						posExata = larguraPista / 7.5;
					}
					else if (rand == 2) {
						posExata = larguraPista / 4;
					}
					else {
						posExata = larguraPista / 8;
					}

					int valor = int(ofRandom(1, 4));		// centro, direita ou esquerda da pista
					if (valor == 1) {		// esquerda
						ofVec4f posFinal = ofVec4f(linhasPista[diferenca].x - posExata, linhasPista[diferenca].y, linhasPista[diferenca].z, corObstaculo);
						obstaculo.push_back(posFinal);
					}
					else if (valor == 2) {		// centro
						ofVec4f posFinal = ofVec4f(linhasPista[diferenca].x, linhasPista[diferenca].y, linhasPista[diferenca].z, corObstaculo);
						obstaculo.push_back(posFinal);
					}
					else {		// direita
						ofVec3f posFinal = ofVec4f(linhasPista[diferenca].x + posExata, linhasPista[diferenca].y, linhasPista[diferenca].z, corObstaculo);
						obstaculo.push_back(posFinal);
					}

				}
			}

			camPos = linhasPista[0];
			camTarget = linhasPista[60];

			posCarro = linhasPista[25];

			posCarro.x = linhasPista[25].x + (xCarro * larguraPista / GLfloat(xLinha)) + tamanho * 0.5;

		}

		if (explosao == 1) {
			for (int i = 0; i < cubos.size(); i++) {
				cubos[i].pos += cubos[i].velocidade;

				cubos[i].tempo -= ofGetLastFrameTime();

				if (cubos[i].tempo <= 0) {
					cubos.erase(cubos.begin() + i);
					i--;
				}
			}

			if (cubos.empty()) {
				explosao = 0;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	glEnable(GL_TEXTURE);

	if (camera != 0 && inicio == 1) {
		if (sol == 1) {
			drawSol();
			drawCeu();
		}
		drawNoite();
	}

	if (perdeu == 1) {

		carro = 0;
		farois = 0;

		if (setUpExp == 0) {
			setupExplosao();
			setUpExp = 1;
		}

		if (explosao == 0) {
			ofClear(0., 150., 0.);
			glColor3f(1, 1, 1);
			ofDrawBitmapString("== GAME OVER ==", gw() / 2 - 50, gh() / 2 - 30);
			ofDrawBitmapString("Pontuacao: "+ofToString(pontuacao), gw() / 2 - 65, gh() / 2);
			ofDrawBitmapString("Pontuacao Recorde: " + ofToString(recorde), gw() /2 - 65, gh() / 2 + 20);
			ofDrawBitmapString("9 - Novo Jogo ", gw() / 2 - 50, gh() - 60);
			ofDrawBitmapString("0 - Sair ", gw() / 2 - 50, gh() - 40);
		}

		if (sair == 1) {
			ofExit();
		}

		if (voltarJogar == 1) {
			setup();
		}
	}

	if (inicio == 0) {
		glColor3f(1, 1, 1);
		if (dificuldade == 97) {
			stringDif = "Facil";
		}
		else if (dificuldade == 95) {
			stringDif = "Media";
		}
		else {
			stringDif = "Dificil";
		}
		ofDrawBitmapString("Dificuldade: "+ofToString(stringDif), gw() / 2 - 75, 30);
		ofDrawBitmapString("1 - Facil\n2 - Media\n3 - Dificil", gw() / 2 - 50, 50);
		ofDrawBitmapString("CONTROLES DO JOGO\n", gw() / 2 - 75, gh() / 3 - 40);
		ofDrawBitmapString("'d' - direita\n", gw() / 2 - 150, gh() / 3 + 15);
		ofDrawBitmapString("'a' - esquerda\n", gw() / 2 - 150, gh() / 3 + 30);
		ofDrawBitmapString("'w' - acelerar\n", gw() / 2 - 150, gh() / 3 + 45);
		ofDrawBitmapString("'<-|->' - mudar camera\n", gw() / 2 - 150, gh() / 3 + 60);
		ofDrawBitmapString("'j' - ligar/desligar farois\n", gw() / 2 - 150, gh() / 3 + 75);
		ofDrawBitmapString("'k' - ligar/desligar luz carro\n", gw() / 2 - 150, gh() / 3 + 90);
		ofDrawBitmapString("'l' - ligar/desligar luz solar\n", gw() / 2 - 150, gh() / 3 + 105);
		ofDrawBitmapString("'u' - ligar/desligar componente ambiente\n", gw() / 2 - 150, gh() / 3 + 120);
		ofDrawBitmapString("'i' - ligar/desligar componente difusa \n", gw() / 2 - 150, gh() / 3 + 135);
		ofDrawBitmapString("'o' - ligar/desligar componente especular\n", gw() / 2 - 150, gh() / 3 + 150);
		ofDrawBitmapString("Pressione ENTER para jogar!", gw() / 2 - 100, gh() - 210);
	}

	if (inicio == 1) {

		luzSol();

		if (setUp == 0) {
			setupPista();
			setUp = 1;
		}

		if (perdeu == 0) {
			glColor3f(1, 1, 1);
			ofDrawBitmapString("Dificuldade: " + ofToString(stringDif), gw() / 2 - 75, 30);
			ofDrawBitmapString("Pontuacao Atual: " + ofToString(pontuacao), gw() - 250, 30);
			ofDrawBitmapString("Pontuacao Recorde: " + ofToString(recorde), gw() - 250, 60);
		}

		switch (camera) {
		case 0:				// vista de cima
			glViewport(0, 0, gw(), gh());
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-gw() * 0.5, gw() * 0.5, -gh() * 0.5, gh() * 0.5, -10000, 10000);
			lookat(linhasPista[55].x, linhasPista[55].y + 200, linhasPista[55].z, linhasPista[55].x, 0, linhasPista[55].z, 0, 0, -1);

			drawPista();

			break;
		case 1:				// vista de trás do carro 
			glViewport(0, 0, gw(), gh());
			ofSetBackgroundColor(0, 0, 100);
			perspective(lensAngle, alpha, beta);
			lookat(camPos.x, 100, perfectDist + 100, camPos.x, 100, 0, 0, 1, 0);

			drawPista();

			// vista de cima (mapa)
			glViewport(0, 0.75 * gh(), gw() * 0.25, gh() * 0.25);
			ofSetBackgroundColor(0, 150, 0);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-gw() * 0.5, gw() * 0.5, -gh() * 0.5, gh() * 0.5, -10000, 10000);
			lookat(linhasPista[55].x, linhasPista[55].y + 200, linhasPista[55].z, linhasPista[55].x, 0, linhasPista[55].z, 0, 0, -1);

			drawPista();

			break;
		case 2:				// vista 1.ª pessoa
			glViewport(0, 0, gw(), gh());
			perspective(lensAngle, alpha, beta);
			lookat(posCarro.x, posCarro.y + tamanho, posCarro.z, camPos.x, posCarro.y + tamanho, camTarget.z, 0, 1, 0);

			drawPista();
		}
	}
}

void ofApp::setupPista() {


	xLinha = 10;
	tamanho = larguraPista / 10;
	xCarro = 0;

	for (int i = 0; i < numLinhas; i++) {
		ofVec3f aux = ofVec3f(5. * gw(), 0, -i * unit);
		linhasPista.push_back(aux);
		if (i > 45) {
			if (ofRandom(0, 100) > dificuldade) {

				int corObstaculo = int(ofRandom(0, 4));
				int rand = int(ofRandom(0, 4));
				float posExata;

				if (rand == 0) {
					posExata = larguraPista / 2.5;
				}
				else if (rand == 1) {
					posExata = larguraPista / 7.5;
				}
				else if (rand == 2) {
					posExata = larguraPista / 4;
				}
				else {
					posExata = larguraPista / 8;
				}

				int valor = int(ofRandom(1, 4));		// centro, direita ou esquerda da pista
				if (valor == 1) {		// esquerda
					ofVec4f posFinal = ofVec4f(linhasPista[i].x - posExata, linhasPista[i].y, linhasPista[i].z, corObstaculo);
					obstaculo.push_back(posFinal);
				}
				else if (valor == 2) {		// centro
					ofVec4f posFinal = ofVec4f(linhasPista[i].x, linhasPista[i].y, linhasPista[i].z, corObstaculo);
					obstaculo.push_back(posFinal);
				}
				else {		// direita
					ofVec3f posFinal = ofVec4f(linhasPista[i].x + posExata, linhasPista[i].y, linhasPista[i].z, corObstaculo);
					obstaculo.push_back(posFinal);
				}
			}
		}

	}

}

void ofApp::drawPista() {

	if (setUpExp == 1) {
		drawExplosao();
	}

	if (perdeu == 0) {
		drawCarro();
	}

	int indice = 0;

	for (int i = 0; i < linhasPista.size(); i++) {
		if (perdeu == 0 || explosao == 1) {
			glEnable(GL_TEXTURE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPushMatrix();
			glTranslatef(linhasPista[i].x, linhasPista[i].y, linhasPista[i].z);
			glScalef(larguraPista, 1, unit);
			imgEstrada.bind();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			malha_unit(10, 1, 214);
			glPopMatrix();
			imgEstrada.unbind();
			glDisable(GL_TEXTURE);
		}
	}

	for (int i = 0; i < obstaculo.size(); i++) {
		if (perdeu == 0 || explosao == 1) {
			drawObstaculo(obstaculo[i].x, obstaculo[i].y, obstaculo[i].z, obstaculo[i].w);
		}
	}
}

void ofApp::drawCarro() {

	glEnable(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(posCarro.x, posCarro.y + tamanho/2, posCarro.z);
	glScalef(tamanho, tamanho, tamanho);
	imgCarro.bind();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	cube_unit(320);
	glPopMatrix();

	luzFarois();
	luzCarro();
	imgCarro.unbind();
	glDisable(GL_TEXTURE);
}

void ofApp::drawObstaculo(float x, float y, float z, int cor) {

	glEnable(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(x, y + tamanho / 2, z);
	glScalef(tamanho, tamanho, tamanho);
	if (cor == 0) {
		imgObstaculo1.bind();
	}
	else if (cor == 1) {
		imgObstaculo2.bind();
	}
	else if (cor == 2) {
		imgObstaculo3.bind();
	}
	else {
		imgObstaculo4.bind();
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	cube_unit(321);
	glPopMatrix();
	if (cor == 0) {
		imgObstaculo1.unbind();
	}
	else if (cor == 1) {
		imgObstaculo2.unbind();
	}
	else if (cor == 2) {
		imgObstaculo3.unbind();
	}
	else {
		imgObstaculo4.unbind();
	}
	glDisable(GL_TEXTURE);
}

void ofApp::checkColisao() {

	for (int i = 0; i < obstaculo.size(); i++) {

		if (
			posCarro.x + tamanho * 0.5 >= obstaculo[i].x - tamanho * 0.5 &&
			posCarro.x - tamanho * 0.5 < obstaculo[i].x + tamanho * 0.5)
		{
			if (
				posCarro.z - tamanho * 0.5 <= obstaculo[i].z + tamanho * 0.5 &&
				posCarro.z - tamanho * 0.5 > obstaculo[i].z - tamanho * 0.5)
			{
				perdeu = 1;
				explosao = 1;
				savePosCarro = posCarro;
				break;
			}
		}

	}
}

void ofApp::setupExplosao() {

	if (explosao == 1) {
		return;
	}

	explosao = 1;

	for (int i = 0; i < numCubos; i++) {
		CuboExplosao novoCubo;
		novoCubo.pos.x = savePosCarro.x;
		novoCubo.pos.y = savePosCarro.y + (tamanho / 2);
		novoCubo.pos.z = savePosCarro.z + (tamanho / 2);
		novoCubo.velocidade = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
		novoCubo.tempo = ofRandom(1, 3);

		cubos.push_back(novoCubo);
	}
}

void ofApp::drawExplosao() {

	if (explosao == 1 && camera != 2) {
		for (int i = 0; i < cubos.size(); i++) {
			if (cubos[i].tempo > 0) {
				glEnable(GL_TEXTURE);
				glPushMatrix();
				glTranslatef(cubos[i].pos.x, cubos[i].pos.y, cubos[i].pos.z);
				glScalef(tamanho / 5, tamanho / 5, tamanho / 5);
				imgCarro.bind();
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				cube_unit(320);
				glPopMatrix();
				imgCarro.unbind();
				glDisable(GL_TEXTURE);
			}
		}
	}
}

void ofApp::drawCeu() {

	glEnable(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(gw() / 2, gh() / 4, 0);
	glScalef(gw(), gh() / 2, 1);
	imgCeu.bind();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	rectFill_unit(400);
	glPopMatrix();
	imgCeu.unbind();
	glDisable(GL_TEXTURE);
}

void ofApp::drawSol() {

	glEnable(GL_TEXTURE);

	imgSol.bind();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glPushMatrix();
	glTranslatef(gw() / 2, gh() / 5, 0);
	glScalef(gh() / 5, gh() / 5, 1.);

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(0.5, 0.5);
	glVertex3f(0, 0, 0);

	for (int i = 0; i <= 32; i++) {
		float theta = i * 2 * PI / 32.;
		float x = 0.5 * cos(theta);
		float y = 0.5 * sin(theta);
		glTexCoord2f(x + 0.5, 1 - (y + 0.5));
		glVertex3f(x, y, 0);
	}

	glEnd();
	glPopMatrix();

	imgSol.unbind();
	glDisable(GL_TEXTURE);
}

void ofApp::drawNoite() {

	glEnable(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(gw() / 2, gh() / 4, 0);
	glScalef(gw(), gh() / 2, 1);
	imgNoite.bind();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	rectFill_unit(885);
	glPopMatrix();
	imgNoite.unbind();
	glDisable(GL_TEXTURE);

}

void ofApp::luzFarois() {

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	GLfloat angulo = 10.0f;
	posFarol[0] = posCarro.x - (tamanho / 2);
	posFarol[1] = posCarro.y;
	posFarol[2] = posCarro.z;
	posFarol[3] = 1.0;

	dirFarol[0] = 0.0; 
	dirFarol[1] = 0.0;  
	dirFarol[2] = -1.0; 
	dirFarol[3] = 0.0;  

	ambFarol[0] = 0.0;
	ambFarol[1] = 0.0;
	ambFarol[2] = 0.0;
	ambFarol[3] = 0.0;

	if (dif == 1) {
		difFarol[0] = 1.0;
		difFarol[1] = 1.0;
		difFarol[2] = 1.0;
		difFarol[3] = 1.0;
	}
	else {
		difFarol[0] = 0.0;
		difFarol[1] = 0.0;
		difFarol[2] = 0.0;
		difFarol[3] = 0.0;
	}

	if (spec == 1) {
		specFarol[0] = 0.2;
		specFarol[1] = 0.2;
		specFarol[2] = 0.2;
		specFarol[3] = 0.5;
	}
	else {
		specFarol[0] = 0.0;
		specFarol[1] = 0.0;
		specFarol[2] = 0.0;
		specFarol[3] = 0.0;
	}

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, angulo);
	glLightfv(GL_LIGHT1, GL_POSITION, posFarol);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dirFarol);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambFarol);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difFarol);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specFarol);

	posFarol2[0] = posCarro.x + (tamanho / 2);
	posFarol2[1] = posCarro.y;
	posFarol2[2] = posCarro.z;
	posFarol2[3] = 1.0;

	dirFarol2[0] = 0.0f;  
	dirFarol2[1] = 0.0f;  
	dirFarol2[2] = -1.0f; 
	dirFarol2[3] = 0.0f;  

	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, angulo);
	glLightfv(GL_LIGHT2, GL_POSITION, posFarol2);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dirFarol2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambFarol);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, difFarol);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specFarol);


	if (farois == 1) {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
}

void ofApp::luzSol() {

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	posSol[0] = gw() / 2;
	posSol[1] = gh() / 5;
	posSol[2] = 1000;
	posSol[3] = 1.0;

	if (amb == 1) {
		ambSol[0] = 0.25;
		ambSol[1] = 0.25;
		ambSol[2] = 0.25;
		ambSol[3] = 1.0;
	}
	else {
		ambSol[0] = 0.0;
		ambSol[1] = 0.0;
		ambSol[2] = 0.0;
		ambSol[3] = 0.0;
	}

	if (dif == 1) {
		difSol[0] = 0.8;
		difSol[1] = 0.8;
		difSol[2] = 0.8;
		difSol[3] = 1.0;
	}
	else {
		difSol[0] = 0.0;
		difSol[1] = 0.0;
		difSol[2] = 0.0;
		difSol[3] = 0.0;
	}

	if (spec == 1) {
		specSol[0] = 0.0;
		specSol[1] = 0.0;
		specSol[2] = 0.0;
		specSol[3] = 1.0;
	}
	else {
		specSol[0] = 0.0;
		specSol[1] = 0.0;
		specSol[2] = 0.0;
		specSol[3] = 0.0;
	}

	glLightfv(GL_LIGHT3, GL_POSITION, posSol);
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambSol);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, difSol);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specSol);


	if (sol == 1) {
		glEnable(GL_LIGHT3);
	}
	else {
		glDisable(GL_LIGHT3);
	}
}

void ofApp::luzCarro() {

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	posLuzCarro[0] = posCarro.x;                   
	posLuzCarro[1] = posCarro.y + 1000;   
	posLuzCarro[2] = posCarro.z;                  
	posLuzCarro[3] = 1.0;                         

	ambLuzCarro[0] = 0.0;
	ambLuzCarro[1] = 0.0;
	ambLuzCarro[2] = 0.0;
	ambLuzCarro[3] = 0.0;

	if (dif == 1) {
		difLuzCarro[0] = 1.0;  
		difLuzCarro[1] = 1.0;
		difLuzCarro[2] = 1.0;
		difLuzCarro[3] = 1.0;
	}
	else {
		difLuzCarro[0] = 0.0;
		difLuzCarro[1] = 0.0;
		difLuzCarro[2] = 0.0;
		difLuzCarro[3] = 0.0;
	}

	if (spec == 1) {
		specLuzCarro[0] = 0.0; 
		specLuzCarro[1] = 0.0;
		specLuzCarro[2] = 0.0;
		specLuzCarro[3] = 1.0;
	}
	else {
		specLuzCarro[0] = 0.0;
		specLuzCarro[1] = 0.0;
		specLuzCarro[2] = 0.0;
		specLuzCarro[3] = 0.0;
	}

	glLightfv(GL_LIGHT4, GL_POSITION, posLuzCarro);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, dirLuzCarro);
	glLightfv(GL_LIGHT4, GL_AMBIENT, ambLuzCarro);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, difLuzCarro);
	glLightfv(GL_LIGHT4, GL_SPECULAR, specLuzCarro);

	if (carro == 1) {
		glEnable(GL_LIGHT4);
	}
	else {
		glDisable(GL_LIGHT4);
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key) {
	case '1':
		if (inicio == 0) {
			dificuldade = 97;
		}
		break;
	case '2':
		if (inicio == 0) {
			dificuldade = 95;
		}
		break;
	case '3':
		if (inicio == 0) {
			dificuldade = 90;
			velocidade = 17.5;
		}
		break;
	case OF_KEY_RETURN:
		inicio = 1;
		break;
	case 'w':
		velocidade = velocidade + 0.1;
		break;
	case 'a':		// esquerda
		xCarro--;
		if (xCarro < -xLinha/2) {
			xCarro = -xLinha/2;
		}
		break;
	case 'd':		// direita
		xCarro++;
		if (xCarro >= xLinha/2) {
			xCarro = xLinha/2 - 1;
		}
		break;
	case OF_KEY_RIGHT:
		camera++;
		if (camera > 2) {
			camera = 0;
		}
		break;
	case OF_KEY_LEFT:
		camera--;
		if (camera < 0) {
			camera = 2;
		}
		break;
	case 'j':
		if (farois == 1) {
			farois = 0;
		}
		else {
			farois = 1;
		}
		break;
	case 'k':
		if (carro == 1) {
			carro = 0;
		}
		else {
			carro = 1;
		}
		break;
	case 'l':
		if (sol == 1) {
			sol = 0;
		}
		else {
			sol = 1;
		}
		break;
	case 'u':
		if (amb == 1) {
			amb = 0;
		}
		else {
			amb = 1;
		}
		break;
	case 'i':
		if (dif == 1) {
			dif = 0;
		}
		else {
			dif = 1;
		}
		break;
	case 'o':
		if (spec == 1) {
			spec = 0;
		}
		else {
			spec = 1;
		}
		break;
	case '9':
		if (perdeu == 1 && explosao == 0) {
			voltarJogar = 1;
		}
		break;
	case '0':
		if (perdeu == 1 && explosao == 0) {
			sair = 1;
		}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	cout << endl << x << " " << y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
