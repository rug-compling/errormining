#ifndef _OBSERVER_HH
#define _OBSERVER_HH

namespace errormining
{

class Observable;

/**
 * This class implements the observer pattern (Gamma et al.). A class
 * that is derrived from Observable can register with and receive signals
 * from Observable classes.
 */
class Observer
{
public:
	/**
	 * This method is called by an observable when a event takes place.
	 */
	virtual void update(Observable const *observable) = 0;

	virtual ~Observer() {}
};

}

#endif // _OBSERVER_HH
