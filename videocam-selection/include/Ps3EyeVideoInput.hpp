#include "VideoInput.hpp"
#include "ps3eye.h"

#include <memory>
#include <vector>

class Ps3EyeVideoInput;

typedef std::unique_ptr<Ps3EyeVideoInput> Ps3EyeVideoInputRef;

class Ps3EyeVideoInput : VideoInput //: public std::enable_shared_from_this<Ps3EyeVideoInput>
{
            
public:
    Ps3EyeVideoInput() = delete;
    Ps3EyeVideoInput(const Ps3EyeVideoInput& ps3) {
        std::cout << "Copy Ps3EyeVideoInput \n";
        this->frame_bgr = ps3.frame_bgr;
        this->mEye = ps3.mEye;
        this->mFrame = ps3.mFrame;
    };
    ~Ps3EyeVideoInput();
    static Ps3EyeVideoInputRef Create(ps3eye::PS3EYECam::PS3EYERef& eye)
   {
        return std::make_unique<Ps3EyeVideoInput>(Ps3EyeVideoInput(eye));
   }
    
    void            Update();
    std::string     GetName()  { return "PS3 Eye"; };
    ci::gl::TextureRef  GetTexture() { return mTexture;};
    friend class Ps3EyeVideoInputManager;
    
private:
    Ps3EyeVideoInput(ps3eye::PS3EYECam::PS3EYERef& eye);
    ci::gl::TextureRef mTexture;
    uint8_t *frame_bgr;
    std::vector<uint8_t> mFrameData;
    ci::Surface mFrame;
    ps3eye::PS3EYECam::PS3EYERef mEye;
};


/*class Ps3EyeVideoInputManager {
public:
    Ps3EyeVideoInputManager() = delete;
    static Ps3EyeVideoInputRef Create(ps3eye::PS3EYECam::PS3EYERef& eye)
    {
        
        return std::make_shared<Ps3EyeVideoInput>(Ps3EyeVideoInput(eye));
    }
};*/
