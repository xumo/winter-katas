#include "CiVideoInput.hpp"
#include "cinder/Log.h"


CiVideoInput::CiVideoInput( const ci::Capture::DeviceRef& device )
{
    CI_LOG_I( "\n=== Setting up capture with device: " << device->getName() << " (auto mode) ===" );
    try {
        // Stop and fully release old capture
        if( mCapture ) {
            CI_LOG_I( "Stopping previous capture" );
            mCapture->stop();
        }

        // Reset everything to ensure clean state
        mCapture = nullptr;
        mTexture = nullptr;

        // Use a working mode instead of the first one (which might be problematic)
        /*if( ! mCurrentModes.empty() ) {
            // Try to find 2560x720 mode that we know works, otherwise use first mode
            int modeIndex = 0;
            for( size_t i = 0; i < mCurrentModes.size(); i++ ) {
                if( mCurrentModes[i].getWidth() == 2560 && mCurrentModes[i].getHeight() == 720 ) {
                    modeIndex = i;
                    break;
                }
            }
            CI_LOG_I( "Creating capture with mode [" << modeIndex << "]: " << toString( mCurrentModes[modeIndex] ) );
            mCapture = Capture::create( device, mCurrentModes[modeIndex] );
        }
        else
         */
        {
            CI_LOG_I( "No modes available, trying default 640x480" );
            mCapture = Capture::create( 640, 480, device );
        }
        mCapture->start();
        CI_LOG_I( "Capture started successfully - actual size: " << mCapture->getWidth() << "x" << mCapture->getHeight() );
    }
    catch( ci::Exception& exc ) {
        CI_LOG_EXCEPTION( "Failed to setup capture with device: " << device->getName(), exc );
        CI_LOG_E( "ERROR: Failed to setup capture!" );
        // Leave mCapture as nullptr if setup fails
        mCapture = nullptr;
        mTexture = nullptr;
    }
}

void CiVideoInput::Update()
{
    if( mCapture && mCapture->checkNewFrame() ) {
        auto surface = mCapture->getSurface();
        if( surface ) {
            /*
            frameCount++;
            if( firstFrame || frameCount % 100 == 0 ) {
                CI_LOG_I( "Frame " << frameCount << " received - Surface size: " << surface->getWidth() << "x" << surface->getHeight() );
                firstFrame = false;
            }
            */
            if( ! mTexture ) {
                // Capture images come back as top-down, and it's more efficient to keep them that way
                mTexture = gl::Texture::create( *surface, gl::Texture::Format().loadTopDown() );
            }
            else {
                mTexture->update( *surface );
            }
        }
    }
}

