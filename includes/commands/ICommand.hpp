#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
# include <iostream>


// interface of each commands
class ICommand
{
    public:
        ICommand(void);
        ICommand(const ICommand& other);
        ICommand &operator=(const ICommand &other);
        ~ICommand();
};

#endif

