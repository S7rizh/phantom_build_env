// #define DEBUG_MSG_PREFIX "vm.unixhal"
// #include <debug_ext.h>
// #define debug_level_flow 10
// #define debug_level_error 10
// #define debug_level_info 10

// #include <stdarg.h>
// #include <pthread.h>
// #include "genode_misc.h"

#include "phantom_env.h"
#include "phantom_threads.h"

// Important to keep this typedef before threads.h
typedef int errno_t;

#include <threads.h>

using namespace Phantom;

extern "C"
{

    /*
*
* Thread creation/destruction
* returns TID
*
*/

    int hal_start_kernel_thread_arg(void (*thread)(void *arg), void *arg)
    {
        // Actually, no distinction between regular and kernel thread since in both cases we will be in userspace
        return main_obj->_threads_repo.createThread(thread, arg);
    }

    void hal_start_kernel_thread(void (*thread)(void))
    {
        main_obj->_threads_repo.createThread(thread);
    }

    void hal_exit_kernel_thread()
    {
        // XXX : Seems that the legit way is to return from Thread::entry
        main_obj->_env.cpu().kill_thread(Thread::myself()->cap());
    }

    tid_t hal_start_thread(void (*thread)(void *arg), void *arg, int flags)
    {
        main_obj->_threads_repo.createThread(thread, arg, flags);
        return 0;
    }

    errno_t t_kill_thread(tid_t tid)
    {
        main_obj->_threads_repo.killThread(tid);
        return 0;
    }

    errno_t t_current_set_death_handler(void (*handler)(phantom_thread_t *tp))
    {

        PhantomGenericThread *ph_thread = (PhantomGenericThread *)Thread::myself();
        ph_thread->setDeathHandler(handler);

        return 0;
    }

    /*
*
* Thread info
*
*/

    phantom_thread_t *get_current_thread()
    {
        // XXX: Probably, better to avoid this allocation. It will inevitably lead to dangling pointers
        PhantomGenericThread *ph_thread = (PhantomGenericThread *)Thread::myself();
        phantom_thread_t *alloced_thread_struct = new (main_obj->_heap) phantom_thread_t(ph_thread->getPhantomStruct());
        return alloced_thread_struct;
    }

    tid_t get_current_tid(void)
    {
        return ((PhantomGenericThread *)Thread::myself())->_info.tid;
    }

    void *get_thread_owner(phantom_thread_t *t)
    {
        return t->owner;
    }

    errno_t t_get_owner(int tid, void **owner)
    {
        *owner = main_obj->_threads_repo.getThreadByTID(tid)->_info.owner;
        return 0;
    }

    errno_t t_current_get_priority(int *prio)
    {
        *prio = ((PhantomGenericThread *)Thread::myself())->_info.prio;
        return 0;
    }

    // errno_t t_get_ctty(tid_t tid, struct wtty **ct) { return ENOENT; }

    /*
*
* Thread configuration
*
*/

    void hal_set_current_thread_name(const char *name)
    {
        // XXX : Not used, actually
        //       Moreover, Genode doesn't allow to set name
        (void)name;
    }

    errno_t hal_set_current_thread_priority(int p)
    {
        return t_current_set_priority(p);
    }

    errno_t t_current_set_name(const char *name)
    {
        // XXX : Not used, actually
        //       Moreover, Genode doesn't allow to set name
        (void)name;
        return 0;
    }
    errno_t t_current_set_priority(int p)
    {
        ((PhantomGenericThread *)Thread::myself())->_info.prio = p;
        return 0;
    }
    errno_t t_set_owner(tid_t tid, void *owner)
    {
        ((PhantomGenericThread *)Thread::myself())->_info.owner = owner;
        return 0;
    }

    /*
*
* Multithreading
*
*/

    void t_smp_enable(int yn)
    {
        // _stub_print();
    }

    //< Enable or disable access to paged memory - calls arch pagemap func.
    void t_set_paged_mem(bool enable)
    {
        // _stub_print();
    }

    // TODO : Rework sleeping threads to use blockades or timers!
    void wake_sleeping_thread(void *arg)
    {
        // arg is tid
        // _stub_print();
    }

    /**
 *
 * Adds flag to sleep flags of thread, removes thread from
 * runnable set, unlocks the lock given, switches off to some other thread.
 */
    void thread_block(int sleep_flag, hal_spinlock_t *lock_to_be_unlocked)
    {
        // _stub_print();
    }

    /*
*
* Snapshot machinery
*
*/

    void t_migrate_to_boot_CPU(void)
    {
        // _stub_print();
    }

    // mark myself as snapper thread
    errno_t t_set_snapper_flag(void)
    {
        // _stub_print();
        return 0;
    }

    // TODO : Remove if not needed

    // void phantom_thread_wait_4_snap()
    // {
    //     // Just return
    // }

    // void phantom_snapper_wait_4_threads()
    // {
    //     // Do nothing in non-kernel version
    //     // Must be implemented if no-kernel multithread will be done
    // }

    // void phantom_snapper_reenable_threads()
    // {
    //     //
    // }

    // void phantom_activate_thread()
    // {
    //     // Threads do not work in this mode
    // }
}