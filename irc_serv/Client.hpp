#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

class Client
{
	public:
		//
	
	private:
		int sd;
		std::string name;
		std::string nickname;
		std::string host;
		std::string read_buf;
		std::vector< std::string > write_buf;
};

#endif