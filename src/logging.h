#pragma once

#include <iostream>

namespace pubsub {

namespace internal {

std::ostream& _Log();

std::ostream& _Error();

}  // namespace internal

#define Log() internal::_Log() << __FILE__ << ":" << __LINE__ << " "

#define Error() internal::_Error() << __FILE__ << ":" << __LINE__ << " "

void SetLogStream(std::ostream& log);

void SetErrorStream(std::ostream& error);

}  // namespace pubsub

