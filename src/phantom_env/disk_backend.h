#ifndef PHANTOM_ENV_DISK_BACKEND_H
#define PHANTOM_ENV_DISK_BACKEND_H

#include "phantom_env.h"

#include <base/stdint.h>
#include <base/component.h>
#include <base/heap.h>
#include <block_session/connection.h>
#include <base/allocator.h>
#include <base/allocator_avl.h>
#include <base/log.h>

namespace Phantom
{
    class Disk_backend;
};

/**
 * Block session connection
 * 
 * Imported from repos/dde_rump/src/lib/rump/io.cc
 * 
 */

class Phantom::Disk_backend
{
protected:
    Genode::Env &_env;
    Genode::Heap &_heap;

    Genode::Allocator_avl _block_alloc{&_heap};
    Block::Connection<> _session{_env, &_block_alloc};
    Block::Session::Info _info{_session.info()};
    Genode::Mutex _session_mutex{};

    void _sync()
    {
        using Block::Session;

        Session::Tag const tag{0};
        _session.tx()->submit_packet(Session::sync_all_packet_descriptor(_info, tag));
        _session.tx()->get_acked_packet();
    }

public:
    enum class Operation
    {
        READ,
        WRITE,
        SYNC
    };

    Disk_backend(Genode::Env &env, Genode::Heap &heap) : _env(env), _heap(heap)
    {
        Genode::log("block device with block size ", _info.block_size, " block count ",
                    _info.block_count, " writeable=", _info.writeable);
        Genode::log("");
    }

    Genode::uint64_t block_count() const { return _info.block_count; }
    Genode::size_t block_size() const { return _info.block_size; }
    bool writable() const { return _info.writeable; }

    void sync()
    {
        Genode::Mutex::Guard guard(_session_mutex);
        _sync();
    }

    /*
    * Offset and length should be block aligned. Otherwise they may be cropped.
    *
    * XXX : 0 offset and 0 length may lead to qemu AHCI error
    * `qemu-system-x86_64: ahci: PRDT length for NCQ command (0x1) is smaller than the requested size (0x2000000)`
    */
    bool submit(Operation op, bool sync_req, Genode::int64_t offset, Genode::size_t length, void *data)
    {
        using namespace Block;

        Genode::Mutex::Guard guard(_session_mutex);

        Block::Packet_descriptor::Opcode opcode;

        switch (op)
        {
        case Operation::READ:
            opcode = Block::Packet_descriptor::READ;
            break;
        case Operation::WRITE:
            opcode = Block::Packet_descriptor::WRITE;
            break;
        default:
            return false;
            break;
        }

        /* allocate packet */
        try
        {
            Block::Packet_descriptor packet(_session.alloc_packet(length),
                                            opcode, offset / _info.block_size,
                                            length / _info.block_size);

            Genode::log("Block: cnt=", packet.block_count());
            Genode::log("       num=", packet.block_number());
            Genode::log("       off=", packet.offset());
            // It is ok for the offset not to be equal to the defined one

            /* out packet -> copy data */
            if (opcode == Block::Packet_descriptor::WRITE)
                Genode::memcpy(_session.tx()->packet_content(packet), data, length);

            _session.tx()->submit_packet(packet);
        }
        catch (Block::Session::Tx::Source::Packet_alloc_failed)
        {
            Genode::error("I/O back end: Packet allocation failed!");
            return false;
        }

        Block::Packet_descriptor packet = _session.tx()->get_acked_packet();

        /* in packet */
        if (opcode == Block::Packet_descriptor::READ)
            Genode::memcpy(data, _session.tx()->packet_content(packet), length);

        bool succeeded = packet.succeeded();

        _session.tx()->release_packet(packet);

        /* sync request */
        if (sync_req)
        {
            _sync();
        }

        return succeeded;
    }
};

#endif