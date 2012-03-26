#pragma once
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkCommand.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <stdio.h>
#include <vtkAxesActor.h>
#include <vtkAxes.h>

int tri_tri_intersect(double V0[3],double V1[3],double V2[3],double U0[3],double U1[3],double U2[3]);

class Spheres
{
private:
	vtkSmartPointer<vtkSphereSource> sphereSource1;
	vtkSmartPointer<vtkSphereSource> sphereSource2;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkRenderWindow> renderWindow;
public:

	void KeypressCallbackFunction(vtkObject *caller, long unsigned int eventId, void *callData)
	{
		vtkRenderWindowInteractor *interactor = (vtkRenderWindowInteractor*)caller;
		char key = interactor->GetKeyCode();
		
		double c1[3] = {0,0,0};
		double c2[3] = {0,0,0};
		sphereSource1->GetCenter(c1);
		sphereSource2->GetCenter(c2);

		double delta = 0.05;

		switch(key)
		{
		case 'g':
			c1[1] += delta;
			break;
		case 'v':
			c1[1] -= delta;
			break;
		case 'c':
			c1[0] -= delta;
			break;
		case 'b':
			c1[0] +=delta;
			break;
		case 'j':
			c2[1] += delta;
			break;
		case 'm':
			c2[1] -= delta;
			break;
		case 'n':
			c2[0] -= delta;
			break;
		case ',':
			c2[0] +=delta;
			break;
		}
		
		if(this->sphereSource1 == NULL || this->sphereSource2 == NULL)
			return;
		
		//double c1[3] = {0,0,0};
		//double c2[3] = {0,0,0};
		//sphereSource1->GetCenter(c1);
		//sphereSource2->GetCenter(c2);
		//c1[0] += 0.05;
		//c2[0] -= 0.05;
		sphereSource1->SetCenter(c1);
		sphereSource2->SetCenter(c2);

		vtkIdList *list1 = vtkIdList::New();
		vtkIdList *list2 = vtkIdList::New();
		vtkCellArray *arr1 = vtkCellArray::New();
		vtkCellArray *arr2 = vtkCellArray::New();

		sphereSource1->GetOutput()->GetPolys()->InitTraversal();
		sphereSource2->GetOutput()->GetPolys()->InitTraversal();
		while(sphereSource1->GetOutput()->GetPolys()->GetNextCell(list1) != 0)
		{
			double p1[3] = {0,0,0};
			double p2[3] = {0,0,0};
			double p3[3] = {0,0,0};

			// pobierz punkty reprezentujace wierzcholki trojkata
			sphereSource1->GetOutput()->GetPoint(list1->GetId(0), p1);
			sphereSource1->GetOutput()->GetPoint(list1->GetId(1), p2);
			sphereSource1->GetOutput()->GetPoint(list1->GetId(0), p3);

			while(sphereSource2->GetOutput()->GetPolys()->GetNextCell(list2) != 0)
			{
				double q1[3] = {0,0,0};
				double q2[3] = {0,0,0};
				double q3[3] = {0,0,0};

				// pobierz punkty reprezentujace wierzchlki trojkata
				sphereSource2->GetOutput()->GetPoint(list2->GetId(0), q1);
				sphereSource2->GetOutput()->GetPoint(list2->GetId(1), q2);
				sphereSource2->GetOutput()->GetPoint(list2->GetId(2), q3);

				// sprawdz czy trojkaty sie przecinaja			
				if(tri_tri_intersect(p1,p2,p3,q1,q2,q3))
				{
					arr1->InsertNextCell(list1);
					arr2->InsertNextCell(list2);
				}
			}
			sphereSource2->GetOutput()->GetPolys()->InitTraversal();
		}
		
		vtkPolyData *d1 = vtkPolyData::New();
		vtkPolyData *d2 = vtkPolyData::New();
		d1->SetPolys(arr1);
		d2->SetPolys(arr2);
		d1->SetPoints(sphereSource1->GetOutput()->GetPoints());
		d2->SetPoints(sphereSource2->GetOutput()->GetPoints());
		
		vtkPolyDataMapper *m1 = vtkPolyDataMapper::New();
		vtkPolyDataMapper *m2 = vtkPolyDataMapper::New();
		m1->SetInput(d1);
		m2->SetInput(d2);
		
		vtkActor *actor1 = vtkActor::New();
		vtkActor *actor2 = vtkActor::New();
		actor1->GetProperty()->SetColor(1,.3,.3);
		actor2->GetProperty()->SetColor(1,.3,.3);
		actor1->SetMapper(m1);
		actor2->SetMapper(m2);
		
		renderer->AddActor(actor1);
		renderer->AddActor(actor2);
		
		renderWindow->Render();
		
		renderer->RemoveActor(actor1);	
		renderer->RemoveActor(actor2);	
		d1->Delete();
		d2->Delete();
		arr1->Delete();
		arr2->Delete();
		actor1->Delete();
		actor2->Delete();
		list1->Delete();
		list2->Delete();
	}

	Spheres(void)
	{
		sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource1->SetCenter(-3,0,0);
		sphereSource2->SetCenter(-1,0,0);

		vtkSmartPointer<vtkPolyDataMapper> mapper1 
			= vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper1->SetInput(sphereSource1->GetOutput());

		vtkSmartPointer<vtkPolyDataMapper> mapper2
			= vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper2->SetInput(sphereSource2->GetOutput());

		vtkSmartPointer<vtkActor> actor1
			= vtkSmartPointer<vtkActor>::New();
		actor1->SetMapper(mapper1);
		actor1->GetProperty()->SetColor(0,1,0);
		actor1->GetProperty()->SetRepresentationToWireframe();

		vtkSmartPointer<vtkActor> actor2
			= vtkSmartPointer<vtkActor>::New();
		actor2->SetMapper(mapper2);
		actor2->GetProperty()->SetColor(0,0,1);
		actor2->GetProperty()->SetRepresentationToWireframe();

		renderer = vtkSmartPointer<vtkRenderer>::New();
		renderer->SetBackground(1,1,1);
		renderer->SetBackground2(0.5,0.5,0.7);
		renderer->SetGradientBackground(1);
		renderer->AddActor(actor1);
		renderer->AddActor(actor2);

		renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
		renderWindow->AddRenderer(renderer);

		vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor
			= vtkSmartPointer<vtkRenderWindowInteractor>::New();
		renderWindowInteractor->SetRenderWindow(renderWindow);

		// strzalki
		vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
		renderer->AddActor(axes);


		renderWindow->Render();
		//renderWindow->Start();
		renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, this, &Spheres::KeypressCallbackFunction);
		renderWindowInteractor->Start();
	}

	~Spheres(void)
	{
	}
};

