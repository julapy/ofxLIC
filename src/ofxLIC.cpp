//
//  ofxLIC.cpp
//  Created by lukasz karluk on 22/03/11.
//

#include "ofxLIC.h"

//------------------------------------------------ vector field.
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
    bounds.set(0, 0, w, h);
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
    if(vecs.size() != size) {
        vecs.resize(size, ofVec2f());
    }
}

ofVec2f & ofxLICVectorField::getVector(ofVec2f & p) {
    return getVector(p.x, p.y);
}

ofVec2f & ofxLICVectorField::getVector(float x, float y) {
    int xInt, yInt;
    xInt = ofClamp(x, 0, width-1);
    yInt = ofClamp(y, 0, height-1);
    int i = yInt * width + xInt;
    
    return getVector(i);
}

ofVec2f & ofxLICVectorField::getVector(int index) {
    vecValue.set(vecs[index]);
    return vecValue;
}

int ofxLICVectorField::getWidth() {
    return width;
}

int ofxLICVectorField::getHeight() {
    return height;
}

int ofxLICVectorField::getSize() {
    return size;
}

const ofRectangle & ofxLICVectorField::getBounds() {
    return bounds;
}

void ofxLICVectorField::draw(int stride, float scale) {
    int w = getWidth();
    int h = getHeight();
    
    for(int y=0; y<h; y+=stride) {
        for(int x=0; x<w; x+=stride) {
            int i = y * w + x;
            
            ofVec2f & vec = getVector(i);
            ofLine(x, y, x + vec.x * scale, y + vec.y * scale);
        }
    }
}

//------------------------------------------------ LIC.
ofxLIC::ofxLIC() {
    vecField = NULL;
    bVecFieldExternal = false;
    bCropToBounds = false;
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

//------------------------------------------------
void ofxLIC::setVectorField(ofxLICVectorField * vecFieldPtr) {
    if(vecField != NULL && bVecFieldExternal == false) {
        delete vecField;
        vecField = NULL;
    }
    vecField = vecFieldPtr;
    bVecFieldExternal = true;
}

void ofxLIC::setVector(float x, float y, ofVec2f & vec) {
    vecField->setVector(x, y, vec);
}

void ofxLIC::setVector(vector<ofVec2f> & vecs) {
    vecField->setVector(vecs);
}

void ofxLIC::setCropToBounds(bool value) {
    bCropToBounds = value;
}

//------------------------------------------------
ofPolyline & ofxLIC::getStreamline(const ofVec2f & pos, int numOfPoints, float stepSize) {
    return getStreamline(pos.x, pos.y, numOfPoints, stepSize);
}

ofPolyline & ofxLIC::getStreamline(float x, float y, int numOfPoints, float stepSize) {

    const ofRectangle & bounds = vecField->getBounds();
    
    vector<ofVec3f> & points = streamline.getVertices();
    if(points.size() != numOfPoints) {
        points.resize(numOfPoints, ofVec2f());
    }
    
    points[0].set(x, y);
    
    ofVec2f p(x, y);
    for(int i=1; i<numOfPoints; i++) {
        RK(p, stepSize);
        
        if(bCropToBounds == true) {
            if(bounds.inside(p) == false) {
                points.erase(points.begin()+i, points.begin()+numOfPoints);
                break;
            }
        }
        
        points[i].set(p);
    }
    
    return streamline;
}

//------------------------------------------------ this is where the magic happens.
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

//------------------------------------------------ draw.
void ofxLIC::draw(int stride, int numOfPoints, int stepSize) {
    if(vecField == NULL) {
        return;
    }
    
    int w = vecField->getWidth();
    int h = vecField->getHeight();
    
    for(int y=0; y<h; y+=stride) {
        for(int x=0; x<w; x+=stride) {
            int i = y * w + x;
            
            ofVec2f & vec = vecField->getVector(i);
            ofPolyline & poly = getStreamline(x, y, numOfPoints, stepSize);
            poly.draw();
        }
    }
}

void ofxLIC::drawDebug() {
    int w = vecField->getWidth();
    int h = vecField->getHeight();
    
    ofNoFill();
    ofRect(0, 0, w, h);
    ofFill();
}
