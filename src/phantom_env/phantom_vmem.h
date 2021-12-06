#ifndef GENODE_PHANTOM_VMEM_H
#define GENODE_PHANTOM_VMEM_H

#include "phantom_env.h"
#include <rm_session/connection.h>
#include <region_map/client.h>
#include <dataspace/client.h>

using namespace Genode;

enum
{
    OBJECT_SPACE_SIZE = 0x80000000,
    OBJECT_SPACE_START = 0x80000000,
    PAGE_SIZE = 4096,
};

/**
 * Region-manager fault handler resolves faults by attaching new dataspaces
 */
class Phantom::Local_fault_handler : public Genode::Entrypoint
{
private:
    Env &_env;
    Region_map &_region_map;
    Signal_handler<Local_fault_handler> _handler;
    volatile unsigned _fault_cnt{0};

    void _handle_fault()
    {
        Region_map::State state = _region_map.state();

        _fault_cnt++;

        log("region-map state is ",
            state.type == Region_map::State::READ_FAULT ? "READ_FAULT" : state.type == Region_map::State::WRITE_FAULT ? "WRITE_FAULT"
                                                                     : state.type == Region_map::State::EXEC_FAULT    ? "EXEC_FAULT"
                                                                                                                      : "READY",
            ", pf_addr=", Hex(state.addr, Hex::PREFIX));

        log("allocate dataspace and attach it to sub region map");
        Dataspace_capability ds = _env.ram().alloc(PAGE_SIZE);
        _region_map.attach_at(ds, state.addr & ~(PAGE_SIZE - 1));

        log("returning from handle_fault");
    }

public:
    Local_fault_handler(Genode::Env &env, Region_map &region_map)
        : Entrypoint(env, sizeof(addr_t) * 2048, "local_fault_handler",
                     Affinity::Location()),
          _env(env),
          _region_map(region_map),
          _handler(*this, *this, &Local_fault_handler::_handle_fault)
    {
        region_map.fault_handler(_handler);

        log("fault handler: waiting for fault signal");
    }

    void dissolve() { Entrypoint::dissolve(_handler); }

    unsigned fault_count()
    {
        asm volatile("" ::
                         : "memory");
        return _fault_cnt;
    }
};

#endif