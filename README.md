# 🐚 MiniShell

> A lightweight, POSIX-inspired Unix shell built from scratch in C — featuring process management, job control, signal handling, and dynamic pipe execution.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Built-in Commands](#built-in-commands)
- [Signal Handling](#signal-handling)
- [Job Control](#job-control)
- [Pipe Support](#pipe-support)
- [Project Structure](#project-structure)
- [System Calls Used](#system-calls-used)
- [Sample Session](#sample-session)
- [Author](#author)

---

## Overview

MiniShell is an interactive command-line shell that mimics the core behavior of Unix shells like `bash` and `sh`. It was built entirely using C system calls — without relying on `system()` — to understand how processes are created, synchronized, and managed at the OS level.

The shell classifies every user input as one of three types:
- **Built-in command** — handled directly by the shell process
- **External command** — executed in a forked child process via `execvp`
- **Unknown command** — reports an error and re-displays the prompt

---

## Features

| Feature | Description |
|---|---|
| Custom prompt | Default `myshell>`, configurable via `PS1=` |
| External command execution | Fork + exec model with `waitpid` synchronization |
| Multi-level pipes | `ls -l \| grep .c \| wc -l` — dynamic pipe chaining |
| Background execution | Run commands with `&` to detach from foreground |
| Job control | `jobs`, `fg`, `bg` for full background process management |
| Signal handling | `Ctrl+C`, `Ctrl+Z`, and SIGCHLD properly handled |
| Exit status | `echo $?` reports last command's exit code |
| Shell info | `echo $SHELL` and `echo $$` supported |

---

## Getting Started

### Prerequisites

- GCC compiler
- Linux / Unix environment

### Build

```bash
gcc main.c commands.c scan_input.c list.c -o minishell
```

Or using the Makefile (if included):

```bash
make
```

### Run

```bash
./minishell
```

---

## Usage

```
-----------------------------------------
       Welcome to MINISHELL
-----------------------------------------
myshell> pwd
/home/user/minishell
myshell> ls -l | wc -l
8
myshell> sleep 30 &
[bg] PID 4321 : sleep 30
myshell> jobs
[1] PID: 4321 Command: sleep 30
myshell>
```

### Customizing the prompt

```bash
myshell> PS1=madhavi$
madhavi$ pwd
/home/madhavi
```

> Note: Spaces around `=` are not allowed. `PS1 = newprompt` will be treated as an unknown command.

---

## Built-in Commands

| Command | Description |
|---|---|
| `pwd` | Print current working directory |
| `cd <path>` | Change directory (defaults to `$HOME` if no path given) |
| `exit` | Terminate the shell |
| `echo $?` | Print exit status of the last command |
| `echo $SHELL` | Print the shell executable path |
| `echo $$` | Print the PID of the shell process |
| `jobs` | List all background jobs |
| `fg` | Bring the most recent background job to the foreground |
| `bg` | Resume the most recent stopped job in the background |

---

## Signal Handling

| Signal | Shortcut | Behavior |
|---|---|---|
| `SIGINT` | `Ctrl+C` | Kills the foreground child process; re-displays prompt if no foreground process |
| `SIGTSTP` | `Ctrl+Z` | Stops the foreground child and moves it to the background job list |
| `SIGCHLD` | *(automatic)* | Reaps finished background processes and prints their exit status |

---

## Job Control

MiniShell maintains a **linked list** of background jobs. Each node stores the PID and command string of the background process.

```bash
myshell> sleep 60 &
[bg] PID 5001 : sleep 60

myshell> jobs
[1] PID: 5001 Command: sleep 60

myshell> fg
# brings sleep 60 to foreground

myshell> bg
# resumes a stopped process in the background
```

When a background process finishes, SIGCHLD is caught automatically and the exit status is printed:

```
[bg done] PID 5001 exited with status 0
```

---

## Pipe Support

MiniShell supports chaining multiple commands using `|`. Pipes are created dynamically using `pipe()` and `dup2()` — one child process per command stage.

```bash
myshell> ls -l | grep .c | wc -l
4

myshell> cat /etc/passwd | grep root | cut -d: -f1
root
```

The parent manages file descriptor cleanup and waits for all children to complete.

---

## Project Structure

```
minishell/
├── main.c          # Entry point, global variable definitions
├── scan_input.c    # Main REPL loop, signal setup, input parsing
├── commands.c      # Command classification and execution (built-in + external + pipes)
├── list.c          # Linked list for background job tracking
├── header.h        # Shared macros, structs, extern declarations
└── ext_cmds.txt    # List of recognized external commands
```

---

## System Calls Used

| Category | System Calls / APIs |
|---|---|
| Process creation | `fork()`, `execvp()` |
| Process synchronization | `waitpid()`, `WUNTRACED`, `WNOHANG`, `WIFEXITED`, `WEXITSTATUS` |
| Inter-process communication | `pipe()`, `dup2()` |
| Signal handling | `signal()`, `kill()` |
| Directory operations | `chdir()`, `getcwd()` |
| Environment | `getenv()` |
| I/O | `fgets()`, `printf()`, `perror()` |
| Memory | `malloc()`, `free()` |

---

## Sample Session

```
-----------------------------------------
       Welcome to MINISHELL
-----------------------------------------
myshell> pwd
/home/madhavi/minishell

myshell> ls -l | wc
      8      74     472

myshell> sleep 10 &
[bg] PID 7842 : sleep 10

myshell> jobs
[1] PID: 7842 Command: sleep 10

myshell> echo $?
0

myshell> cd ..
myshell> pwd
/home/madhavi

myshell> exit
Exiting mini shell!!!!!
```

---

## Author

**Madhavi Shaji**  
Date: February 2026  
Systems Programming Project — MiniShell Implementation

---

> *"Building a shell forces you to confront problems that most application-level programming completely hides — how processes are created and destroyed, how signals interrupt execution, how file descriptors are shared across forks, and how a parent and child coordinate without race conditions."*
