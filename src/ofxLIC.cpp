//
//  ofxLIC.cpp
//  Created by lukasz karluk on 22/03/11.
//

#include "ofxLIC.h"

//------------------------------------------------
ofxLICVectorField::ofxLICVectorField() {
    width = 0;
    height = 0;
}

ofxLICVectorField::~ofxLICVectorField() {
    //
}

void ofxLICVectorField::setup(int w, int h) {
    width = w;
    height = h;
    size = width * height;
    vecs.resize(size, ofVec2f());
}

void ofxLICVectorField::setVector(float x, float y, ofVec2f & vec) {
    int xInt, yInt;
    xInt = ofClamp(x, 0, width-1);
    yInt = ofClamp(y, 0, height-1);
    int i = yInt * width + xInt;
    
    vecs[i].set(vec);
}

void ofxLICVectorField::setVector(vector<ofVec2f> & vecsToCopy) {
    vecs = vecsToCopy;
    vecs.resize(size, ofVec2f());
}

ofVec2f & ofxLICVectorField::getVector(ofVec2f & p) {
    return getVector(p.x, p.y);
}

ofVec2f & ofxLICVectorField::getVector(float x, float y) {
    int xInt, yInt;
    xInt = ofClamp(x, 0, width-1);
    yInt = ofClamp(y, 0, height-1);
    int i = yInt * width + xInt;
    
    vecValue.set(vecs[i]);
    
    return vecValue;
}

int ofxLICVectorField::getWidth() {
    return width;
}

int ofxLICVectorField::getHeight() {
    return height;
}

//------------------------------------------------
ofxLIC::ofxLIC() {
    
    vecField = NULL;
    bVecFieldExternal = false;
    
    
    setPosition(0, 0);
    setStep(1.0);
    
    stepNum = 10;
    pulsePhase = 0;
    pulseSpeed = 0.3;
    colorPhase = 0;
    colorSpeed = 0.01;
    dotSize = 5;
    stripWidth = 3;
}

ofxLIC::~ofxLIC() {
    if(vecField != NULL && bVecFieldExternal == false) {
        delete vecField;
        vecField = NULL;
    }
}

//------------------------------------------------
void ofxLIC::setup(int w, int h) {
    vecField = new ofxLICVectorField();
    vecField->setup(w, h);
    bVecFieldExternal = false;
}

void ofxLIC::setVectorField(ofxLICVectorField * vecField) {
    this->vecField = vecField;
    bVecFieldExternal = true;
}

void ofxLIC::setVector(float x, float y, ofVec2f & vec) {
    vecField->setVector(x, y, vec);
}

void ofxLIC::setPosition(float x, float y) {
    pos.x = x;
    pos.y = y;
}

void ofxLIC::setStep(float step) {
    this->step = step;
    dotSize = step * 0.5;
}

//////////////////////////////////////////////////
//  UPDATE.
//////////////////////////////////////////////////

void ofxLIC::update() {
    ofVec2f v;
    int s = stepNum;
    int t = s * 2 + 1;
    
    points.clear();
    points.assign(t, ofVec2f());

    points[s].x = pos.x;          // center point.
    points[s].y = pos.y;
    
    v = pos;
    for(int i=0; i<s; i++) {        // move backwards.
        RK(v, -step);
        
        int j = s - 1 - i;
        points[j].x = v.x;
        points[j].y = v.y;
    }
    
    v = pos;
    for(int i=0; i<s; i++) {      // move forwards.
        RK(v, step);
        
        int j = s + 1 + i;
        points[j].x = v.x;
        points[j].y = v.y;
    }
}

void ofxLIC::RK(ofVec2f & p, double h) {
    ofVec2f v;
    ofVec2f v2;
    ofVec2f k1, k2, k3, k4;
    
    v = vecField->getVector(p);
    v.normalize();
    
    k1 = v * h;
    v2 = p + k1 * 0.5;
    v = vecField->getVector(v2);
    v.normalize();
    
    k2 = v * h;
    v2 = p + k2 * 0.5;
    v = vecField->getVector(v2);
    v.normalize();
    
    k3 = v * h;
    v2 = p + k3;
    v = vecField->getVector(v2);
    v.normalize();
    
    k4 = v * h;
    p += k1/6 + k2/3 + k3/3 + k4/6;
}

//////////////////////////////////////////////////
//  DRAW.
//////////////////////////////////////////////////

void ofxLIC::drawLines(bool bSmooth) {
    if(bSmooth) {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
    }
    
    int t = points.size();
    
    float* vert = new float[t * 3];
    float* colr = new float[t * 4];
    
    for(int i=0; i<t; i++) {
        vert[i * 3 + 0] = points[i].x;
        vert[i * 3 + 1] = points[i].y;
        vert[i * 3 + 2] = 0;
        
        colr[i * 4 + 0] = 0;
        colr[i * 4 + 1] = 0;
        colr[i * 4 + 2] = 0;
        colr[i * 4 + 3] = 255;
    }
    
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, colr);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vert);
    
    glDrawArrays(GL_LINE_STRIP, 0, t);
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    delete[] vert;
    delete[] colr;
}

void ofxLIC::drawStrip(bool bSmooth) {
    if(bSmooth) {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
    }
    
    int t = points.size();
    
    float* vert = new float[t * 3 * 2];
    float* colr = new float[t * 4 * 2];

    float* vert_line = NULL;
    float* colr_line = NULL;
    
    if(bSmooth) {
        vert_line = new float[t * 3 * 2];
        colr_line = new float[t * 4 * 2];
    }
    
    int j, k;
    
    for(int i=0; i<t; i++) {
        float s;
        
        //--- envelope.
        
        float e;
        e = sin((i/(float)(t - 1)) * PI);
        
        //--- pulse phase.
        
        float p;
        p = sin((ofGetFrameNum() + i) * pulseSpeed + pulsePhase);
        p += 1;
        p *= 0.5;

        s = 0.8;      // trucate.
        p *= s;
        p += (1 - s) * 0.5;
        
        //--- color phase.
        
        float c;
        c = sin((ofGetFrameNum() - i) * colorSpeed + colorPhase);
        c += 1;
        c *= 0.5;
        
        s = 0.6;      // trucate.
        c *= s;
        c += (1 - s) * 0.5;
        
         //--- points.
        
        j = MAX(i - 1, 0);
        
        ofVec2f p1 = ofVec2f(points[i]);
        ofVec2f p2 = ofVec2f(points[j]);
        
        ofVec2f v = p1 - p2;
        v.perpendicular();
        v.normalize();
        v *= stripWidth * e * p;
        
        vert[i * 3 * 2 + 0] = points[i].x + v.x;
        vert[i * 3 * 2 + 1] = points[i].y + v.y;
        vert[i * 3 * 2 + 2] = 0;
        vert[i * 3 * 2 + 3] = points[i].x - v.x;
        vert[i * 3 * 2 + 4] = points[i].y - v.y;
        vert[i * 3 * 2 + 5] = 0;
        
        colr[i * 4 * 2 + 0] = c;
        colr[i * 4 * 2 + 1] = c;
        colr[i * 4 * 2 + 2] = c;
        colr[i * 4 * 2 + 3] = 1.0;
        colr[i * 4 * 2 + 4] = c;
        colr[i * 4 * 2 + 5] = c;
        colr[i * 4 * 2 + 6] = c;
        colr[i * 4 * 2 + 7] = 1.0;
        
        if(bSmooth == false) {
            continue;
        }
        
        j = i * 3;
        k = t * 3 * 2 - 3 - j;
        
        vert_line[j + 0] = points[i].x + v.x;
        vert_line[j + 1] = points[i].y + v.y;
        vert_line[j + 2] = 0;
        vert_line[k + 0] = points[i].x - v.x;
        vert_line[k + 1] = points[i].y - v.y;
        vert_line[k + 2] = 0;

        j = i * 4;
        k = t * 4 * 2 - 4 - j;
        
        colr_line[j + 0] = c;
        colr_line[j + 1] = c;
        colr_line[j + 2] = c;
        colr_line[j + 3] = 1.0;
        colr_line[k + 0] = c;
        colr_line[k + 1] = c;
        colr_line[k + 2] = c;
        colr_line[k + 3] = 1.0;
    }
    
    //---
    
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colr);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vert);
	
	glDrawArrays(GL_QUAD_STRIP, 0, t * 2);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

    delete[] vert;
    delete[] colr;
    
    //---
    
    if(bSmooth == false) {
        return;
    }
    
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colr_line);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vert_line);
	
	glDrawArrays(GL_LINE_STRIP, 0, t * 2);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

    delete[] vert_line;
    delete[] colr_line;
}

void ofxLIC::drawDots() {
    float s = step * 0.5;
    
    int t = points.size();
    for(int i=0; i<t; i++) {
        float p;
        p = sin((ofGetFrameNum() + i) * pulseSpeed + pulsePhase);
        p += 1;
        p *= 0.5;
        p = ofClamp(p, 0.2, 1.0);
        
        float c;
        c = sin((ofGetFrameNum() - i) * colorSpeed + colorPhase);
        c += 1;
        c *= 0.5;

        ofSetColor(255 * c, 255);
        
        ofFill();
        ofCircle(points[i].x, points[i].y, s * p);
        
        ofNoFill();
        ofCircle(points[i].x, points[i].y, s * p);
    }
    
    ofSetColor(255);
}
