# IPC-Process-Manager

A UNIX-based process manager in C that uses **POSIX semaphores** and **shared memory** for synchronized communication between a parent process and dynamically spawned children.

It simulates a process management system where a parent process creates and manages child processes, which perform read operations.
The program is designed to control the spawning, execution and termination of processes using inter-process communication (IPC) mechanisms, such as semaphores and shared memory.

## 🔧 Build
To compile the project, run the following command:
```
make main
```

## 🚀 Run
To execute the program, use the following command:
```
./main <CF> <TF> <M>
```

Where:
- `<CF>` is the configuration file that declares the process actions and their execution timestamps.
- `<TF>` is the text file which contains text that child processes read from.
- `<M>` is the number of semaphores used for synchronizing the processes (limits the number of concurrently active child processes).


## 📝 Example Usage

Here’s an example of a config file (CF):

```
0 S C1
10 T C1
20 S C2
...
```
Where:
- S indicates the spawn command, instructing the parent to spawn a new child process.
- T indicates the terminate command, instructing the parent to terminate a child process.
- The number represents the timestamp at which the action should occur.

## 🧠 How it Works

The program uses **Inter-Process Communication (IPC)** to manage the synchronization and execution of parent and child processes.

- **Parent Process:** 
  - Reads the configuration file and interprets the spawn (`S`) and terminate (`T`) commands at the specified timestamps.
  - Uses **POSIX semaphores** to control the number of concurrently active child processes, ensuring proper synchronization and execution.
  - Sends a random line of text from a file to an available child for reading (if no child process available nothing happens).

- **Child Processes:**
  - Each child waits for instructions from the parent. 
  - On receiving a spawn command (`S`), it starts and reads a random line from a text file.
  - On receiving a terminate command (`T`), it terminates and reports back to the parent.

- **Shared Memory:** 
  - Shared memory is used to transfer messages between the parent and child processes. This allows for dynamic communication and control over the process execution.

- **Synchronization via Semaphores:**
  - Semaphores control the timing and sequencing of tasks, ensuring that no more than `M` child processes are active at a time.




