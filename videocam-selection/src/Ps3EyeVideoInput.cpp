#include "Ps3EyeVideoInput.hpp"

Ps3EyeVideoInput::Ps3EyeVideoInput(ps3eye::PS3EYECam::PS3EYERef& eye)
{
    mEye = eye;
    bool res = eye->init(640, 480, 60);
    std::cout << "init eye result " << res << std::endl;
    
    mEye->start();
   
    //mFrameData.clear();
    //mFrameData.assign(mEye->getWidth()*mEye->getHeight()*3, 0);
    frame_bgr = new uint8_t[mEye->getWidth()*mEye->getHeight()*3];
    mFrame = ci::Surface(frame_bgr, mEye->getWidth(), mEye->getHeight(), mEye->getWidth()*3, ci::SurfaceChannelOrder::BGR);
    memset(frame_bgr, 0, mEye->getWidth()*mEye->getHeight()*3);
   
}

Ps3EyeVideoInput::~Ps3EyeVideoInput()
{
    //if (mEye)
    //    mEye->stop();
    //
    //delete[] frame_bgr;
    std::cout << "~Ps3EyeVideoInput\n";
}

void Ps3EyeVideoInput::Destroy()
{
    if (mEye)
        mEye->stop();
    //
    delete[] frame_bgr;
    std::cout << "~Ps3EyeVideoInput\n";
}


void Ps3EyeVideoInput::Update()
{
    if(mEye)
    {
        mEye->getFrame(frame_bgr);
        if (mTexture) {
            //mTexture->update(mFrame);
         mTexture->update(frame_bgr, GL_BGR, GL_UNSIGNED_BYTE,0, mEye->getWidth(), mEye->getHeight());
            //! Updates the pixels of a Texture with the data pointed to by \a data, of format \a dataFormat (Ex: GL_RGB), and type \a dataType (Ex: GL_UNSIGNED_BYTE) of size (\a width, \a height). \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
           // void            update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int height, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
        } else {
            // static Texture2dRef    create( const void *data, GLenum dataFormat, int width, int height, const Format &format = Format() );
            //Texture = ci::gl::Texture::create( mFrame );
            mTexture = ci::gl::Texture::create(frame_bgr, GL_BGR, mEye->getWidth(), mEye->getHeight());
            mTexture->setTopDown(true);
        }
        /*mCamFrameCount++;
        double now = mTimer.getSeconds();
        if( now > mCamFpsLastSampleTime + 1 ) {
            uint32_t framesPassed = mCamFrameCount - mCamFpsLastSampleFrame;
            mCamFps = (float)(framesPassed / (now - mCamFpsLastSampleTime));

            mCamFpsLastSampleTime = now;
            mCamFpsLastSampleFrame = mCamFrameCount;
        }
    
        gui->update();
        eyeFpsLab->update_fps(mCamFps);
         */
    }
}
