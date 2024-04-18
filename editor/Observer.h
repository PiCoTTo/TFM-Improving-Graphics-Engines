#pragma once
#include "Subject.h"

template<class T>
class Subject;

template<class T>
class Observer
{
public:
	virtual ~Observer() {}

	virtual void update(T* subject) = 0;
};

