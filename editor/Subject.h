#pragma once
#include "Observer.h"
#include <memory>
#include <vector>
#include <algorithm>


template<class T>
class Observer;

template <class T>
class Subject
{
public:
	virtual ~Subject();

	virtual void addObserver(Observer<T>* observer);
	virtual void removeObserver(Observer<T>* observer);
	virtual void removeAllObservers();
	void notify();

private:
	std::vector<Observer<T>*> m_observers;
};


template <class T>
Subject<T>::~Subject()
{
}


template <class T>
void Subject<T>::addObserver(Observer<T>* observer)
{
	m_observers.push_back(observer);
	notify();
}


template <class T>
void Subject<T>::removeObserver(Observer<T>* observer)
{
	auto o = std::find(m_observers.begin(), m_observers.end(), observer);
	if (o != m_observers.end())
		m_observers.erase(o);
}


template <class T>
void Subject<T>::removeAllObservers()
{
	m_observers.clear();
}


template <class T>
void Subject<T>::notify()
{
	for (const auto& ptrObserver : m_observers)
		ptrObserver->update(static_cast<T*>(this));
}
