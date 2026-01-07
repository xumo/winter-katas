#include "VideoInput.hpp"
#include "ps3eye.h"

#include <memory>
#include <vector>

class Ps3EyeVideoInput;

typedef std::unique_ptr<Ps3EyeVideoInput> Ps3EyeVideoInputRef;

class Ps3EyeVideoInput : public VideoInput
{
            
public:
    Ps3EyeVideoInput() = delete;
    Ps3EyeVideoInput(const Ps3EyeVideoInput& ps3) {
        this->frame_bgr = ps3.frame_bgr;
        this->mEye = ps3.mEye;
        this->mFrame = ps3.mFrame;
    };
    ~Ps3EyeVideoInput();
    static Ps3EyeVideoInputRef Create(ps3eye::PS3EYECam::PS3EYERef& eye)
   {
        return std::make_unique<Ps3EyeVideoInput>(Ps3EyeVideoInput(eye));
   }
    
    void                Update();
    void                Destroy();
    std::string         GetName()  { return "PS3 Eye"; };
    ci::gl::TextureRef  GetTexture() { return mTexture;};
    CamType             GetType() { return CamType::PS3EYE; };
    
private:
    Ps3EyeVideoInput(ps3eye::PS3EYECam::PS3EYERef& eye);
    ci::gl::TextureRef mTexture;
    uint8_t *frame_bgr;
    std::vector<uint8_t> mFrameData;
    ci::Surface mFrame;
    ps3eye::PS3EYECam::PS3EYERef mEye;
};
