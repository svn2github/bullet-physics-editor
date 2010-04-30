/*
 *  ExperimentalBulletWrapper.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 04/02/10.
 *  For license see LICENSE.TXT
 *
 */

#include "ExperimentalBulletWrapper.h"
#include "MarshalHelpers.h"
using namespace std;

namespace CppCLI
{
	ExperimentalBulletWrapper::ExperimentalBulletWrapper()
	{
		wrapper = new BulletWrapperHelper();
	}

	void ExperimentalBulletWrapper::Load(String ^fileName)
	{
		delete wrapper;
		wrapper = new BulletWrapperHelper();
		
		string nativeFileName = NativeString(fileName);
		wrapper->Load(nativeFileName.c_str());
	}

	void ExperimentalBulletWrapper::StepSimulation(btScalar timeStep)
	{
		wrapper->dynamicsWorld->stepSimulation(timeStep);
	}

	void ExperimentalBulletWrapper::Save(String ^fileName)
	{
		string nativeFileName = NativeString(fileName);
		wrapper->Save(nativeFileName.c_str());
	}

	#pragma region OpenGLManipulatingModel implementation

	void ExperimentalBulletWrapper::Draw(ViewMode mode)
	{
		if (mode == ViewMode::ViewModeSolid)
			wrapper->DrawSolid();
		else
			wrapper->DrawWireframe();
	}

	void ExperimentalBulletWrapper::Draw(uint index, CocoaBool forSelection, ViewMode mode)
	{
		wrapper->Draw(index, !forSelection && this->IsSelected(index));
	}

	uint ExperimentalBulletWrapper::Count::get()
	{
		return (uint)wrapper->dynamicsWorld->getNumCollisionObjects();
	}

	CocoaBool ExperimentalBulletWrapper::IsSelected(uint index)
	{
		return wrapper->selection->at(index);
	}

	void ExperimentalBulletWrapper::SetSelected(CocoaBool selected, uint index)
	{
		wrapper->selection->at(index) = selected;
	}

	void ExperimentalBulletWrapper::CloneSelected() 
	{ 
	
	}

	void ExperimentalBulletWrapper::RemoveSelected() 
	{
		wrapper->RemoveSelected();
	}

	void ExperimentalBulletWrapper::WillSelect() 
	{
	
	}

	void ExperimentalBulletWrapper::DidSelect() 
	{
	
	}

	Vector3D ExperimentalBulletWrapper::GetPosition(uint index)
	{
		return wrapper->GetPosition(index);
	}

	Quaternion ExperimentalBulletWrapper::GetRotation(uint index)
	{
		return wrapper->GetRotation(index);
	}

	Vector3D ExperimentalBulletWrapper::GetScale(uint index)
	{
		return Vector3D(1, 1, 1); // ignored
	}

	void ExperimentalBulletWrapper::SetPosition(Vector3D position, uint index)
	{
		wrapper->SetPosition(position, index);
	}

	void ExperimentalBulletWrapper::SetRotation(Quaternion rotation, uint index)
	{
		wrapper->SetRotation(rotation, index);
	}

	void ExperimentalBulletWrapper::SetScale(Vector3D scale, uint index)
	{
		// ignored
	}

	void ExperimentalBulletWrapper::MoveBy(Vector3D offset, uint index)
	{
		Vector3D position = this->GetPosition(index);
		position += offset;
		this->SetPosition(position, index);
	}

	void ExperimentalBulletWrapper::RotateBy(Quaternion offset, uint index)
	{
		Quaternion rotation = this->GetRotation(index);
		rotation = offset * rotation;
		this->SetRotation(rotation, index);
	}

	void ExperimentalBulletWrapper::ScaleBy(Vector3D offset, uint index)
	{
		// ignored
	}

	

	String ^ExperimentalBulletWrapper::GetShapeType(uint index)
	{
		return ManagedString(wrapper->GetShapeType(index));
	}

	
	float ExperimentalBulletWrapper::GetMass(uint index)
	{
		return wrapper->GetMass(index);
	}

	void ExperimentalBulletWrapper::SetMass(float value, uint index)
	{
		wrapper->SetMass(value, index);
	}

	String ^ExperimentalBulletWrapper::GetName(uint index)
	{
		return ManagedString(wrapper->GetName(index));
	}

	void ExperimentalBulletWrapper::SetName(String ^name, uint index)
	{
		wrapper->SetName(NativeString(name), index);
	}

	#pragma endregion
}