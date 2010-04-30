//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/26/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../PureCpp/MathCore/MathDeclaration.h"
#include "OpenGLManipulating.h"
#include "OpenGLManipulatingModel.h"

using namespace System;
using namespace System::ComponentModel;


namespace CppCLI
{
	public ref class OpenGLManipulatingController : OpenGLManipulating
	{
	private:
		OpenGLManipulatingModel ^model;
		OpenGLManipulatingModelMesh ^modelMesh;
		OpenGLManipulatingModelItem ^modelItem;
		Vector3D *selectionCenter;
		Quaternion *selectionRotation;
		Vector3D *selectionEuler;
		Vector3D *selectionScale;
		uint selectedCount;
		int lastSelectedIndex;
		ManipulatorType currentManipulator;
		
		Matrix4x4 *modelTransform;
		Vector3D *modelPosition;
		Quaternion *modelRotation;
		Vector3D *modelScale;		

	public:
		OpenGLManipulatingController();
		~OpenGLManipulatingController();

		// OpenGLSelecting

		[Browsable(false)]
		virtual property uint SelectableCount { uint get(); }
		virtual void DrawForSelection(uint index);
		virtual void SelectObject(uint index, OpenGLSelectionMode selectionMode);

		//@optional
		virtual void WillSelect();
		virtual void DidSelect();

		// OpenGLManipulating

		
		[Browsable(false)]
		
		virtual property ManipulatorType CurrentManipulator { ManipulatorType get(); void set(ManipulatorType value); }
		[Browsable(false)]
		virtual property Vector3D SelectionCenter { Vector3D get(); void set(Vector3D value); }
		[Browsable(false)]
		virtual property Quaternion SelectionRotation { Quaternion get(); void set(Quaternion value); }
		[Browsable(false)]
		virtual property Vector3D SelectionScale { Vector3D get(); void set(Vector3D value); }
		[Browsable(false)]
		virtual property uint SelectedCount { uint get(); }

		[Browsable(true)]
		[CategoryAttribute("Rigid Body")]
		virtual property float  Mass { float get();}// void set(float value); }

		[Browsable(true)]
		[CategoryAttribute("Collision Object")]
		virtual property String ^ Name { String ^ get(); void set(String ^ value); }

		[Browsable(true)]
		[CategoryAttribute("Collision Shape")]
		virtual property String ^ CollisionType{ String ^ get(); }

		[Browsable(true)]
		[Category("World Transform")]
		[DescriptionAttribute("Worldspace position of the object")]
		virtual property float PositionX { float get(); void set(float value); }
		[Browsable(true)]
		[Category("World Transform")]
		[DescriptionAttribute("Worldspace position of the object")]
		virtual property float PositionY { float get(); void set(float value); }
		[Browsable(true)]
		[Category("World Transform")]
		[Description("Worldspace position of the object")]
		virtual property float PositionZ { float get(); void set(float value); }
		[Browsable(true)]
		[Category("World Transform")]
		virtual property float RotationX { float get(); void set(float value); }
		[Browsable(true)]
		[Category("World Transform")]
		virtual property float RotationY { float get(); void set(float value); }
		[Browsable(true)]
		[Category("World Transform")]
		virtual property float RotationZ { float get(); void set(float value); }

		
		[Browsable(true)]
		[CategoryAttribute("Collision Shape")]
		virtual property float ScaleY { float get(); void set(float value); }
		[Browsable(true)]
		[CategoryAttribute("Collision Shape")]
		virtual property float ScaleZ { float get(); void set(float value); }
		[Browsable(true)]
		[CategoryAttribute("Collision Shape")]
		virtual property float ScaleX { float get(); void set(float value); }
		virtual void MoveSelectedBy(Vector3D offset);
		virtual void RotateSelectedBy(Quaternion offset);
		virtual void ScaleSelectedBy(Vector3D offset);
		virtual void UpdateSelection();
		virtual void Draw(ViewMode mode);
		virtual void ChangeSelection(CocoaBool isSelected);
		virtual void InvertSelection();
		virtual void CloneSelected();
		virtual void RemoveSelected();

		virtual String^ GetName(uint index);
		virtual void SetName(String^ name, uint index);

		[Browsable(false)]
		property OpenGLManipulatingModel ^Model { OpenGLManipulatingModel ^get(); 
												  void set(OpenGLManipulatingModel ^value); }
		[Browsable(false)]
		property int LastSelectedIndex { int get(); }
		
		float GetSelectionValue(uint index, ManipulatorType manipulatorType);
		void SetSelectionValue(float value, uint index, ManipulatorType manipulatorType);
		void SetTransform(Vector3D position, Quaternion rotation, Vector3D scale);
	};
}
