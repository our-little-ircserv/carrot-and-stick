#ifndef SIGNAL_HPP
# define SIGNAL_HPP

class	Signal
{
	public:
		Signal(int t_signo);
		int	getSignal(void) const;
	
	private:
		int	_signo;
};

#endif
