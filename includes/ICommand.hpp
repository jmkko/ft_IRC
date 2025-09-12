#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
# include <iostream>

class ICommand
{
    public:
        ICommand(void);
        ICommand(const ICommand& other);
        ICommand &operator=(const ICommand &other);
        ~ICommand();
};

#endif

