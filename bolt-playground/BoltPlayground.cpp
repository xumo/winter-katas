//
// Created by Rodrigo Torres on 05/12/2025.
//
#include "Resources.hpp"

#include <format>
#include <iostream>
#include <ostream>
#include <string>
#include <initializer_list>
#include <vector>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Context.h"
#include "cinder/CinderImGui.h"

//#include "glm/glm.hpp"
//#include "cinder/ip/Checkerboard.h"



using namespace ci;
using namespace ci::app;
using namespace std;


struct Bolt {
    vec2 start;
    vec2 end;
    int divs;
};

struct Segment {
	size_t start;
	size_t end;
};

struct Bullet {
	vec2 position {0,0}	 ;
	bool hovered { false };
	bool pressed { false };
};
const int NUM_POINTS = 4;

class BoltPlaygroundApp : public App {
  public:
	void	setup() override;
	void	update() override;
	void	draw() override;

	void mouseDown( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;


    

	static constexpr int	windowWidth = 1024;
	static constexpr int	windowHeight = 768;

private:
    void addSegment(const size_t& start, const size_t& end);
    void renderSegmentsFbo() const;
    void drawPrism(const vector<vec3>& vertices) const;
    void drawGui();
    void createCircles();
    void drawWithGeometrShader() const;
    void drawVba();
	vector<Bullet>		mBullets;
	vector<Segment>		mSegments;
    vector<Bolt>        mBolts;
	gl::FboRef			mFbo,mBlurFbo, mBloomFbo;
	gl::GlslProgRef		mBloomShader, mBlurShader, mGeometryShader, mBoltProg;
	gl::VboMeshRef		mVboMesh;
    
    gl::VaoRef        mAttributes[2];
    gl::VboRef        mParticleBuffer[2];
    std::uint32_t    mSourceIndex        = 0;
    std::uint32_t    mDestinationIndex    = 1;
    
    gl::TextureRef        mTexture;
	gl::VertBatchRef		mBatch;
	size_t				mActiveBullet = -1;
	const float			mEpsilon = 0.0001f;

	const float			mBulletRadius = 10.0f;
	const float			mDetectionRadius = mBulletRadius * 0.1f;

	bool				mMouseDown = false;
    //bool                mDrawSquares = false;
    bool                mDrawCircles = false;
    //bool                mDrawLines  = false;
    bool                mUseGeometryShader = true;
  
    int                 mNumCircles = 6;
    float               mCircleRadius = 300;
    int                 mBoltDivisions = 4;
    
    int                  mNumSides = 4;
    float                mRadius = 50.0f;
    float                mOffset = 0.1f;

	vector<vec3>				mVertices;
    
    
    int               mRenderOptionsSelection = 3;
    vector<string> mRenderOptionsItems = { "GL Lines", "CPU Segments", "Geometry Shader", "VBA"};
};


///
/// Begin Setup
///


void BoltPlaygroundApp::setup()
{
  
    ImGui::Initialize( ImGui::Options().window( getWindow() ).enableKeyboard( true ) );
    ImGui::GetStyle().ScaleAllSizes( getWindowContentScale() ); // for Retina / hi-dpi
    ImGui::GetStyle().FontScaleMain = getWindowContentScale();
	// vector<gl::VboMesh::Layout> bufferLayout = {
	// 	gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
	// 	gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::TEX_COORD_0, 2 )
	// };
	// mVboMesh = gl::VboMesh::create( 4, GL_TRIANGLES ,bufferLayout );
    
    mBatch = gl::VertBatch::create(GL_POINTS, true);
    createCircles();
	
	//mFbo = gl::Fbo::create(windowWidth, windowHeight, gl::Fbo::Format().colorTexture( cinder::gl::Fbo::Format::getDefaultColorTextureFormat().internalFormat( GL_RGB16F )));

    //auto format = gl::Fbo::Format().samples(4);
    mFbo = gl::Fbo::create(windowWidth, windowHeight, true);
    
	auto vertShader = loadAsset( "shaders/shader.vert" );
	auto fragShader = loadAsset( "shaders/shader.frag" );
	auto geomShader = loadAsset( "shaders/shader.geom" );
    mGeometryShader = gl::GlslProg::create(vertShader, fragShader, geomShader);

	mBlurShader = gl::GlslProg::create(
		loadAsset( "shaders/passthrough.vert" ),
		loadAsset( "shaders/simple.frag" )
	);
    
    auto progFormat = gl::GlslProg::Format()
        .vertex( loadAsset( "shaders/bolt.vert" ) )
        .geometry( loadAsset( "shaders/bolt.geom" ) )
        .fragment( loadAsset( "shaders/bolt.frag" ) )
        //.feedbackFormat( GL_INTERLEAVED_ATTRIBS )
        //.feedbackVaryings( { "start" ,  "end", "divisions" } )
        .attribLocation( "iStartPosition", 0 )
        .attribLocation( "iEndPosition", 1 )
        .attribLocation( "iDivisions", 2 );
    
    mBoltProg = gl::GlslProg::create(progFormat);

    mBloomShader = gl::getStockShader( gl::ShaderDef().color() );
	// mBatch = gl::VertBatch::create();gl::Fbo::Format().colorTexture( cinder::gl::Fbo::Format::getDefaultColorTextureFormat().internalFormat( GL_RGB16F )));
	mBloomFbo = gl::Fbo::create(windowWidth, windowHeight, gl::Fbo::Format().colorTexture( cinder::gl::Fbo::Format::getDefaultColorTextureFormat().internalFormat( GL_RGB32F )));


	mVertices.assign(4, vec3(0.0f, 0.0f, 0.0f));
	mVboMesh = gl::VboMesh::create( NUM_POINTS, GL_PATCHES, { gl::VboMesh::Layout().attrib( geom::POSITION, 3 ) } );
	mVboMesh->bufferAttrib( geom::POSITION, sizeof(vec2) * NUM_POINTS, &mVertices[0] );

	//VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec3) * points.size() );
	//arrayVbo->bufferSubData( 0, sizeof(vec3) * points.size(), points.data() );

	
    gl::enableAlphaBlending();
    mTexture = gl::Texture::create( loadImage( loadAsset( "images/cruz_azul.png" ) ), gl::Texture::Format().mipmap() );
    gl::enableVerticalSync(false);
    //mRenderProg = gl::getStockShader( gl::ShaderDef().color() );
    
};

void BoltPlaygroundApp::createCircles()
{
    cout << "creating circles" << endl;
    mBullets.clear();
    mSegments.clear();
    mBolts.clear();
    float angle = 0.0f;
    mBatch->clear();
    vec2 pointOnCircle = vec2(cos(angle), sin(angle)) * mCircleRadius + vec2(windowWidth / 2, windowHeight / 2);
    mBullets.emplace_back(Bullet{ pointOnCircle });
    
    mBatch->vertex( pointOnCircle );
    mBatch->color(Color::white());
    for (int i = 1; i < mNumCircles + 1; ++i) {
        angle = static_cast<float>(i) / static_cast<float>(mNumCircles) * glm::two_pi<float>();
        pointOnCircle = vec2(cos(angle), sin(angle)) * mCircleRadius + vec2(windowWidth / 2, windowHeight / 2);
        mBullets.emplace_back(Bullet{ pointOnCircle });
        addSegment(i-1,i);
        mBatch->vertex( pointOnCircle );
        mBatch->color(Color::white());
    }
    
    mParticleBuffer[mSourceIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, mBolts.size() * sizeof(Bolt), mBolts.data(), GL_STATIC_DRAW );
    mParticleBuffer[mDestinationIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, mBolts.size() * sizeof(Bolt), mBolts.data(), GL_STATIC_DRAW );
    
    for( int i = 0; i < 2; ++i )
    {    // Describe the particle layout for OpenGL.
        mAttributes[i] = gl::Vao::create();
        gl::ScopedVao vao( mAttributes[i] );
        
        // Define attributes as offsets into the bound particle buffer
        gl::ScopedBuffer buffer( mParticleBuffer[i] );
        gl::enableVertexAttribArray( 0 );
        gl::enableVertexAttribArray( 1 );
        gl::enableVertexAttribArray( 2 );
        gl::vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(Bolt), (const GLvoid*)offsetof(Bolt, start) );
        gl::vertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Bolt), (const GLvoid*)offsetof(Bolt, end) );
        gl::vertexAttribPointer( 2, 1, GL_INT, GL_FALSE, sizeof(Bolt), (const GLvoid*)offsetof(Bolt,divs) );
    }
   
}
void BoltPlaygroundApp::addSegment(const size_t &start, const size_t &end)
{
    assert(!mBullets.empty());
    assert(start < mBullets.size());
   // if (mBullets.size() < 2) return;
    assert(end < mBullets.size());
    mSegments.emplace_back(start, end);
    mBolts.emplace_back(Bolt{ mBullets[start].position, mBullets[end].position, mBoltDivisions });
}

///
/// Mouse interactions
///

void BoltPlaygroundApp::mouseMove(MouseEvent event)
{
    if (mMouseDown) { return;}
    const auto mousePos = vec2(event.getX(), event.getY());

    for (auto & bullet : mBullets) {
            float distance = glm::distance(mousePos, bullet.position);
        bullet.hovered = distance < (mBulletRadius + mDetectionRadius);
    }
}

void BoltPlaygroundApp::mouseDrag(MouseEvent event)
{
    const auto mousePos = vec2(event.getX(), event.getY());
    if (mActiveBullet != -1) {
        mBullets[mActiveBullet].position = mousePos;
    }
}


void BoltPlaygroundApp::mouseDown( MouseEvent event )
{
    mMouseDown = true;
    const auto mousePos = vec2(event.getX(), event.getY());
    for (size_t i = 0; i < mBullets.size(); ++i) {
        auto& bullet = mBullets[i];
        float distance = glm::distance(mousePos, bullet.position);
        if ( distance < (mBulletRadius + mDetectionRadius) ) {
            bullet.hovered = true;
            bullet.pressed = bullet.hovered;
            mActiveBullet = i;
            break;
        };
    }
}

void BoltPlaygroundApp::mouseUp( MouseEvent event )
{
    mMouseDown = false;
    mActiveBullet = -1;
    for (auto & bullet : mBullets) {
        bullet.hovered = false;
        bullet.pressed = false;
    }
}

///
/// Begin drawgin methods
///

void BoltPlaygroundApp::drawWithGeometrShader()const
{
    gl::ScopedGlslProg shaderScp( mGeometryShader );
    mGeometryShader->uniform( "uNumSides", mNumSides );
    mGeometryShader->uniform( "uRadius", mRadius );
    
    mBatch->draw();
}

void BoltPlaygroundApp::drawPrism(const vector<vec3>& vertices) const
{
    gl::ScopedGlslProg shaderScp( mBlurShader );
    auto ctx = gl::context();
    GLfloat data[8+8]; // both verts and texCoords
    GLfloat *verts = data, *texs = data + 8;
    
    //
    //        (0,1)       (1,1)
    //        * 0 -------- 2
    //        * |          |
    //        * |          |
    //        * 1 -------- 3
    //        (0,0)        (1,0)
    //

    verts[0*2+0] = vertices[0].x; texs[0*2+0] = 0;
    verts[0*2+1] = vertices[0].y; texs[0*2+1] = 1;
    verts[1*2+0] = vertices[1].x; texs[1*2+0] = 0;
    verts[1*2+1] = vertices[1].y; texs[1*2+1] = 0;
    verts[2*2+0] = vertices[3].x; texs[2*2+0] = 1;
    verts[2*2+1] = vertices[3].y; texs[2*2+1] = 1;
    verts[3*2+0] = vertices[2].x; texs[3*2+0] = 1;
    verts[3*2+1] = vertices[2].y; texs[3*2+1] = 0;
    
    ctx->pushVao();
    ctx->getDefaultVao()->replacementBindBegin();
    
    gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*16 );

    gl::ScopedBuffer bufferBindScp( defaultVbo );
    defaultVbo->bufferSubData( 0, sizeof(float)*16, data );


    int posLoc = mBlurShader->getAttribSemanticLocation( geom::Attrib::POSITION );
    if( posLoc >= 0 ) {
        gl::enableVertexAttribArray( posLoc );
        gl::vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    }
    int texLoc = mBlurShader->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
    if( texLoc >= 0 ) {
        gl::enableVertexAttribArray( texLoc );
        gl::vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );
    }

    ctx->getDefaultVao()->replacementBindEnd();
    ctx->setDefaultShaderVars();
    ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    ctx->popVao();
}

void BoltPlaygroundApp::renderSegmentsFbo() const {
    
    
	vector<vec3> positions {4};
	
	auto segmentRect = [] (const vec2 &start, const vec2 &end,  vector<vec3>& positions) {
		if (positions.empty()) return;
		vec2 direction = end - start;
		vec2 normal = normalize( vec2(-direction.y, direction.x) );
		float halfWidth = 40.0f;
		vec2 offset = normal * halfWidth;
		positions[0] = vec3(start - offset, 0.0f);
		positions[1] = vec3(start + offset, 0.0f);
		positions[2] = vec3(end + offset, 0.0f);
		positions[3] = vec3(end - offset, 0.0f);
	};

	float nX, nY;
	vec2 aV, bV, eV, d, n;
	auto k = 10;
	for (auto& segment : mSegments) {
		aV = mBullets[segment.start].position;
		//auto rand = Rand(getElapsedFrames() + k++);
		auto rand = Rand(++k);
		for (int i=0; i< mBoltDivisions; i++) {
			//float m =  Rand::nextFloat(0.3,0.7);
			const float m = rand.nextFloat(0.1,0.4);
			const float nm = 0.5f - rand.nextFloat(1.0f);
			const float ex = rand.nextFloat(0.25,0.5);
            
			d = mBullets[segment.end].position - aV;
			n = 0.4f * nm * vec2(-d.y, d.x);
			bV = aV + m * d  + n;
			eV = aV + (bV - aV) * (1.0f + ex);
			
			segmentRect(aV, eV, positions);
            drawPrism(positions);
			
			aV = bV;
		}

		segmentRect(aV, mBullets[segment.end].position, positions);
        drawPrism(positions);
	}
   
	gl::color( Color::white() );
}

void BoltPlaygroundApp::drawVba()
{
   
    gl::ScopedGlslProg render( mBoltProg );
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_ELEMENT_ARRAY_BUFFER, 0, mParticleBuffer[mSourceIndex] );
    mBoltProg->uniform( "uModelViewProjection", gl::getModelViewProjection() );
    mBoltProg->uniform( "uTime", static_cast<float>(getElapsedSeconds()) );
    mBoltProg->uniform( "uOffset", mOffset);
    //gl::setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, static_cast<GLsizei>( mSegments.size() ) );
    
}



void BoltPlaygroundApp::drawGui()
{
    /**
     bool                mMouseDown = false;
     bool                mDrawSquares = false;
     int                 mNumCircles = 10;
     int                 mBoltDivisions = 8;*/
    auto fpsString = std::format( "FPS: {}", getAverageFps());
    //gl::drawString(fpsString, vec2(120, 40), Color::white(), Font("Arial", 24) );
    
    ImGui::Begin("Bolt Playground");
    ImGui::Text("%s", fpsString.c_str());
    if (
    ImGui::SliderFloat( "Circle Radius", &mCircleRadius, windowWidth * 0.5f, 1.0f ) ||
        ImGui::SliderInt( "circles number", &mNumCircles, 2, 200 ) ){
            createCircles();
        }
    
    ImGui::SliderInt( "sides", &mNumSides, 4, 20 );
    ImGui::SliderFloat( "radius", &mRadius, 2.0f, 500.0f );
    ImGui::SliderInt( "bolt divisions", &mBoltDivisions, 2, 20 );
    ImGui::SliderFloat( "offset", &mOffset, 0.01f, 5.0f );
        //ImGui::Checkbox( "Draw squares", &mDrawSquares );
    ImGui::Checkbox( "Draw circles", &mDrawCircles );
        //ImGui::Checkbox( "Draw lines", &mDrawLines);
        //ImGui::Checkbox( "Use Geometry Shader", &mUseGeometryShader );
    
    ImGui::Combo( "Draw method", &mRenderOptionsSelection, mRenderOptionsItems, 3 );
    ImGui::End();
}

void BoltPlaygroundApp::update()
{
    
    gl::ScopedFramebuffer fbScp( mBloomFbo );
    gl::clear( ColorA(0, 0, 0, 0) );
    gl::ScopedViewport scpVp( ivec2( 0 ), mBloomFbo->getSize() );
    gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
    
    switch (mRenderOptionsSelection) {
        case 0:
            for (const auto& segment : mSegments) {
                gl::drawLine(mBullets[segment.start].position, mBullets[segment.end].position);
            }
            return;
            break;
        case 1:
            renderSegmentsFbo();
            break;
            
        case 2:
            drawWithGeometrShader();
            break;
            
        case 3:
            drawVba();
            break;
        default:
            
            break;
    }
    
}

void BoltPlaygroundApp::draw() {

    gl::clear(Color(0.0,0.0,8.0));
    //gl::draw(mTexture);
    gl::draw( mBloomFbo->getColorTexture());
   
    if(mDrawCircles) {
        for (auto& bullet : mBullets) {
            Color bulletColor = bullet.pressed ? Color(1.0f, 0.0f, 0.0f) :
            (bullet.hovered ? Color(0.0f, 1.0f, 0.0f) : Color(1.0f, 1.0f, 1.0f));
            //
            gl::color(bulletColor);
            gl::drawSolidCircle(bullet.position, mBulletRadius);
        }
    }

    //gl::color( ColorA( 1, 0, 0, 1 ) );
	//gl::setMatricesWindow( toPixels( getWindowSize() ) );
	
    //gl::color(Color::white());
	//mFbo->bindTexture();

	//// draw a cube textured with the FBO
	//{
	//	gl::ScopedGlslProg shaderScp( mBlurShader );
	//	gl::drawSolidRect(Rectf(mFbo->getBounds()));
	//}
    drawGui();
}

CINDER_APP( BoltPlaygroundApp, RendererGl, [] ( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );

	settings->setWindowSize( BoltPlaygroundApp::windowWidth, BoltPlaygroundApp::windowHeight	 );
} )
