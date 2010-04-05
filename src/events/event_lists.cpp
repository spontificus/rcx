#include "event_lists.hpp"

//make sure header pointers are NULL for the wanted classes
template<> Event_List<Geom> *Event_List<Geom>::head = NULL;
template<> Event_List<Body> *Event_List<Body>::head = NULL;
template<> Event_List<Joint> *Event_List<Joint>::head = NULL;

template<typename T> Event_List<T>::Event_List(T *component)
{
}

template<typename T> T *Event_List<T>::Get_Event()
{
}

template<typename T> void Event_List<T>::Remove(T *component)
{
}
