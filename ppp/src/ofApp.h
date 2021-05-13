#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ParticleEmitter.h"

typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	glm::vec3 trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
	float width, height;
	glm::mat4 T;
	bool inside(glm::vec3 p);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofVec3f acceleration;
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	bool collision(Sprite s);
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem  {
public:
	glm::vec3 shipPos;
	bool boom = false;
	bool bam = false;
	bool charge = false;
	bool charge2 = false;
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	void collideCheck(SpriteSystem *);
	void collideCheck2(BaseObject *);
	vector<Sprite> sprites;
	ofVec3f explodePos;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter: public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(ofVec3f);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void update();
	SpriteSystem *sys;
	float rate;
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	bool move;
	bool rotating;
	float rotateValue;
	bool shoot;
	bool pew;
	bool random = false;
	bool rando = false;
	glm::vec3 moveDirection;
	glm::vec3 heading;
	bool dragged;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		ofSoundPlayer radio;

		void keyPressed(int key);
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
		
		//vector<Emitter *> emitters;
		//int numEmitters;

		ImpulseRadialForce *radialForce;
		ParticleEmitter emitter;

		Emitter  *turret = new Emitter(new SpriteSystem());
		Emitter  *spawn1 = new Emitter(new SpriteSystem());
		Emitter  *spawn2 = new Emitter(new SpriteSystem());

		ofImage defaultImage;
		ofVec3f mouse_last;
		bool imageLoaded;
		map<int, bool> keymap;
		bool pause;

		bool bHide;
		bool bHide2;

		ofxFloatSlider erate;
		ofxFloatSlider elife;

		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxVec3Slider velocity;
		float speed;
		int score = 0;
		ofxLabel screenSize;


		ofxPanel gui;
		ofxPanel gui2;
};
