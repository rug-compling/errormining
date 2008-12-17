#ifndef _OBSERVABLE_HH
#define _OBSERVABLE_HH

#include <algorithm>
#include <memory>
#include <vector>

namespace errormining
{

class Observer;

/**
 * This class implements the observer pattern (Gamma et al.). Typically,
 * a class derrives from Observable, and calls notify() to make all
 * observers aware of an event. While this is too course-grained for most
 * signal handling, it's an ok lightweight solution for objects that
 * emit just one kind of signal without arguments.
 */
class Observable
{
	std::auto_ptr<std::vector<Observer *> > d_observers;
public:
	/**
	 * Construct an observable class.
	 */
	Observable() : d_observers(new std::vector<Observer *>()) {}
	
	/**
	 * Attach an observer to this class.
	 */
	virtual void attach(Observer *observer);

	/**
	 * Detach an observer from this class. If the Observer was added
	 * more than once, all attachments of this Observer will be removed.
	 */	
	virtual void detach(Observer *observer);
	
	/**
	 * Notify all observers.
	 */
	virtual void notify();

	virtual ~Observable() {};
};

inline void Observable::attach(Observer *observer)
{
	d_observers->push_back(observer);
}

inline void Observable::detach(Observer *observer)
{
	std::remove(d_observers->begin(), d_observers->end(), observer);
}

}

#endif // _OBSERVABLE_HH
