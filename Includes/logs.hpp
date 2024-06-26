#ifndef LOGS_HPP
# define LOGS_HPP

# include <iostream>
# include <DayCycle.hpp>

# define LOG(x)         std::cout << x << std::endl
# define LOGERROR(x)    std::cerr << x << std::endl
# define TLOG(x)        std::cout << "[" << DayCycle::Get()->getGameTicks() << "] " << x << std::endl
# define REDLOG(x)      std::cout << "[REDSTONE] [" << DayCycle::Get()->getGameTicks() << "] " << x << std::endl

/**
 * macros used to able/disable log of a whole section
 * ie all log related to pistons can be turned on and off from the macro PISTONLOG
 * #define MACRO(x) x    <-- ON
 * #define MACRO(x)      <-- OFF
 */

# define MAINLOG(x)     x
# define PISTONLOG(x)
# define COMPLOG(x)
# define REPEATLOG(x)
# define REDUPLOG(x)
# define BLOCKLOG(x)
# define FURNACELOG(x)
# define FLUIDLOG(x)
# define TICKUPLOG(x)
# define BENCHLOG(x)	x

/**
 * macros used to facilitate coordinates output
 */

# define POS(pos)             pos.x << ", " << pos.y << ", " << pos.z
# define POS2(pos, delta)      pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z
# define POSXYZ(pos, dx, dy, dz) pos.x + dx << ", " << pos.y + dy << ", " << pos.z + dz

#endif
