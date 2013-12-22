//
//  ofxStreamline.h
//  opencvExample
//
//  Created by lukasz karluk on 22/03/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxVec2f.h"

class ofxStreamline : public ofBaseApp
{
  
public:
    
     ofxStreamline();
    ~ofxStreamline();
    
    void        setup   ();
    void        update  ();
    void        draw    ();
    
    void        drawLines       ( bool bSmooth = false );
    void        drawStrip       ( bool bSmooth = false );
    void        drawDots        ();
    
    void        setVectorData   ( ofPoint* vecField, int vecFieldW, int vecFieldH );
    void        setPosition     ( float x, float y );
    void        setStep         ( float step );
    
    void        RK              ( ofxVec2f &p, double h );
    ofxVec2f    getVector       ( const ofPoint &p );
    
    ofPoint*        vecField;
    int             width;
    int             height;
    
    ofxVec2f        pos;
    vector<ofPoint> points;
    float           step;
    int             stepNum;
    
    float           dotSize;
    float           stripWidth;
    float           pulsePhase;
    float           pulseSpeed;
    float           colorPhase;
    float           colorSpeed;
    
};