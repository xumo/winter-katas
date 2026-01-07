#include "VideoInput.hpp"

#include "cinder/gl/gl.h"
#include "cinder/Capture.h"

using namespace ci;

class CiVideoInput;

typedef std::unique_ptr<CiVideoInput> CiVideoInputRef;

class CiVideoInput : public VideoInput
{
public:
    gl::TextureRef  GetTexture() {return mTexture;};
    
    //~CiVideoInput();
    static CiVideoInputRef Create(const ci::Capture::DeviceRef& device )
    {
        return std::make_unique<CiVideoInput>(CiVideoInput(device));
    }
    
    void                Update();
    void                Destroy() { };
    std::string         GetName() { return "Cinder WebCam"; };
    CamType             GetType() { return CamType::CI_WEBCAM; };
    
private:
    CiVideoInput(const ci::Capture::DeviceRef& device);
    CaptureRef                          mCapture;
    gl::TextureRef                      mTexture;
};
