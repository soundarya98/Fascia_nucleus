#ifndef COMMANDS
#define COMMANDS

enum State 
{
    WAIT = 0,
    STREAMING = 1
};

enum Commands
{
    START_STREAM = 'b',
    STOP_STREAM =  's'
};

#endif
