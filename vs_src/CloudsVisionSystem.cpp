//
//  CloudsVisionSystem.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/23/13.
//
//

#include "CloudsVisionSystem.h"
using namespace ofxCv;
string CloudsVisionSystem::getSystemName()
{
	return "CloudssVisionSystem";
}

void CloudsVisionSystem::selfSetup()
{
    currentMode = OpticalFlow;
    curFlow = &farneback;
    movieIndex =0;
    pyrScale = 0.5;
    levels =4;
    winsize = 8;
    iterations =2;
    polyN = 7;
    polySigma = 1.5;
    winSize =32;
    maxLevel =  3;
    maxFeatures = 200;
    qualityLevel = 0.01;
    minDistance = 4;
    
    useFarneback = true;
    drawPlayer = true;
    drawThresholded =false;
    
    learningTime = 15;
    thresholdValue =128;
    
    cvPersistance =3;
    cvMaxDistance = 80;
    cvMinAreaRadius =15;
    cvMaxAreaRadius = 100;
    cvThresholdValue = 25;
    cvUpdateParameters = false;
    lineWidth = 2;
    
    //	app
    movieStrings.push_back("GreenPoint_bike.m4v");
    movieStrings.push_back("unionsq_1_realtime 3.mp4");
    movieStrings.push_back("unionsq_1 - Wi-Fi.m4v");
    movieStrings.push_back("Road 2.mov");
    movieStrings.push_back("traffic_1.mov");
    movieStrings.push_back("indianTraffic.mov");

    
    ofSetVerticalSync(true);
	ofBackground(0);
    if(player.loadMovie(movieStrings[0])){
        player.play();
    }
    else{
        cout<<"Not Playing"<<endl;
        
    }
    
    updateCVParameters();
    
    populateOpticalFlowRegions();
}

void CloudsVisionSystem::populateOpticalFlowRegions(){
    int rectWidth =20;
    int rectHeight = 20;
    for( int j=0; j<player.getHeight(); j +=rectHeight){
        for( int i=0; i<player.getWidth(); i += rectWidth){
            //;
            if (i +rectWidth>player.width) {
                rectWidth =player.width- i;
            }
            
            if (j +rectHeight>player.height) {
                rectHeight =player.height- j;
            }
            
            
            flowRegions.push_back(ofRectangle(i, j, rectWidth, rectHeight));
        }
    }
}

void CloudsVisionSystem::updateOpticalFlow(){
    
    
    if(useFarneback) {
        curFlow = &farneback;
        farneback.setPyramidScale( pyrScale);
        farneback.setNumLevels( levels );
        farneback.setWindowSize( winsize) ;
        farneback.setNumIterations( iterations);
        farneback.setPolyN( polyN) ;
        farneback.setPolySigma( polySigma );
        farneback.setUseGaussian(OPTFLOW_FARNEBACK_GAUSSIAN);
        
    } else {
        curFlow = &pyrLk;
        pyrLk.setMaxFeatures( maxFeatures);
        pyrLk.setQualityLevel( qualityLevel );
        pyrLk.setMinDistance( minDistance);
        pyrLk.setWindowSize( winSize);
        pyrLk.setMaxLevel( maxLevel);
    }
    
    //check it out that that you can use Flow polymorphically
    curFlow->calcOpticalFlow(player);
    
}

void CloudsVisionSystem::getTextures(){
    
}
void CloudsVisionSystem::updateCVParameters(){
    //  background subtraction
    background.setDifferenceMode(RunningBackground::ABSDIFF);
    background.setLearningTime(learningTime);
    background.setThresholdValue(thresholdValue);
    
	contourFinder.setMinAreaRadius(cvMinAreaRadius);
	contourFinder.setMaxAreaRadius(cvMaxAreaRadius);
	contourFinder.setThreshold(cvThresholdValue);
//    contourFinder.setTargetColor(ofColor::yellow);
    
    
    //how many frames before we give up on it
	tracker.setPersistence(cvPersistance);
	// an object can move up to 50 pixels per frame
	tracker.setMaximumDistance(cvMaxDistance);
//    tracker. 
    
}

void CloudsVisionSystem::selfPresetLoaded(string presetPath){
	
}

void CloudsVisionSystem::selfBegin()
{

}

void CloudsVisionSystem::selfEnd()
{
    
}

void CloudsVisionSystem::selfExit()
{
    
}

void CloudsVisionSystem::selfSetupSystemGui()
{
    sysGui->addLabel("BACKGROUND PARAM");
    sysGui->addSlider("LEARNING TIME", 0,100,&learningTime);
    sysGui->addSlider("THRESHOLD VALUE", 0,255  ,&thresholdValue);
    
    sysGui->addLabel("TRACKER PARAM");
    sysGui->addSlider("PERSISTANCE", 0,100,&cvPersistance);
    sysGui->addSlider("MAXDISTANCE", 0,100  ,&cvMaxDistance);

    sysGui->addLabel("CONTOUR FINDER PARAMS");
    sysGui->addSlider("MIN AREA RADIUS", 0,50,&cvMinAreaRadius);
    sysGui->addSlider("MAX AREA RADIUS",0,255,&cvMaxAreaRadius);
    sysGui->addSlider("THRESHOLD VALUE", 0, 255, &cvThresholdValue);
    sysGui->addButton("UPDATE CV PARAMS", false);

    
    sysGui->addLabel("OPTICAL FLOW PARAMS");
    
	sysGui->addSlider("PYRSCALE", .5, 0, &pyrScale);
    sysGui->addSlider("LEVELS",  1, 8, &levels);
	sysGui->addSlider("WINSIZE",  4, 64, &winsize);
	sysGui->addSlider("ITERATIONS",1, 8, &iterations);
	sysGui->addSlider("POLYN",5, 10, &polyN);
	sysGui->addSlider("POLYSIGMA", 1.5, 1.1, &polySigma);
	sysGui->addToggle("OPTFLOW_FARNEBACK_GAUSSIAN", useFarneback);
	
	sysGui->addSlider("WINSIZE", 4, 64, &winSize);
	sysGui->addSlider("MAXLEVEL", 0, 8, &maxLevel);
	
	sysGui->addSlider("MAXFEATURES", 200, 1, &maxFeatures);
	sysGui->addSlider("QUALITYLEVEL", 0.01, 0.001, &qualityLevel);
	sysGui->addSlider("MINDISTANCE", 4, 1, &minDistance);
    
    sysGui->autoSizeToFitWidgets();
    
    ofAddListener(sysGui->newGUIEvent, this, &CloudsVisionSystem::selfGuiEvent);
    
}

void CloudsVisionSystem::selfSetupRenderGui()
{
    
    rdrGui->addSpacer();
    rdrGui->addLabel("CV MODES");
    rdrGui->addSlider("BOX LINE WIDTH", 1, 10, &lineWidth);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);;
    ofxUIButton *loadbtn = rdrGui->addButton("OPTICAL FLOW", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *updatebtn = rdrGui->addToggle("CONTOUR TRACKING", false);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    ofxUIButton *drawplayerbtn = rdrGui->addToggle("DRAW PLAYER", &drawPlayer);
    rdrGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIButton *drawthresholdedbtn = rdrGui->addToggle("DRAW THRESHOLDED", &drawThresholded);
//    rdrGui->addButton("LOAD NEXT CLIP", false);
//    rdrGui->addButton("PLAY CLIP", false);
    rdrGui->autoSizeToFitWidgets();
    ofAddListener(rdrGui->newGUIEvent, this, &CloudsVisionSystem::selfGuiEvent);

}

void CloudsVisionSystem::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisionSystem::selfKeyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisionSystem::selfUpdate(){
    player.update();
    if(currentMode == ControurTracking){
        if(player.isPlaying() && player.isLoaded()){
            
            if(player.isFrameNew() ){
                background.update(player, thresholded);
                thresholded.update();
                blur(thresholded, 5);
                contourFinder.findContours(thresholded);
                tracker.track(contourFinder.getBoundingRects());
            }
        }
        
        
    }
    else if(currentMode == OpticalFlow){
        updateOpticalFlow();
        if(useFarneback){
            
//            if(farneback.getTotalFlow()){
//                for(int i =0; i<flowRegions.size(); i++){
//                    flowMotion.push_back(farneback.getAverageFlowInRegion(flowRegions[0]));
//                }
//            }

            
            averageFlow =farneback.getTotalFlow();
            
        }
        else{
            
        }
        
        
    }
}


void CloudsVisionSystem::selfDraw()
{
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    mat->begin();
    mat->end();
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    
}


void CloudsVisionSystem::selfSetupGuis()
{
    
}

void CloudsVisionSystem::selfAutoMode()
{
    
}


void CloudsVisionSystem::selfDrawBackground()
{
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofPushMatrix();
    ofTranslate(-player.getWidth()/2, -player.getHeight()/2);
    
    if(drawPlayer){
        player.draw(0,0);
        
    }
    if(drawThresholded){
        thresholded.draw(0,0, thresholded.width, thresholded.height);
    }

    if(currentMode == ControurTracking){
        contourFinder.draw();
        
        vector<MyTracker>& followers = tracker.getFollowers();

        for(int i = 0; i < followers.size(); i++) {
                followers[i].draw(lineWidth);
//                followers[i].kill();
        }
    }
    
    else if(currentMode == OpticalFlow){
        curFlow->draw(0, 0);
        
        if(useFarneback){
            for(int i=0; i<flowRegions.size(); i++){
                ofPushStyle();
                ofNoFill();
                ofSetColor(255);
                //ofCircle(flowRegions[i].x, flowRegions[i].y, flowRegions[i].width);
                ofRect(flowRegions[i]);
                ofPopStyle();
                ofLine(player.getWidth()/2, player.getHeight(), ofClamp(averageFlow.x, 0, player.getWidth()) , ofClamp(averageFlow.y, 0, player.getHeight()));
                
            }
//            for (int i=0; i<flowRegions.size(); i++) {
//                ofLine(flowRegions [i].x,flowRegions[i].y,flowMotion[i].x,flowMotion[i].y);
//            }
        }
    }
    
    ofPopMatrix();
    ofPopMatrix();
    
    
}

void CloudsVisionSystem::selfDrawDebug()
{
    
}

void CloudsVisionSystem::selfSceneTransformation()
{
    
}

void CloudsVisionSystem::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisionSystem::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisionSystem::selfSetupGui()
{
    
}

void CloudsVisionSystem::selfGuiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    ofxUIButton* b = (ofxUIButton*) e.widget;
    if(name == "UPDATE CV PARAMS" &&  b->getValue() ){
        updateCVParameters();
        cout<<"updating parameters"<<endl;
    }
    else if (name == "OPTICAL FLOW" &&  b->getValue()){
        cout<<"setting mode to optical flow"<<endl;
        currentMode = OpticalFlow;
        
    }
    else if( name == "CONTOUR TRACKING" &&  b->getValue() ){
        cout<<"setting mode to contour"<<endl;
        currentMode = ControurTracking;
    }
    else if (name == "OPTFLOW_FARNEBACK_GAUSSIAN" ){
        
        useFarneback = b->getValue();
        
    }
    else if (name == "DRAW PLAYER"){
        drawPlayer = b->getValue();
    }
    else if( name == "DRAW THRESHOLDED"){
        drawThresholded = b->getValue();
    }
    
    
}

void CloudsVisionSystem::guiRenderEvent(ofxUIEventArgs &e)
{
    
}