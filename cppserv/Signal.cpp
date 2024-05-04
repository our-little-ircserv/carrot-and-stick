#include <signal.h>
#include "Signal.hpp"

Signal::Signal(int _signo) : m_signo(_signo)
{
}

int	Signal::getSignal(void) const
{
	return m_signo;
}
