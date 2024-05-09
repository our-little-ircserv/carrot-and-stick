#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Channel
{
	public:
		void addMember(int sd);
		void delMember(int sd);
	
	private:
		std::string name;
		std::string topic;
		int mode;
		std::string password;
		int limits;
		std::vector< int > invited_list;
		std::vector< int > member_list;
		std::vector< int > operator_list;
};

#endif