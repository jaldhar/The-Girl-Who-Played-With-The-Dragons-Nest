#ifndef STATE_H
#define STATE_H

#include <cstdint>

enum class STATE : std::uint8_t { ERROR = 0, COMMAND, DRAW, FIGHTING, MOVING, QUIT };

#endif // STATE_H
