#include "ofApp.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------------
//
// This example code demonstrates the use of an "Emitter" class to emit Sprites
// and set them in motion. The concept of an "Emitter" is taken from particle
// systems (which we will cover next week).
//
// The Sprite class has also been upgraded to include lifespan, velocity and age
// members.   The emitter can control rate of emission and the current velocity
// of the particles. In this example, there is no acceleration or physics, the
// sprites just move simple frame-based animation.
//
// The code shows a way to attach images to the sprites and optional the
// emitter (which is a point source) can also have an image.  If there are
// no images attached, a placeholder rectangle is drawn.
// Emitters  can be placed anywhere in the window. In this example, you can drag
// it around with the mouse.
//
// OF has an add-in called ofxGUI which is a very simple UI that is useful for
// creating sliders, buttons and fields. It is not recommended for commercial 
// game development, but it is useful for testing.  The "h" key will hide the GUI
// 
// If you want to run this example, you need to use the ofxGUI add-in in your
// setup.
//
//
//  Kevin M. Smith - CS 134 SJSU

BaseObject::BaseObject() {
	trans = ofVec3f(0,0,0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}

bool BaseObject::inside(glm::vec3 p) {
	if (p.x < -width / 2.0 || p.x > width / 2.0 || p.y < -height / 2.0 || p.y > height / 2.0)
		return false;
	else
		return true;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 10;
	height = 100;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(trans));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
		T = translate * rotation;
		ofPushMatrix();
		ofMultMatrix(T);
		image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
		ofPopMatrix();
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(trans));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
		T = translate * rotation;
		ofPushMatrix();
		ofMultMatrix(T);
		ofDrawRectangle(-width / 2.0, -height / 2.0, width, height);
		ofPopMatrix();
	}
}

bool Sprite::collision(Sprite s) {
	/*if (s.inside(glm::inverse(s.T) * glm::vec4(trans.x-width, trans.y - height, 1, 1)) ||
		s.inside(glm::inverse(s.T) * glm::vec4(trans.x - width, trans.y + height, 1, 1)) || 
		s.inside(glm::inverse(s.T) * glm::vec4(trans.x + width, trans.y + height, 1, 1)) || 
		s.inside(glm::inverse(s.T) * glm::vec4(trans.x + width, trans.y - height, 1, 1)))
		return true;*/
	if (s.inside(glm::inverse(s.T) * glm::vec4(trans.x, trans.y, 1, 1)))
		return true;
	return false;
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		if (charge) {
			if (sprites[i].age() > 350 && sprites[i].age() < 1500)
				sprites[i].velocity *= 0;
			if (sprites[i].age() > 2000)
				sprites[i].velocity += sprites[i].acceleration;
		}
		sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

void SpriteSystem::collideCheck(SpriteSystem *enemy) {
	for (int i = 0; i < sprites.size(); i++) {
		for (int j = 0; j < enemy->sprites.size(); j++) {
			if (sprites[i].collision(enemy->sprites[j])) {
				sprites[i].lifespan = 0;
				enemy->sprites[j].lifespan = 0;
				boom = true;
				//cout << "pop" << endl;
				break;
			}
		}
	}
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;
	move = false;
	shoot = false;
	pew = false;
	rotating = false;

	lastSpawned = 0;
	rate = 3;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(0, 200, 0);
	drawable = false;
	width = 50;
	height = 50;
	trans = glm::vec3(0, 0,1);
	rot = 0;
}


//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {
	if (drawable) {

		if (haveImage) {
			ofSetColor(200, 200, 200);
			glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(trans));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(rot+180), glm::vec3(0, 0, 1));
			T = translate * rotation;
			ofPushMatrix();
			ofMultMatrix(T);
			image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
			ofPopMatrix();
		}
		else {
			ofSetColor(0, 0, 200);
			glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(trans));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0,0,1));
			T = translate * rotation;
			ofPushMatrix();
			ofMultMatrix(T);
			ofDrawRectangle(-width/2.0 , -height/2.0, width, height);
			ofPopMatrix();
		}
	}

	// draw sprite system
	//
	sys->draw();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

	if(velocity.x < 0)
		rot = velocity.angle(ofVec3f(0, 1, 0));
	else
		rot = velocity.angle(ofVec3f(0, 1, 0))*-1;

	float time = ofGetElapsedTimeMillis();
	if (shoot == true && (time - lastSpawned) > (1000.0/rate)) {
		if (random) {
			trans.x = rand() % ofGetWindowWidth();
		}
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.acceleration = velocity.getNormalized()*5;
		sprite.rot = rot;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans);
		sprite.birthtime = time;
		sys->add(sprite);
		lastSpawned = time;
		pew = true;
	}

	sys->update();
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}


//--------------------------------------------------------------
void ofApp::setup(){

	pause = true;
	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/turret.png")) {
		imageLoaded = true;
		turret->setImage(defaultImage);
	}
	else {
		ofLogFatalError("can't load image: images/turret.png");
		ofExit();
	}

	if (defaultImage.load("images/bullet.png")) {
		imageLoaded = true;
		turret->setChildImage(defaultImage);
	}
	else {
		ofLogFatalError("can't load image: images/bullet.png");
		ofExit();
	}

	if (defaultImage.load("images/enemy1.png")) {
		imageLoaded = true;
		spawn1->setChildImage(defaultImage);
	}
	else {
		ofLogFatalError("can't load image: images/enemy1.png");
		ofExit();
	}

	// create an array of emitters and set their position;
	//
	

	//turret = new Emitter(new SpriteSystem());
    //turret = new Emitter();

	turret->setPosition(ofVec3f(ofGetWindowWidth()/2.0, ofGetWindowHeight()/2.0, 0));
	turret->drawable = true; 
	//turret->setChildImage(defaultImage);
	turret->start();

	//spawn enemies off screen
	spawn1->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, -50, 0));
	spawn1->setLifespan(7000);
	spawn1->sys->charge = true;
	spawn1->random = true;
	spawn1->shoot = true;


	
	gui.setup();
	gui.add(rate.setup("rate", 2, 0, 10));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, -700, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	bHide = true;

	gui2.setup();
	gui2.add(erate.setup("Enemy rate", 1, 0, 3));
	gui2.add(elife.setup("Enemy life", 7, .1, 10));
	bHide2 = true;

}

//--------------------------------------------------------------
void ofApp::update() {
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 

	spawn1->setRate(erate);
	spawn1->setLifespan(elife * 1000);

	turret->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
	turret->update();

	spawn1->update();

	turret->sys->collideCheck(spawn1->sys);

	if (turret->move) {
		glm::vec3 newPosition = turret->trans + turret->moveDirection;
		if(newPosition.x > 0 && newPosition.x < ofGetWindowWidth() && newPosition.y > 0 && newPosition.y < ofGetWindowHeight())
			turret->setPosition(newPosition);
	}

	if (turret->rotating) {
		float rotator = turret->rot;
		rotator += turret->rotateValue;
		glm::vec3 heading = glm::rotateZ(glm::vec3(0, 1, 0), glm::radians(rotator));
		speed = sqrt(pow(velocity->x, 2) + pow(velocity->y, 2));
		velocity = heading * speed;
	}

	if (turret->pew)
	{
		turret->pew = false;
		radio.load("sounds/laser.mp3");
		radio.play();
	}

	if (turret->sys->boom)
	{
		turret->sys->boom = false;
		score += 100;
		radio.load("sounds/boom.wav");
		radio.play();
	}
}


//--------------------------------------------------------------
void ofApp::draw(){
	if (pause)
	{
		const char *text =
			"Controls \n"
			"Movement: Arrow keys or mouse \n"
			"Rotate turret: z and x \n"
			"Shoot: space bar \n"
			"Turret Slider bar: h\n"
			"Enemy Slider bar: j\n"
			"Press spacebar to start\n";
		ofDrawBitmapString(text, ofGetWindowWidth() *.25, ofGetWindowHeight() *.25);
	}
	else
	{
		turret->draw();
		spawn1->draw();

		if (!bHide) {
			gui.draw();
		}
		if (!bHide2) {
			gui2.draw();
		}
	}

	std::string s = "Score: " + std::to_string(score);
	ofDrawBitmapString(s, ofGetWindowWidth() *.83, 10);
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	glm::vec3 newPosition = glm::vec3(x, y, 1);
	if (turret->dragged && newPosition.x > 0 && newPosition.x < ofGetWindowWidth() && newPosition.y > 0 && newPosition.y < ofGetWindowHeight()) {
		turret->setPosition(newPosition);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	//
	if (turret->inside(glm::inverse(turret->T) * glm::vec4(x, y, 1, 1)))
		turret->dragged = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	turret->dragged = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	keymap[key] = true;
	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'Z':
	case 'z':
		turret->rotating = true;
		turret->rotateValue = -2;
		break;
	case 'X':
	case 'x':
		turret->rotating = true;
		turret->rotateValue = 2;
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'J':
	case 'j':
		bHide2 = !bHide2;
		break;
	case 'r':
		break;
	case 's':
		break;
	case 'u':
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_LEFT:
		turret->move = true;
		turret->moveDirection = glm::vec3(-5, 0, 0);
		break;
	case OF_KEY_RIGHT:
		turret->move = true;
		turret->moveDirection = glm::vec3(5, 0, 0);
		break;
	case OF_KEY_UP:
		turret->move = true;
		turret->moveDirection = glm::vec3(0, -5, 0);
		break;
	case OF_KEY_DOWN:
		turret->move = true;
		turret->moveDirection = glm::vec3(0, 5, 0);
		break;
	case 32:
		if (pause == true) {
			pause = false;
			spawn1->start();
		}
		else
			turret->shoot = true;
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
	switch (key) {
	case OF_KEY_LEFT:
		if(!(keymap[OF_KEY_RIGHT] || keymap[OF_KEY_UP] || keymap[OF_KEY_DOWN]))
			turret->move = false;
		break;
	case OF_KEY_RIGHT:
		if (!(keymap[OF_KEY_LEFT] || keymap[OF_KEY_UP] || keymap[OF_KEY_DOWN]))
			turret->move = false;
		break;
	case OF_KEY_UP:
		if (!(keymap[OF_KEY_RIGHT] || keymap[OF_KEY_LEFT] || keymap[OF_KEY_DOWN]))
			turret->move = false;
		break;
	case OF_KEY_DOWN:
		if (!(keymap[OF_KEY_RIGHT] || keymap[OF_KEY_UP] || keymap[OF_KEY_LEFT]))
			turret->move = false;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case 32:
		turret->shoot = false;
		break;
	case 'X':
	case 'x':
		if (!(keymap['Z'] || keymap['z']))
			turret->rotating = false;
		break;
	case 'Z':
	case 'z':
		if (!(keymap['X'] || keymap['x']))
			turret->rotating = false;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

