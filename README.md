# OS Emulator - Process Scheduler  
_MO1 Project — README / User Guide_

---

## Overview

This program simulates a basic operating system process scheduler with support for multiple scheduling algorithms and per-process instruction execution. It features a CLI menu, per-process screens, and logs for each process.

---

## How to Build and Run

1. **Compile:**
   ```sh
   g++ -std=c++11 -I"Header Files" -o emulator main.cpp config.cpp screen.cpp util.cpp core_manager.cpp process.cpp

2. **Run:**
   ```sh
   emulator

3. **Config:**
    - Make sure config.txt is present in the project directory.
    - Edit as needed to set CPU count, scheduler type, instruction lengths, etc.
  
## Main Menu Commands

| Command              | Description                                              |
|----------------------|---------------------------------------------------------|
| `initialize`         | Loads config.txt and prepares the scheduler             |
| `scheduler-start`    | Starts the scheduler and begins process execution       |
| `scheduler-stop`     | Stops the scheduler (can be started again)              |
| `screen -ls`         | Lists all running and finished processes and core usage |
| `screen -s <proc>`   | Attach to a running process screen (interactive mode)   |
| `screen -r <proc>`   | Re-attach to a running process screen                   |
| `report-util`        | Prints and saves CPU utilization report                 |
| `clear`              | Clears the console and prints the program header        |
| `exit`               | Stops scheduler (if running) and exits the program      |

## Process Screen Commands (`screen -s` or `screen -r`)

| Command       | Description                                                       |
| ------------- | ----------------------------------------------------------------- |
| `process-smi` | Shows process info (variables, current instruction, logs, status) |
| `exit`        | Exits process screen and returns to main menu                     |

While attached to a process:
- Use `process-smi` as many times as you want to see live progress, logs, and variables.
- When a process finishes, “Finished!” is shown and the screen closes automatically.

## Instruction Types (Barebones)
Processes randomly generate and execute these instructions:

| Instruction | Description                                                   |
| ----------- | ------------------------------------------------------------- |
| PRINT       | Logs a message                                                |
| DECLARE     | Creates or updates a variable                                 |
| ADD         | Adds two variables/constants, saves result to a variable      |
| SUBTRACT    | Subtracts two variables/constants, saves result to a variable |
| SLEEP       | Pauses the process for N ticks                                |
| FOR         | Repeats a block of instructions N times                       |

*All variables are unsigned 16-bit (0–65535), and values are clamped accordingly.*

## Config File (`config.txt`)
Edit `config.txt` to control:

| Setting       | Description                       |
| ------------- | --------------------------------- |
| numCPU        | Number of CPU cores               |
| schedulerType | Scheduling algorithm (FCFS or RR) |
| quantumCycles | Quantum value (for RR)            |
| batchProcFreq | Batch process creation frequency  |
| minIns        | Minimum instructions per process  |
| maxIns        | Maximum instructions per process  |
| delayPerExec  | Delay per instruction (in ms)     |

Example:
```
numCPU=2
schedulerType=fcfs
quantumCycles=1
batchProcFreq=5
minIns=10
maxIns=50
delayPerExec=100
```

## Tips and Edge Cases
- Run `initialize` before any scheduler or screen commands.
- Once a process finishes, it cannot be re-attached.
- Logs and variables are per-process and shown only in process screens.
- Exiting the program while the scheduler is running will stop everything cleanly.
- Both SLEEP and FOR instructions are supported.

## Troubleshooting
- **Can’t attach to a process?**
  <br> Make sure the name is correct and the process hasn’t finished.

- **No logs?**
  <br> The process may not have generated any PRINT instructions.

- **Crash/bug?**
  <br> Check your config file and restart the program.

## Credits
Developed by Ang, Capio, Robles, Sarreal <br>
MO1 OS Emulator - Process Scheduler Project


