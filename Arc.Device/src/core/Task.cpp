//#include "Task.h"
//#include <algorithm>
//#include "../utils/LogManager.h"
//
//using namespace arc::device::core;
//
//vector<Task*> arc::device::core::Task::instances;
//
//arc::device::core::Task::Task(const char* name, Callback<void()> cb, bool periodic)
//{
//	this->name = string(name);
//	this->state = TaskState::ASLEEP;
//	this->cb = cb;
//	this->periodic = periodic;
//	Task::instances.push_back(this);
//}
//
//arc::device::core::Task::~Task()
//{
//	Task::instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
//}
//
//void arc::device::core::Task::run()
//{
//	state = TaskState::ALIVE;
//	cb.call();
//	if (!periodic)
//	{
//		state = TaskState::ASLEEP;
//	}
//}
//
//const char * arc::device::core::Task::getName()
//{
//	return name.c_str();
//}
