//
//  ofxStreamline.h
//  Created by lukasz karluk on 22/03/11.
//

#pragma once

#include "ofMain.h"

class ofxStreamline : public ofBaseApp {
  
public:
    
    ofxStreamline();
    ~ofxStreamline();
    
    void setup();
    void update();
    void drawLines(bool bSmooth = false);
    void drawStrip(bool bSmooth = false);
    void drawDots();
    
    void setVectorData(ofVec2f * vecField, int vecFieldW, int vecFieldH);
    void setPosition(float x, float y);
    void setStep(float step);
    
    void RK(ofVec2f & p, double h);
    ofVec2f getVector(const ofVec2f & p);
    
    ofVec2f * vecField;
    int width;
    int height;
    
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