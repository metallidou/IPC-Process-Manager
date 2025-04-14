# IPC-Process-Manager

## Clarifications:
For the development of the process management system using Inter-Process Communication (IPC), the `semaphore.h` library is used, specifically POSIX semaphores.  
 
The program is compiled using the `make` command, as the code includes a Makefile.  
The program is executed using the command `./main test_files/"CF".txt "M"` from the command line, where `main` is the executable file, `test_files/"CF".txt` is the config file used to create timestamps, processes, and their respective actions, and `M` is the total number of semaphores used for synchronizing the processes.  

The program structure is as follows:

### main.c

#### main:
First, the `main` function reads the command line arguments and, if valid, calls the `read_config` function. This function takes as arguments the config file, the text file (`mobydick.txt`), and two pointers to the `Config` and `Processes` structs, which are defined in `info.h`.  
Next, the `parent_process` function is called, where the parent process reads the commands from the config file and continuously assigns tasks to child processes via IPC.  
Finally, it frees the memory allocated for the structs and its variables.

### info.h
A header file that defines 4 structs:
1. **Processes:** Contains basic information about the processes from the config file, such as their number (how many processes per label, e.g., C2), the exit timestamp, and a pointer to the `struct ProcessInfo`.
2. **ProcessInfo:** Holds information about each individual process, such as the process label (name), the corresponding index (position in the `ProcessInfo*` array in `Processes`), its `pid`, the index of the semaphore assigned to it (in the array of semaphores), and an `active` flag indicating whether the process is active (1 for active, 0 for inactive).
3. **Config:** Contains basic info about the config file, such as the number of lines, the name of the text file used (for convenience), and a pointer to the `struct ConfigInfo`.
4. **ConfigInfo:** Holds info for each line of the config file (except the last, which contains the exit timestamp), including the timestamp, process name, command (S or T), and the index corresponding to the process in the `ProcessInfo` struct.

### file.c/file.h

#### count_file_lines:
Returns the total number of lines in a file.

#### read_config:
Initially calls `initialize_config` and `initialize_processes` to set up the members of the `Config` and `Processes` structs.  
Then it reads the contents of the config file and populates the respective struct variables accordingly.

#### initialize_config:
Initializes the members of the `Config` struct and allocates memory.

#### choose_random_file_line:
Selects a random line from the text file (recursively called if the selected line contains only spaces) and returns it.

### process.c/process.h

#### initialize_processes:
Initializes the member variables of the `Processes` struct and allocates memory.

#### is_process_defined:
Checks whether a process is defined within the `Processes` struct (by checking if the process name/label has been set).

#### get_process_index:
Returns the index corresponding to the process with the given name.


### semaphores.c/semaphores.h

#### create_semaphores_array:
Creates an array of semaphores with a size defined by the variable `M`.  
Each semaphore is given a name of the format `/sem_<i>`, where `i` is its index in the array.  
Before creating the semaphore with `sem_open`, any existing semaphore with the same name is unlinked using `sem_unlink`.  
The function returns the array of semaphores.

#### create_semaphore_print:
Creates a `sem_print` semaphore used to synchronize the print messages from child processes assigned by the parent process.

#### cleanup_semaphores_array:
Cleans up the array of semaphores.

#### cleanup_semaphore_print:
Cleans up the `sem_print` semaphore.

#### assign_semaphore_to_process:
This function selects the first available semaphore to assign to a child process.  
It creates an array named `sem_indexes` initialized with the indexes of the semaphores in the array, then filters out any semaphore that has already been assigned to an active child until it finds one that is available.


### shared_memory.c/shared_memory.h

#### create_shared_memory:
This function creates the shared memory used for communication between the parent and child processes.  
It uses `shm_open` to create the memory object, checks that the memory segment is allocated with the correct size, and uses `mmap` to map the shared memory object into the processes' address space.


### parent.c/parent.h

#### parent_process:
This function allows the parent process to create the semaphore array, the shared memory segment, and a print synchronization semaphore, then launches the child processes.  
  
In each execution cycle:
- A new process is created via the `S` (spawn) command, only if it is inactive and the number of active children is less than the number of available semaphores (each semaphore is associated with one active child process).
- A process is terminated via the `T` (terminate) command, making its semaphore available again for another child process.

During both the creation (`spawn_child`) and termination (`terminate_child`) of a child process:
- The relevant members of the `Processes` struct are updated,
- The `active_children` variable is updated to reflect the current number of active children, and
- The `i` index is updated to represent the current line of the config file being executed.

Additionally, in each execution cycle (tracked by `current_time`, and printed with synchronization via `sem_print`):
- The parent selects a random line from the text file and assigns it to a random active child to print.
- This is done by copying the line into shared memory and notifying the corresponding child process by posting its semaphore (`sem_post`), while the child waits using `sem_wait`.

After execution:
- The parent must terminate any remaining active children by calling `child_terminate`, which writes the message `"TERMINATE"` into the shared memory and signals the child to terminate.

Finally:
- All semaphores are cleaned up using `sem_close` and `sem_unlink`.
- Shared memory is unmapped and the memory object is closed and removed.


#### choose_random_active_child:
Returns the index of a randomly selected active child.  
It does this by collecting the indexes of all currently active children into an `active_indices` array and selecting one at random.

### child.c/child.h

#### spawn_child:
Creates a new child process using `fork`.  
If the process is successfully created, it calls the `child_process` function, where the tasks assigned by the parent process are executed.

#### terminate_child:
To terminate a child process:
- The message `"TERMINATE"` is copied into shared memory,
- The corresponding semaphore is posted (`sem_post`) to notify the child, and
- The parent waits for the child to finish its tasks using `waitpid`.

#### child_process:
This function contains the main logic for the child process.  
The child enters a loop where it continuously waits (`sem_wait`) for a message from the parent (either a termination command or a print task), received via shared memory.

- If it receives a **spawn** message:
  - It records the **start timestamp** and enters a `while` loop.

- If it receives a **print** message:
  - It prints the message retrieved from shared memory.
  - It then posts the `sem_print` semaphore to allow the parent to print the next timestamp.

- If it receives a **terminate** message:
  - It records the **end timestamp** and exits the `while` loop.
  - Then it prints the total number of active steps (iterations) and the number of print requests it received from the parent (tracked via a `counter`).
  - Finally, it calls `exit`.



