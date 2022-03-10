#pragma once

#include "ofMain.h"
#include <complex>
#include <iostream>
#include <cmath>
using namespace std;

const complex<double> iCplx(0, 1);

const int N = 512;

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void audioOut(ofSoundBuffer & buffer);
		float integrale(vector <float> &g, float a, float b);
		void transfourier(vector <float> S);
		float syntheseAdditive(float phase);
		float syntheseAdditiveTriangle(float phase);
		float sinusClassique(float phase);

		std::vector<float> freq;
		
		
		ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	a;
		int 	harmonique;
		float	dt;
		int 	fmax;
		int 	choice;	
		int 	n;

		vector <float> signal;
		vector <float> rAudio;
		
		//------------------- for the simple sine wave synthesis
		float 	f;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
};
