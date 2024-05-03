#include <signal.h>
#include "Signal.hpp"

Signal::Signal(int _signo) : signo(_signo)
{
}

int	Signal::getSignal(void) const
{
	return signo;
}
