#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/Utilities.h"

#include "ps3eye.h"

#include "Ps3EyeVideoInput.hpp"

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
    std::vector<Ps3EyeVideoInputRef> mPs3s;
};

void VideoCamSelectionApp::setup()
{

    using namespace ps3eye;

    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );
    console() << "found " << devices.size() << " cameras" << std::endl;
    
    for(auto device :  PS3EYECam::getDevices())
    {
        cout << " device " << device << "\n";
        mPs3s.push_back(Ps3EyeVideoInput::Create(device));
    }
    
    
}

void VideoCamSelectionApp::cleanup() {
 
}

void VideoCamSelectionApp::update()
{
    for(const auto& ps3 :  mPs3s)
    {
        ps3->Update();
    }

}

void VideoCamSelectionApp::draw()
{
    gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    gl::enableAlphaBlending();
    
    
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    int cols = mPs3s.size() > 1 ? 2 : 1;
    int rows = static_cast<int>(mPs3s.size()) / 2;
    int width = getWindowWidth() / cols;
    int height = getWindowHeight() / rows;
    int i = 0;
    for(const auto& ps3 :  mPs3s)
    {
        glPushMatrix();
        gl::draw( ps3->GetTexture(), Rectf(width * i, 0,width * (i + 1), height ) );
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
