#include "redstoneSchedule.hpp"
# include "Chunk.hpp"
#include "logs.hpp"

namespace schedule {
	std::array<std::vector<t_redstoneSchedule>, priority::size> redstoneSchedule;

    /**
     * @brief loop through redstoneSchedule and reduce ticks by 1,
     * if ticks reach 0, scheduled redstone componant is updated
     */
    void tickUpdate( void )
    {
        const std::array<size_t, priority::size> sizes = {redstoneSchedule[priority::repeat_diode].size(), redstoneSchedule[priority::repeat_off].size(), redstoneSchedule[priority::repeat_on].size(), redstoneSchedule[priority::comparator].size(), redstoneSchedule[priority::other].size()};
        for (size_t schedIndex = 0; schedIndex < priority::size; ++schedIndex) {
            if (!sizes[schedIndex]) {
                continue ;
            }

            size_t delCount = 0;
            for (size_t index = 0; index < sizes[schedIndex]; ++index) {
                t_redstoneSchedule& redRef = redstoneSchedule[schedIndex][index - delCount];
                if (--redRef.schedule.ticks == 0) {
                    redRef.chunk->updateRedstone(schedIndex, redRef.schedule);

                    redstoneSchedule[schedIndex].erase(redstoneSchedule[schedIndex].begin() + (index - delCount));
                    ++delCount;
                }
            }
        }
    }

    /**
     * @brief check if there's no conflict with current schedule and
     * push_back red to schedule.
     */
    void scheduleRedstoneTick( int priority, int targetValue, t_redstoneSchedule red )
    {
        for (auto &sched : redstoneSchedule[priority]) {
            if (sched.chunk != red.chunk) {
                continue ;
            }
            if (sched.schedule.pos == red.schedule.pos) {
                if (sched.schedule.ticks == red.schedule.ticks) {
                    if (sched.schedule.state == red.schedule.state) {
                        REDUPLOG(LOG("aborted."));
                    } else if (TYPE(targetValue) == blocks::comparator) {
                        sched.schedule.state = red.schedule.state;
                        REDUPLOG(LOG("abort previous state."));
                    }
                    return ;
                } else if (TYPE(targetValue) == blocks::observer && sched.schedule.state == redstone::off) {
                    REDUPLOG(REDLOG("observer aborted."));
                    return ;
                }
            }
        }
        redstoneSchedule[priority].push_back(red);
    }

    /**
     * @brief check if repeater being updated already has schedule / has been
     * (un)locked thif frame
     * @return true if schedule is aborted
     */
    bool abortRepeaterScheduleTick( int priority, t_redstoneSchedule red )
    {
        for (size_t index = 0; index < redstoneSchedule[priority].size(); ++index) {
			t_redstoneSchedule sched = redstoneSchedule[priority][index];
			if (sched.chunk == red.chunk && sched.schedule.pos == red.schedule.pos && sched.schedule.ticks == red.schedule.ticks) {
				if (sched.schedule.state & mask::redstone::repeater::lock) {
					REDUPLOG(LOG("abort because lock/unlock schedule this frame."));
					return (true);
				} else {
					REDUPLOG(LOG("[next new schedule:] abort previous instruction of priority (" << (-3 + priority) << ") " << sched.schedule.state));
					redstoneSchedule[priority].erase(redstoneSchedule[priority].begin() + index);
				}
				break ;
			}
		}
        return (false);
    }

    /**
     * @brief abort previous schedule of comparator at pos if found
     */
    void abortComparatorScheduleTick( Chunk* chunk, glm::ivec3 pos )
    {
        size_t index = 0;
		for (auto &sched : redstoneSchedule[priority::repeat_on]) { // TODO I guess this should be priority::comparator instead ??
			REDUPLOG(LOG("abortComparatorScheduleTick checking " << sched.chunk << ": " << POS(sched.schedule.pos) << " ticks " << sched.schedule.ticks));
			if (sched.chunk == chunk && sched.schedule.pos == pos && sched.schedule.ticks == redstone::tick) {
				REDUPLOG(LOG("abort previous comparator schedule " << sched.schedule.state << "."));
				redstoneSchedule[priority::repeat_on].erase(redstoneSchedule[priority::repeat_on].begin() + index);
				return ;
			}
			++index;
		}
    }

	/**
     * @brief loop through schedule, if repeater scheduled to turn off, extend its delay
     */
    void extendRepeaterDelay( int priority, int value, t_redstoneSchedule red )
    {
        for (auto &schedule : redstoneSchedule[(priority == priority::repeat_on) ? priority::repeat_off : priority::repeat_diode]) {
			if (schedule.chunk == red.chunk && schedule.schedule.pos == red.schedule.pos && schedule.schedule.state == redstone::off) {
				// std::cout << "extending delay of scheduled repeater to " << (((value >> offset::redstone::repeater::ticks) & 0x3) + 1) * redstone::tick << std::endl;
				schedule.schedule.ticks = (((value >> offset::redstone::repeater::ticks) & 0x3) + 1) * redstone::tick + (priority == priority::repeat_diode); // +1 repeat_diode at the end because we loop through schedule this very tick
			}
		}
    }

	/**
     * @brief loop through schedule, if chunk has redstone schedule, abort schedule (TODO save schedule to backup)
     */
    void deleteChunkSchedule( std::vector<std::shared_ptr<Chunk>>& deleted_chunks )
    {
        for (size_t schedIndex = 0; schedIndex < priority::size; ++schedIndex) {
            size_t limit = redstoneSchedule[schedIndex].size();
            if (!limit) {
                continue ;
            }

            size_t delCount = 0;
            for (size_t index = 0; index < limit; ++index) {
                Chunk* chunk = redstoneSchedule[schedIndex][index - delCount].chunk;

                auto search = std::find_if(deleted_chunks.begin(), deleted_chunks.end(), [chunk](auto& c) { return (c.get() == chunk); } );
                if (search != deleted_chunks.end()) {
                    redstoneSchedule[schedIndex].erase(redstoneSchedule[schedIndex].begin() + (index - delCount));
                    ++delCount;
                }
            }
        }
    }
};
