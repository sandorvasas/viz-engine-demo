#include "GameObject.h"
#include "Geometry\Model_MD5.h"
#include "Geometry\ModelManager.h"
#include "Physics\PhysicsManager.h"
#include "Physics\Physics.h"
#include "Player.h"

vsGameObject::vsGameObject( void ) {
	physics = NULL; 
	model = NULL;
}

vsGameObject::~vsGameObject( void ) {
	modelManager->DeleteMesh( model );
	physicsManager->Delete( physics );
}

void vsGameObject::Render( bool frustumTest ) {
	
	if ( model == NULL ) 
		return;

	if ( physics ) {
		physicsManager->ssObjects.Enter();
		model->SetOffset( physics->GetCenterOfMass() );
		physicsManager->ssObjects.Leave();
	}

	if ( frustumTest ) 
		visible = player->GetFrustum()->IntersectBox( model->GetBounds(), 0.002f );
	else
		visible = true;

	if ( visible ) 
		if ( model->HasAnimation() ) {
			model->Animate( 3.0f * renderSystem->FPSCounter.GetInverseCurrent() );
			model->RenderAnimated();
			physics->SetCenterOfMass( model->GetOffset() );
		} else {
			model->RenderBindpose();
		}
}

void vsGameObject::SetOrigin( const vsVec3f& o )
{
	if ( physics )	physics->SetCenterOfMass( o );
	if ( model )	model->SetOffset( o );
}

void vsGameObject::SaveToFile( vsOutputFile * f )
{
	physics->SaveToFile( f );
	f->WriteBool( & visible );
}

void vsGameObject::RestoreFromFile( vsInputFile * f )
{
	physics->RestoreFromFile( f );
	f->ReadBool( & visible );
}