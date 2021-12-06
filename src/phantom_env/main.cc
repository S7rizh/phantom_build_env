/*
* Envrionment for Phantom OS
* Based on rm_nested example and rump block device backend
*/

#include <base/component.h>
#include <rm_session/connection.h>
#include <region_map/client.h>
#include <dataspace/client.h>

#include <base/allocator_avl.h>
#include <block_session/connection.h>

#include "phantom_env.h"
#include "disk_backend.h"
#include "phantom_vmem.h"
#include "phantom_threads.h"

#include "phantom_entrypoints.h"

void Phantom::test_obj_space(addr_t const addr_obj_space)
{

	// Reading from mem

	log("Reading from obj.space");

	addr_t read_addr = addr_obj_space;

	log("  read     mem                         ",
		(sizeof(void *) == 8) ? "                " : "",
		Hex_range<addr_t>(OBJECT_SPACE_START, OBJECT_SPACE_SIZE), " value=",
		Hex(*(unsigned *)(read_addr)));

	// Writing to mem

	log("Writing to obj.space");
	*((unsigned *)read_addr) = 256;

	log("    wrote    mem   ",
		Hex_range<addr_t>(OBJECT_SPACE_START, OBJECT_SPACE_SIZE), " with value=",
		Hex(*(unsigned *)read_addr));

	// Reading again

	log("Reading from obj.space");
	log("  read     mem                         ",
		(sizeof(void *) == 8) ? "                " : "",
		Hex_range<addr_t>(OBJECT_SPACE_START, OBJECT_SPACE_SIZE), " value=",
		Hex(*(unsigned *)(read_addr)));
}

// Block test function

void Phantom::test_block_device(Phantom::Disk_backend &disk)
{
	char buffer[512] = {0};
	char test_word[] = "Hello, World!";
	bool success = false;

	// Write then read

	memcpy(buffer, test_word, strlen(test_word));

	log("Writing to the disk");
	success = disk.submit(Disk_backend::Operation::WRITE, true, 1024, 512, buffer);
	log("Competed write (", success, ")");

	memset(buffer, 0x0, 512);

	log("Reading from the disk");
	success = disk.submit(Disk_backend::Operation::READ, false, 1024, 512, buffer);
	log("Competed read (", success, ")");

	log("Comparing results");
	if (strcmp(buffer, test_word) == 0)
	{
		log("Single write-read test was successfully passed!");
	}
	else
	{
		log("Single write-read test was failed!");
	}

	log("Done!");
}

void Component::construct(Genode::Env &env)
{
	log("--- Phantom env test ---");

	{
		/*
		 * Initialize object space region.
		 */
		Rm_connection rm(env);

		Region_map_client rm_obj_space(rm.create(OBJECT_SPACE_SIZE));
		Dataspace_client rm_obj_space_client(rm_obj_space.dataspace());

		Phantom::Local_fault_handler fault_handler(env, rm_obj_space);

		void *ptr_obj_space = env.rm().attach(rm_obj_space.dataspace(), 0, 0, true, OBJECT_SPACE_START, false, true);
		addr_t const addr_obj_space = reinterpret_cast<addr_t>(ptr_obj_space);
		log("Addr ", addr_obj_space);
		// log("Size ", rm_obj_space_client.size());
		// log(" region top        ",
		// 	Hex_range<addr_t>(addr_obj_space, rm_obj_space_client.size()));

		Phantom::test_obj_space(addr_obj_space);

		/*
		 * Setup Main object and disk backend
		 */
		try
		{

			static Phantom::Main main(env);
			Phantom::main_obj = &main;

			Phantom::test_block_device(main._disk);
		}
		catch (Genode::Service_denied)
		{
			error("opening block session was denied!");
		}

		/*
		* Starting Phantom
		*/

		int p_argc = 1;
		char **p_argv = nullptr;
		char **p_envp = nullptr;
		phantom_main_entry_point(p_argc, p_argv, p_envp);

		/*
		 * Tests
		 */
	}

	log("--- finished Phantom env test ---");
}
