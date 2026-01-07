#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/Utilities.h"

#include "ps3eye.h"
#include "VideoInput.hpp"

#include "Ps3EyeVideoInput.hpp"
#include "CiVideoInput.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;


class VideoCamSelectionApp : public App {
  public:
    void setup() override;
    void update() override;
    void draw() override;
    void cleanup() override;
    void keyDown( KeyEvent event ) override;

  private:
    std::vector<VideoInputRef> mVideoInputs;
};

void VideoCamSelectionApp::setup()
{

    using namespace ps3eye;

    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );
    console() << "found " << devices.size() << " cameras" << std::endl;
    
    for(auto device :  PS3EYECam::getDevices())
    {
        cout << " ps3 device " << device << "\n";
        mVideoInputs.push_back(Ps3EyeVideoInput::Create(device));
    }
    
    auto ciDevices =  Capture::getDevices();
    for (auto device : ciDevices) {
        cout << " cinder cwebcam device " << device << "\n";
        mVideoInputs.push_back(CiVideoInput::Create(device));
    }
    
    
}

void VideoCamSelectionApp::cleanup() {
    for(const auto& videoInput :  mVideoInputs)
    {
        videoInput->Destroy();
    }
}

void VideoCamSelectionApp::update()
{
    for(const auto& videoInput :  mVideoInputs)
    {
        videoInput->Update();
    }

}

void VideoCamSelectionApp::draw()
{
    gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    gl::enableAlphaBlending();
    
    
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    int cols = mVideoInputs.size() > 1 ? 2 : 1;
    int rows = mVideoInputs.size() > 1 ? static_cast<int>(mVideoInputs.size()) / 2 : 1;
    int width = getWindowWidth() / cols;
    int height = getWindowHeight() / rows;
    int i = 0, j = 0;
    for(const auto& videoInput :  mVideoInputs)
    {
        j = std::ceil(i / rows);
        glPushMatrix();
        gl::draw(
                 videoInput->GetTexture(),
                 Rectf(width * (i % 2), height * j, width * ( (i % 2) + 1), height * (j + 1))
                 );
        glPopMatrix();
        i++;
    }
    //ImGui::End();
}

void VideoCamSelectionApp::keyDown( KeyEvent event )
{
    
}


void prepareSettings( VideoCamSelectionApp::Settings* settings )
{
#if defined( CINDER_ANDROID )
    settings->setKeepScreenOn( true );
#endif

#if defined( CINDER_MSW )
    // Enable console window on Windows for debugging
    settings->setConsoleWindowEnabled( true );
#endif
}

CINDER_APP( VideoCamSelectionApp, RendererGl, prepareSettings )
