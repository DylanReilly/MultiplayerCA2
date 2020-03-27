//D00137655 - Jason Lynch
//D00194504 - Dylan
#include "CommandQueue.hpp"
#include "SceneNode.hpp"


void CommandQueue::push(const Command& command)
{
	mQueue.push(command);
}

Command CommandQueue::pop()
{
	Command command = mQueue.front();
	mQueue.pop();
	return command;
}

bool CommandQueue::isEmpty() const
{
	return mQueue.empty();
}
