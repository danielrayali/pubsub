#pragma once

#include <iostream>

namespace pubsub {

std::ostream& Log();

std::ostream& Error();

void SetLogStream(std::ostream& log);

void SetErrorStream(std::ostream& error);

}  // namespace pubsub

