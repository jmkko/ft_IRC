#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
# include <vector>

class Server;
class Client;
// interface of each commands
class ICommand
{
  public:
	virtual ~ICommand() {}
	virtual void execute(Server& s, Client& c) = 0;

  protected:
	ICommand() {}

  private:
	ICommand(const ICommand& inst);

	ICommand& operator=(const ICommand& inst);
};

#endif
