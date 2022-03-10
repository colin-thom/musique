#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	a					= 0.1f;											// A
	bNoise 				= false;
	harmonique			= 50;
	dt 					= 1.0/22050.0;
	fmax 				= 22050;
	choice 				= 0;
	f 					= 523.3;
	n 					= 0;

	

	signal.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	freq.assign(bufferSize, 0.0);
	
	soundStream.printDeviceList();

	ofSoundStreamSettings settings;

	// if you want to set the device id to be different than the default:
	//
	//	auto devices = soundStream.getDeviceList();
	//	settings.setOutDevice(devices[3]);

	// you can also get devices for an specific api:
	//
	//	auto devices = soundStream.getDeviceList(ofSoundDevice::Api::PULSE);
	//	settings.setOutDevice(devices[0]);

	// or get the default device for an specific api:
	//
	// settings.api = ofSoundDevice::Api::PULSE;

	// or by name:
	//
	//	auto devices = soundStream.getMatchingDevices("default");
	//	if(!devices.empty()){
	//		settings.setOutDevice(devices[0]);
	//	}

#ifdef TARGET_LINUX
	// Latest linux versions default to the HDMI output
	// this usually fixes that. Also check the list of available
	// devices if sound doesn't work
	auto devices = soundStream.getMatchingDevices("default");
	if(!devices.empty()){
		settings.setOutDevice(devices[0]);
	}
#endif

	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);

	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("rAudio", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < signal.size(); i++){
				float x =  ofMap(i, 0, signal.size(), 0, 900, true);
				ofVertex(x, 100 -signal[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("transformée de fourier", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
		transfourier(signal);
					
			ofBeginShape();
			for (unsigned int i = 0; i < freq.size(); i++){
				float x =  ofMap(i, 0, freq.size(), 0, 900, true);
				ofVertex(x, 100 -freq[i]*50000.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
		
	ofSetColor(225);
	string reportString = "a: ("+ofToString(a, 2)+") modify with -/+ keys\npan: ("+ofToString(pan, 2)+") modify with mouse x\nsynthesis: ";
	if( !bNoise ){
		reportString += "sine wave (" + ofToString(f, 2) + "hz) modify with mouse y";
	}else{
		reportString += "noise";	
	}
	ofDrawBitmapString(reportString, 32, 579);

}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){

	if (key == '-' || key == '_' ){
		a -= 0.05;
		a = MAX(a, 0);
	} else if (key == '+' || key == '=' ){
		a += 0.05;
		a = MIN(a, 1);
	}
	
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}

	if ( key == 'p'){
		harmonique ++;
	}
	if ( key == 'm'){
		harmonique --;
	} 

	if(key == '&') choice = 0;
    if(key == 'a') choice = 1;
    if(key == '"') choice = 2;

	if ( key == 'w'){
	f = 32.70 * pow(2, n); //do
	} else if (key == 'x'){
		f = 36.71 * pow(2, n); //ré
	} else if (key == 'c') {
		f = 41.20 * pow(2, n); //mi
	} else if (key == 'v') {
		f = 43.65 * pow(2, n); //fa
	} else if (key == 'b') {
		f = 49.00 * pow(2, n); //sol
	} else if (key == 'n') {
		f = 55.00 * pow(2, n); //la
	} else if (key == ',') {
		f = 61.74 * pow(2, n); //si
	} 

	if (key == '!'){
		n++;
	}
	if (key == '*'){
		n--;
	}
	
	phaseAdderTarget = (f / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	f = 22050.0f * heightPct; 									// plage de fréquence (f)
			// 2 pi f t (car t = 1/samplerate)
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	bNoise = true;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bNoise = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

float ofApp::integrale(vector <float> &g, float a, float b)
{
	float dx = 1.0/fmax;
	float somme = 0;
	for (int i = 0; i < N - 1; i++)
	{
		somme += g[i];
	}
	return dx * somme;
}

void ofApp::transfourier(vector <float> S)
{
	for (int jf = 0; jf < 512 ; jf++) {
		float f = (jf*fmax) / 512;
		complex<double> somme = 0;
		for (int jt = 0; jt < 512; jt++){
			float t = jt * dt;
			somme += (double) S[jt] * exp(-2.0 * PI * f * t * iCplx);
		}
		freq[jf] = dt * abs(somme);
	}
}
float ofApp::sinusClassique(float phase) {
	return sin(phase);
}

float ofApp::syntheseAdditive(float phase) {
	float somme = 0;
	for (int k = 0; k < harmonique; k++) {
		somme += (sin((2 * k + 1) * phase)) / (float) (2 * k + 1);
	}
	return 4.0 / PI * somme; 
}

float ofApp::syntheseAdditiveTriangle(float phase) {
	float somme = 0;
	for (int k = 1; k < harmonique; k++) {
		somme += -1 * ((pow(-1, k)) * (sin(k * phase) / k));
	}
	return 2.0 / PI * somme; 
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

	if ( bNoise == true){
		// ---------------------- noise --------------
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			signal[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * a * leftScale;
			rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ofRandom(0, 1) * a * rightScale;
		}
	} else {
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			//phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
			phase += phaseAdderTarget;
			float sample = 0;
			switch(choice) {
                case 0: sample = sinusClassique(phase); break;
                case 1: sample = syntheseAdditive(phase); break;
                default: sample = syntheseAdditiveTriangle(phase); break;
            }
			signal[i] = buffer[i*buffer.getNumChannels()    ] = sample * a * leftScale;	
			// rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ;
		}
		float integral = integrale(signal, 0, 512);
		printf("%f\n", integral);
	}

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
