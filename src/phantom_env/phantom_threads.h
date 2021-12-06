#ifndef GENODE_PHANTOM_THREADS_H
#define GENODE_PHANTOM_THREADS_H

#include "phantom_env.h"

#include <base/thread.h>
#include <base/log.h>

namespace Phantom
{
    class PhantomThreadsRepo;

    struct PhantomGenericThread;
    struct PhantomThread;
    struct PhantomThreadWithArgs;
};

using namespace Phantom;

// XXX : Required, but probably should be reworked
extern "C"
{

    struct phantom_thread
    {
        int tid = 0;
        void *owner = 0;
        int prio = 0;
    };
}

// Struct containing generic fields for all threads
struct Phantom::PhantomGenericThread : public Genode::Thread
{
    // Make sure not to change this field outside this class!!!
    phantom_thread _info = {0};

    // Required by VM threads
    // XXX : probably should be reworked
    void (*_death_handler)(phantom_thread *tp) = nullptr;

    void setDeathHandler(void (*death_handler)(phantom_thread *tp))
    {
        _death_handler = death_handler;
    }

    const phantom_thread getPhantomStruct()
    {
        return _info;
    }

    PhantomGenericThread(Genode::Env &env, const char *name, Genode::size_t stack_size) : Genode::Thread(env, name, stack_size)
    {
    }

    ~PhantomGenericThread() override
    {
        if (_death_handler != nullptr)
        {

            phantom_thread local_thread = getPhantomStruct();
            _death_handler(&local_thread);
        }
    }
};

struct Phantom::PhantomThread : PhantomGenericThread
{
    // TODO: Probably, add tid, owner e.t.c here as fields

    void (*_thread_entry)(void);

    void entry() override
    {
        Genode::log("Entered a new thread!");
        _thread_entry();
    }

    PhantomThread(Genode::Env &env, void (*thread_entry)(void)) : PhantomGenericThread(env, "Phantom", 8192), _thread_entry(thread_entry)
    {
    }
};

struct Phantom::PhantomThreadWithArgs : PhantomGenericThread
{
    void (*_thread_entry)(void *arg);
    void *_args;

    void entry() override
    {
        _thread_entry(_args);
    }

    PhantomThreadWithArgs(Genode::Env &env, void (*thread_entry)(void *arg), void *args) : PhantomGenericThread(env, "Phantom", 8192),
                                                                                           _thread_entry(thread_entry),
                                                                                           _args(args)

    {
    }

    PhantomThreadWithArgs(const PhantomThreadWithArgs &another) = delete;
    int operator=(const PhantomThreadWithArgs &another) = delete;
};

class Phantom::PhantomThreadsRepo
{
    Genode::Env &_env;
    Genode::Heap &_heap;
    PhantomGenericThread *_threads[512] = {0};
    unsigned int _thread_count = 0;

public:
    PhantomThreadsRepo(Genode::Env &env, Genode::Heap &heap) : _env(env), _heap(heap)
    {
    }

    int addThread(PhantomGenericThread *thread)
    {
        if (_thread_count == 512)
        {
            Genode::error("Threads number limit is reached! Can't create more");
            return -1;
        }

        _threads[_thread_count] = thread;

        // XXX : Very scetchy tid, need something better
        return _thread_count++;
    }

    int createThread(void (*thread_entry)(void))
    {
        return addThread(new (_heap) PhantomThread(_env, thread_entry));
    }

    int createThread(void (*thread_entry)(void *arg), void *args)
    {
        return addThread(new (_heap) PhantomThreadWithArgs(_env, thread_entry, args));
    }

    int createThread(void (*thread_entry)(void *arg), void *args, int flags)
    {
        (&flags); // XXX: Seems that they are not used
        return addThread(new (_heap) PhantomThreadWithArgs(_env, thread_entry, args));
    }

    PhantomGenericThread *getThreadByTID(int tid)
    {
        return _threads[tid];
    }

    void killThread(int tid)
    {
        // For now tid is an index
        _env.cpu().kill_thread(_threads[tid]->cap());
    }
};

#endif