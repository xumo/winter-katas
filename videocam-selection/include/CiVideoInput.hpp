#include "VideoInput.hpp"

#include "cinder/gl/gl.h"
#include "cinder/Capture.h"

using namespace ci;

class CiVideoInput : VideoInput
{
    
    void            Update();
    std::string     GetName();
    gl::TextureRef  GetTexture() {return mTexture;};
    
private:
    CaptureRef                          mCapture;
    gl::TextureRef                      mTexture;
};
