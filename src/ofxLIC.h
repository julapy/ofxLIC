//
//  ofxLIC.h
//  Created by lukasz karluk on 22/03/11.
//

#pragma once

#include "ofMain.h"

class ofxLICVectorField {
public:
    
    ofxLICVectorField();
    ~ofxLICVectorField();
    
    virtual void setup(int w, int h);
    virtual void setVector(float x, float y, ofVec2f & vec);
    virtual void setVector(vector<ofVec2f> & vecsToCopy);
    virtual ofVec2f & getVector(float x, float y);
    virtual ofVec2f & getVector(ofVec2f & p);
    virtual int getWidth();
    virtual int getHeight();
    
    int width;
    int height;
    int size;
    vector<ofVec2f> vecs;
    ofVec2f vecValue;
};

class ofxLIC {
  
public:
    
    ofxLIC();
    ~ofxLIC();
    
    void setup(int w, int h);
    void setVectorField(ofxLICVectorField * vecField);
    void setVector(float x, float y, ofVec2f & vec);
    void setVector(vector<ofVec2f> & vecs);
    
    void setPosition(float x, float y);
    void setStep(float step);
    
    void update();
    void drawLines(bool bSmooth = false);
    void drawStrip(bool bSmooth = false);
    void drawDots();
    
    void RK(ofVec2f & p, double h);
    
    ofxLICVectorField * vecField;
    bool bVecFieldExternal;
    
    ofVec2f pos;
    vector<ofVec2f> points;
    float step;
    int stepNum;
    
    float dotSize;
    float stripWidth;
    float pulsePhase;
    float pulseSpeed;
    float colorPhase;
    float colorSpeed;
};