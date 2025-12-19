#include "Resources.hpp"


#include <format>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/CameraUi.h"
#include "cinder/Sphere.h"
#include "cinder/CinderImGui.h"


#include "glm/glm.hpp"
#include "cinder/ip/Checkerboard.h"

/*
*
 https://www.monolithsoft.co.jp/techblog/articles/000629.html
 translated:
 https://www-monolithsoft-co-jp.translate.goog/techblog/articles/000629.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=pt-PT&_x_tr_pto=wapp
1 Give the initial torque ( N ).
2 Angular acceleration is calculated by solving Euler's equation of rotational motion in the rigid body's local coordinate system.

Based on the angular acceleration, the angular velocity ( ω ) and the rotation (orientation) of the rigid body are calculated.

3. Set (integrate) the angular velocity ( ω ) and the rigid body rotation (orientation) calculated in step 3 as new values.

Reset the torque ( N ).

Return to 2.


 */

using namespace ci;
using namespace ci::app;
using namespace std;

class Triangle {
public:
	Triangle(const vec3& a, const vec3& b, const vec3& c): r1(a), r2(b), r3(c), A(r1, r2, r3) {
		detA = glm::determinant(A);
	};

	[[nodiscard]] std::string String() const {
		return std::format("detA {}\n",detA);
	};

	[[nodiscard]] float Determinant() const {
		return detA;
	};

	[[nodiscard]] vec3 R1() const { return r1; };
	[[nodiscard]] vec3 R2() const { return r2; };
	[[nodiscard]] vec3 R3() const { return r3; };

private:
	vec3 r1, r2, r3;
	glm::mat3x3 A;
	float  detA;
};

class BoltPlaygroundApp : public App {
  public:
	void	setup() override;
	void	update() override;
	void	draw() override;

  private:

    void                    reset();
    TriMeshRef              loadObj( const DataSourceRef &dataSource );
    std::vector<Triangle>   triangulate(const TriMeshRef &mesh);
    void                    createGrid();
    void                    calculateBodyInertia();
    glm::mat3x3             getTriangleInerciaTensor(const Triangle& triangle, const vec3 &centerMass);

	CameraUi		mCamUi;
	CameraPersp		mCam;
	TriMeshRef		mMesh;
	gl::BatchRef	mBatch;
	gl::GlslProgRef	mGlsl;
	gl::VertBatchRef	mGrid;
	gl::TextureRef	mCheckerTexture;
	std::vector<Triangle> mTriangles = {};
	vec3			mCenterMass = vec3(0.0f);
	glm::mat<3,3,double>     mInertiaTensor = glm::mat<3,3,double> (0.0);
	
   
	vec3			mLastPos;
	double			mLastTime;
	double			mVolume = 0.0;
	double			mMass = 50.0f;
    
	glm::vec<3, double>			mTorque =   glm::vec<3, double>(0.0);
    glm::vec<3, double>			mW =   glm::vec<3, double>(0.0);
    glm::qua<double>            mQuat;
    double          mYMult = 66000000;
    double          mZMult = 3000000000000;
};

void BoltPlaygroundApp::setup()
{
    
    ImGui::Initialize( ImGui::Options().window( getWindow() ).enableKeyboard( true ) );
    ImGui::GetStyle().ScaleAllSizes( getWindowContentScale() ); // for Retina / hi-dpi
    ImGui::GetStyle().FontScaleMain = getWindowContentScale();
    
	auto vertShader = loadResource( RES_SHADER_VERT );
	auto fragShader = loadResource( RES_SHADER_FRAG );
	mGlsl = gl::GlslProg::create( vertShader, fragShader );
	mGlsl->uniform( "uTex0", 0 );
	
    mCam.setPerspective( 50.0f, getWindowAspectRatio(), 0.1, 1000 );
	mCamUi = CameraUi( &mCam );

	mCheckerTexture = gl::Texture::create( ip::checkerboard( 512, 512, 32 ) );
	mCheckerTexture->bind( 0 );

	mMesh = loadObj( loadResource( RES_8LBS_OBJ ) );
    mBatch = gl::Batch::create( *mMesh, mGlsl );
    assert(mMesh);
    mTriangles =  triangulate(mMesh);
    calculateBodyInertia();
    
	createGrid();

	mLastTime = getElapsedSeconds();
    //reset();
}

void BoltPlaygroundApp::reset()
{
    mQuat = glm::qua<double>();
    mTorque = glm::vec<3, double>(0.0, M_2_PI + mMass * mYMult, mZMult );
    mW =   glm::vec<3, double>(0.0);
}


glm::mat3x3    BoltPlaygroundApp::getTriangleInerciaTensor(const Triangle& triangle, const vec3 &centerMass) {
	using namespace glm;
	auto v1 = triangle.R1() - centerMass;
	auto v2 = triangle.R2() - centerMass;
	auto v3 = triangle.R3() - centerMass;
	float rho = 1000.0f; // density
	float a = rho + (1.0f/60.0f) * triangle.Determinant() * (v1.y * v1.y + v1.y * v2.y + v1.y * v3.y + v2.y * v2.y + v2.y * v3.y + v3.y * v3.y
		+ v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v1.z * v3.z + v2.z * v3.z + v3.z * v3.z);
	float b = rho + (1.0f/60.0f) * triangle.Determinant() * (v1.x * v1.x + v1.x * v2.x + v1.x * v3.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x
		+ v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v1.z * v3.z + v2.z * v3.z + v3.z * v3.z);
	float c = rho + (1.0f/60.0f) * triangle.Determinant() * (v1.x * v1.x + v1.x * v2.x + v1.x * v3.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x
		+ v1.y * v1.y + v1.y * v2.y + v2.y * v2.y + v1.y * v3.y + v2.y * v3.y + v3.y * v3.y);
	float ap = (1.0f/120.0f) * triangle.Determinant() * ( 2.0f * v1.z * v1.x + v1.z * v2.x + v1.z * v3.x
		+ 2.0f * v2.z * v2.x + v2.z * v1.x + v2.z * v3.x
		+ 2.0f * v3.z * v3.x + v3.z * v1.x + v3.z * v2.x);
	float bp = (1.0f/120.0f) * triangle.Determinant() * ( 2.0f * v1.x * v1.y + v1.x * v2.y + v1.x * v3.y
		+ 2.0f * v2.x * v2.y + v2.x * v1.y + v2.x * v3.y
		+ 2.0f * v3.x * v3.y + v3.x * v1.y + v3.x * v2.y);
	float cp = (1.0f/120.0f) * triangle.Determinant() * ( 2.0f * v1.y * v1.z + v1.y * v2.z + v1.y * v3.z
		+ 2.0f * v2.y * v2.z + v2.y * v1.z + v2.y * v3.z
		+ 2.0f * v3.y * v3.z + v3.y * v1.z + v3.y * v2.z);

	return {
		 {a, -cp, -bp},
			{-cp, b, -bp},
					{-bp, -ap, c} };
}

TriMeshRef BoltPlaygroundApp::loadObj( const DataSourceRef &dataSource )
{
	ObjLoader loader( dataSource );
	auto mesh  = TriMesh::create( loader );
	auto normals = mesh->getNormals();

	cout << "Loaded mesh with " << mesh->getNumVertices() << " vertices, " << mesh->getNumIndices() / 3 << " triangles, "
		<< ( normals.empty() ? 0 : normals.size()  ) << " normals." << std::endl;

	if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
		mesh->recalculateNormals();
    return mesh;
}

vector<Triangle> BoltPlaygroundApp::triangulate(const TriMeshRef& mesh)
{
    vector<Triangle> triangles = {};
    if (mesh->getNumIndices() > 0 && mesh->getNumIndices() % 3 == 0) {
        for (int i = 0; i <  mesh->getIndices().size(); i+=3){
            uint32_t idx1 = mesh->getIndices()[i];
            uint32_t idx2 = mesh->getIndices()[i+1];
            uint32_t idx3 = mesh->getIndices()[i+2];
            vec3 v1 = mesh->getPositions<3>()[idx1];
            vec3 v2 = mesh->getPositions<3>()[idx2];
            vec3 v3 = mesh->getPositions<3>()[idx3];
            triangles.emplace_back(v1, v2, v3 );
        }
    }
    return triangles;
}

void BoltPlaygroundApp::calculateBodyInertia()
{
    mVolume= 0.0f;
    mCenterMass= vec3(0.0f);
    for (const auto &t : mTriangles) {
        auto vol = t.Determinant() /6.0f;
        mVolume += vol;
        mCenterMass += vol * ( (t.R1() + t.R2() + t.R3()) / 4.0f );

    }

    mCenterMass /= mVolume > 0 ?  mVolume : 1.0f;
    mInertiaTensor = glm::mat<3,3,double> (0.0f);
    for (const auto &t : mTriangles) {
        auto tensor = getTriangleInerciaTensor(t, mCenterMass);
        mInertiaTensor += tensor;
    }
    mMass = mVolume * 1000; // density 1000 kg/m^3
}

void BoltPlaygroundApp::createGrid()
{
	mGrid = gl::VertBatch::create( GL_LINES );
	mGrid->begin( GL_LINES );
	float width =  4.0f;
	float height = 4.0f;
	const float  n = 20;
	vec3 center = vec3(0.0f, -20.0f, 0.0f);
	for( int i = -n; i <= n; ++i ) {
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );

		mGrid->vertex( center.x + static_cast<float>(i) * height, center.y, center.z - n * width);
		mGrid->vertex( center.x + static_cast<float>(i) * height, center.y, center.z + n * width);
		mGrid->vertex( center.x - n * width, center.y, center.z + static_cast<float>(i)  * width);
		mGrid->vertex( center.x + n * width, center.y, center.z + static_cast<float>(i) * width);
	}
	mGrid->end();
}

/*

function vector dw(const vector torque, w; const matrix3 I) {
	// Euler's equations
	//  N = (dL/dt)space = (dL/dt)body + cross(w, L)
	return invert(I) * (torque - cross(w, I * w));
}
vector  t  = v@torque;
vector  w  = @w;
vector4 q  = @orient;
matrix3 I  = 3@itensor * @mass;
w = qrotate(qinvert(q), w);
vector  kw1  = w;
vector4 kq1  = q;
vector  kdw1 = dw(qrotate(qinvert(kq1), t), kw1, I);>

vector  kw2  = w + kdw1 * @TimeInc * 0.5;
vector4 kq2  = qmultiply(q, quaternion(kw1 * @TimeInc * 0.5));
vector  kdw2 = dw(qrotate(qinvert(kq2), t), kw2, I);

vector  kw3  = w + kdw2 * @TimeInc * 0.5;
vector4 kq3  = qmultiply(q, quaternion(kw2 * @TimeInc * 0.5));
vector  kdw3 = dw(qrotate(qinvert(kq3), t), kw3, I);

vector  kw4  = w + kdw3 * @TimeInc;
vector4 kq4  = qmultiply(q, quaternion(kw3 * @TimeInc));
vector  kdw4 = dw(qrotate(qinvert(kq4), t), kw4, I);

vector kdw = (kdw1 + kdw2 * 2 + kdw3 * 2 + kdw4) * @TimeInc / 6;
vector kw  = (kw1  + kw2  * 2 + kw3  * 2 + kw4)  * @TimeInc / 6;
v@torque = set(0, 0, 0);
@orient  = qmultiply(q, quaternion(kw));
@w       = qrotate(@orient, w + kdw);
 
 
 Runge Kutta 4th order
 dy/dt = f(t, y), y(t_0) = y_0
 
 y_n+1 = y_n + h/6(k1 + k2 + k3 + k4)
 t_n+1 = t_n + delta
 
 k1 = f(t_n, y_n)
 k2 = f(t_n + delta / 2, y_n + delta * k1 / 2)
 k3 = f(t_n + delta / 2, y_n + delta * k2 / 2)
 k4 = f(t_n + delta, y_n + delta * k3 )

 ---------------------------
 Looking to solve
 
 dw/dt = f(t, w) = I^-1 * (N - w x I*w )
 w = dq/dt
 q = q_0 + delta * w

 */
void BoltPlaygroundApp::update()
{
	double elapsed = getElapsedSeconds() - mLastTime;
	mLastTime = getElapsedSeconds();
	auto t = mTorque;;
	auto w = mW;
	auto q = mQuat;
	auto I = mInertiaTensor * mMass;

	auto qi = inverse(q);
    auto dw = [](const glm::vec<3,double>& t, const glm::vec<3,double>& w, const glm::mat<3,3,double>& I) {
		return glm::inverse(I) * (t - glm::cross(w, I * w));
	};
    
    w = qi * mW;
	auto kw1 = w;
    auto kq1 = q;
	auto kdw1 = dw(inverse(kq1) * t,w,I);

	auto kw2  = w + kdw1 * elapsed * 0.5;
	auto kq2  = q * glm::qua<double>(kw1 * elapsed * 0.5);
	auto kdw2 = dw(inverse(kq2) * t, kw2, I);

	auto kw3  = w + kdw2 * elapsed * 0.5;
	auto kq3  = q * glm::qua<double>(kw2 * elapsed * 0.5);
	auto kdw3 = dw(inverse(kq3) * t, kw3, I);

	auto kw4  = w + kdw3 * elapsed;
	auto kq4  = q * glm::qua<double>(kw3 * elapsed);
	auto kdw4 = dw(inverse(kq4) * t, kw4, I);

	auto kdw = (kdw1 + kdw2 * 2.0 + kdw3 * 2.0 + kdw4) * elapsed / 6.0;
	auto kw  = (kw1  + kw2  * 2.0 + kw3  * 2.0 + kw4)  * elapsed / 6.0;

	mTorque = glm::vec<3, double>(0.0);
	mQuat   = glm::qua<double>(kw) * q;
	mW      = mQuat * (w + kdw);
}



void BoltPlaygroundApp::draw() {
	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::clear(Color(0.4f, 0.6f, 0.9f));
	gl::setMatrices( mCam );
	gl::pushMatrices();
		gl::rotate( mQuat );
        mBatch->draw();
    gl::popMatrices();
    gl::disableDepthWrite();
    if( mGrid ) {
        gl::ScopedGlslProg scopedGlslProg( gl::context()->getStockShader( gl::ShaderDef().color() ) );
        mGrid->draw();
        gl::drawCoordinateFrame( 2 );
    }
    gl::disableDepthRead();
    auto fpsString = std::format( "FPS: {}", getAverageFps());

    ImGui::Begin("Space Wrench");
    ImGui::Text("%s", fpsString.c_str());
    double yMin = 1000.0;
    double yMax = 1000000000.0;
    double zMin = 100000000.0;
    double zMax = 100000000000000.0;
    ImGui::Text("Torque for Y, Z");
    ImGui::SliderScalar("Y Multiplicator", ImGuiDataType_Double, &mYMult, &yMin, &yMax);
    ImGui::SliderScalar("Z Multiplicator", ImGuiDataType_Double, &mZMult, &zMin, &zMax);
    if (ImGui::Button("Reset")) {
        reset();
    }
    ImGui::End();
}


CINDER_APP( BoltPlaygroundApp, RendererGl, [] ( App::Settings *settings ) {} )

