#ifndef STATE_H
#define STATE_H

#include <cstdint>

enum class STATE : std::uint8_t { ERROR = 0, COMMAND, FIGHTING, MOVING, DEAD, QUIT };

#endif // STATE_H
