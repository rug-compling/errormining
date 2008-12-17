#include "Observable.ih"

void Observable::notify()
{
	for(vector<Observer *>::iterator iter = d_observers->begin();
			iter != d_observers->end(); ++iter)
		(*iter)->update(this);
}
