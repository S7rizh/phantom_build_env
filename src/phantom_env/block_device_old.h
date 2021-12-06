// /*
//  * Block device interface implemented using update_jobs instead of tx_channel
//  * XXX : Not complete and untested
// */

// #include <base/component.h>
// #include <rm_session/connection.h>
// #include <region_map/client.h>
// #include <dataspace/client.h>

// #include <base/allocator.h>
// #include <base/allocator_avl.h>
// #include <block_session/connection.h>

// typedef Block::Connection<Job> Block_connection;

// struct Job : Block_connection::Job
// {
//     unsigned const id;

//     Job(Block_connection &connection, Block::Operation operation, unsigned id)
//         : Block_connection::Job(connection, operation), id(id)
//     {
//     }
// };

// class Scratch_buffer
// {
// private:
//     Allocator &_alloc;

//     Scratch_buffer(Scratch_buffer const &);
//     Scratch_buffer &operator=(Scratch_buffer const &);

// public:
//     char *const base;
//     size_t const size;

//     Scratch_buffer(Allocator &alloc, size_t size)
//         : _alloc(alloc), base((char *)alloc.alloc(size)), size(size) {}

//     ~Scratch_buffer() { destroy(&_alloc, base); }
// };

// class BlockDevice
// {
//     // From block_tester

//     Env &_env;
//     Allocator &_alloc;
//     size_t const _io_buffer = Number_of_bytes(4 * 1024 * 1024);

//     Allocator_avl _block_alloc{&_alloc};
//     Constructible<Block_connection> _block{};

//     // Required to store read results and write input
//     Scratch_buffer &_scratch_buffer;

//     // Configuration
//     Block::Session::Info _info{};
//     bool const _copy;
//     size_t const _batch;
//     bool const _verbose;

//     // Statistics
//     uint64_t _start_time{0};
//     uint64_t _end_time{0};
//     size_t _bytes{0};
//     uint64_t _rx{0};
//     uint64_t _tx{0};
//     size_t _triggered{0}; /* number of I/O signals */
//     unsigned _job_cnt{0};
//     unsigned _completed{0};
//     bool _stop_on_error{true};
//     bool _finished{false};
//     bool _success{false};

//     void _handle_block_io()
//     {
//         _block->update_jobs(*this);
//     }

//     Signal_handler<BlockDevice> _block_io_sigh{
//         _env.ep(), *this, &BlockDevice::_handle_block_io};

//     BlockDevice(Env &env, Allocator &alloc) : _env(env), _alloc(alloc)
//     {
//         _block.construct(_env, &_block_alloc, _io_buffer);
//         _block->sigh(_block_io_sigh);
//         _info = _block->info();

//         log("Block device info:");
//         log("Block size:       %d", _info.block_size);
//         log("Block count:      %d", _info.block_count);
//         log("packet alignment: %d", _info.align_log2);
//         log("Writeable:        %d", _info.writeable);
//     }

//     void _memcpy(char *dst, char const *src, size_t length)
//     {
//         if (length > _scratch_buffer.size)
//         {
//             warning("scratch buffer too small for copying");
//             return;
//         }

//         Genode::memcpy(dst, src, length);
//     }

//     /**
// 	 * Block::Connection::Update_jobs_policy
// 	 */
//     void produce_write_content(Job &job, off_t offset, char *dst, size_t length)
//     {
//         _tx += length / _info.block_size;
//         _bytes += length;

//         if (_verbose)
//             log("job ", job.id, ": writing ", length, " bytes at ", offset);

//         if (_copy)
//             _memcpy(dst, _scratch_buffer.base, length);
//     }

//     /**
// 	 * Block::Connection::Update_jobs_policy
// 	 */
//     void consume_read_result(Job &job, off_t offset,
//                              char const *src, size_t length)
//     {
//         _rx += length / _info.block_size;
//         _bytes += length;

//         if (_verbose)
//             log("job ", job.id, ": got ", length, " bytes at ", offset);

//         if (_copy)
//             _memcpy(_scratch_buffer.base, src, length);
//     }

//     /**
// 	 * Block_connection::Update_jobs_policy
// 	 */
//     void completed(Job &job, bool success)
//     {
//         _completed++;

//         if (_verbose)
//             log("job ", job.id, ": ", job.operation(), ", completed");

//         if (!success)
//             error("processing ", job.operation(), " failed");

//         destroy(_alloc, &job);

//         if (!success && _stop_on_error)
//             log("failed job");
//     }

// public:
//     void test()
//     {

//         Block::Operation const operation_write{
//             .type = Block::Operation::Type::WRITE,
//             .block_number = 1024,
//             .count = 1024};

//         Block::Operation const operation_sync{
//             .type = Block::Operation::Type::SYNC,
//             .block_number = 1024,
//             .count = 1024};

//         Block::Operation const operation_read{
//             .type = Block::Operation::Type::READ,
//             .block_number = 1024,
//             .count = 1024};

//         _job_cnt++;
//         new (&_alloc) Job(*_block, operation, _job_cnt);

//         // log()
//     }
// };