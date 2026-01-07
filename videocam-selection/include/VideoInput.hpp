#pragma once
//
//  VideoCamDevice.hpp
//  cinder-sandbox
//
//  Created by Rodrigo Torres on 23/12/2025.
//

#include "cinder/gl/gl.h"

enum CamType
{
    CI_WEBCAM,
    PS3EYE,
    KINECT,
    KINECT2
};

class VideoInput;

typedef std::unique_ptr<VideoInput> VideoInputRef;

class VideoInput {
public:
    virtual void                Update()        = 0;
    virtual void                Destroy()       = 0;
    virtual std::string         GetName()       = 0;
    virtual ci::gl::TextureRef  GetTexture()    = 0;
    virtual CamType             GetType()       = 0;
};
