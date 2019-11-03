#include "Animations.h"

Animations::Animations()
{

	currentanimframe = animationframes.getFirst();
	animationfinished = false;
	numberofFrames = 0;
	animationloop = false;
	animationname = "";
	//texture = nullptr;
	//texture = nullptr;
}

Animations::~Animations()
{

	p2List_item<FrameInfo*>* item;
	item = animationframes.getFirst();
	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	delete currentanimframe;
	animationframes.clear();

}

void Animations::AddFrame(int duration, SDL_Rect texturerect, iPoint textureoffset)
{
	FrameInfo* newframe = new FrameInfo;

	newframe->frameduration = duration;
	newframe->textureoffset = textureoffset;
	newframe->animationRect = texturerect;
	newframe->actualduration = 0;
	animationframes.add(newframe);
	numberofFrames++;
	currentanimframe = animationframes.getFirst();
}

FrameInfo* Animations::StepAnimation()
{
	FrameInfo* ret = currentanimframe->data;
	if (ret->actualduration++ >= ret->frameduration)//only executes the code once the duration of the frame is max
	{
		ret->actualduration = 0;//restarts the duration

		if (currentanimframe->next != nullptr)//if the next element exists go to the next element
			currentanimframe = currentanimframe->next;
		else if (animationloop)//if the animation can loop return to the start
			currentanimframe = animationframes.getFirst();
		else animationfinished = true;


	}



	return ret;
}

bool Animations::GetAnimationFinish()
{
	if (animationfinished)
	{
		animationfinished = false;
		return true;
	}
	return false;
}

FrameInfo* Animations::GetFrame(int id)
{
	FrameInfo* aux;
	animationframes.at(id, aux);
	return aux;
}


p2SString Animations::GetName()
{
	return animationname;
}
p2List_item<Animations*>* Animations::GetAnimFromName(p2SString name, p2List<Animations*>* animlist)
{
	p2List_item<Animations*>* animitem = animlist->getFirst();
	while (animitem != NULL)
	{
		if (animitem->data->GetName() == name)
		{
			break;
		}
		animitem = animitem->next;
	}
	return animitem;
}

void Animations::ResetAnimation()
{
	p2List_item<FrameInfo*>* item = animationframes.getFirst();
	while (item != NULL)
	{
		item->data->actualduration = 0;//resets the duration of each frame.
		item = item->next;
	}



	currentanimframe = animationframes.getFirst();
	animationfinished = false;
}