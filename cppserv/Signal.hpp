#ifndef SIGNAL_HPP
# define SIGNAL_HPP

class	Signal
{
	public:
		Signal(int _signo);
		int	getSignal(void) const;
	
	private:
		int	signo;
};

#endif
