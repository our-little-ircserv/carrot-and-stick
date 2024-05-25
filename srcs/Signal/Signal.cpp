#include <signal.h>
#include "Signal.hpp"

Signal::Signal(int t_signo) : _signo(t_signo)
{
}

int	Signal::getSignal(void) const
{
	return _signo;
}
