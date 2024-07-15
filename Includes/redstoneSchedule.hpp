#ifndef REDSTONESCHEDULE_HPP
# define REDSTONESCHEDULE_HPP

#include "utils.h"
class Chunk;

namespace schedule {
    namespace priority {
        const int repeat_diode = 0; // priority == -3 if repeater outputs in back or side of other diode
        const int repeat_off = 1; // priority == -2 if repeater turns off
        const int repeat_on = 2; // priority == -1 if repeater turns on
        const int comparator = 3; // priority == -0.5 for comparator update
        const int other = 4; // priority == 0 for rest of redstone schedule
        const size_t size = 5;
    };

    typedef struct s_redstone {
        glm::ivec3 pos = {0, 0, 0}; // offset of block inside chunk
        int ticks = 0;              // size of delay
        int state = redstone::off;
    }				t_redstoneTick;

    typedef struct s_redstoneSchedule {
        Chunk* chunk;
        t_redstoneTick schedule;
    }               t_redstoneSchedule;

    bool abortRepeaterScheduleTick( int priority, t_redstoneSchedule red );
    void abortComparatorScheduleTick( Chunk* chunk, glm::ivec3 pos );
    void scheduleRedstoneTick( int priority, int targetValue, t_redstoneSchedule red );
    void extendRepeaterDelay( int priority, int value, t_redstoneSchedule red );

    void tickUpdate( void );

    void deleteChunkSchedule( std::vector<std::shared_ptr<Chunk>>& deleted_chunks );
};

#endif
