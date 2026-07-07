# CoreShell

**CoreShell** is a custom, lightweight Linux shell built entirely in C. It is designed to demonstrate deep mastery of Operating System internals, including process management, inter-process communication (IPC), job control, and signal handling.

## 🚀 Features

- **Built-in Commands**: Support for standard internal commands like `cd`, `pwd`, `echo`, `clear`, and `exit`.
- **External Command Execution**: Seamlessly loads and executes standard external UNIX binaries (e.g., `ls`, `cat`, `grep`).
- **Advanced Job Control**:
  - Suspend running foreground tasks (`Ctrl + Z`).
  - Send tasks to the background (`bg`).
  - Bring background tasks to the foreground (`fg`).
  - View current suspended/background jobs (`jobs`).
- **Inter-Process Communication (IPC)**: Full support for chaining multiple commands using pipes (`|`), allowing standard output of one process to serve as standard input to the next.
- **Signal Handling**: Robust signal trapping for `SIGINT` (Ctrl+C), `SIGTSTP` (Ctrl+Z), and `SIGCHLD` to prevent the parent shell from crashing while managing child processes.
- **Environment Variables**: Access to shell variables like `$$` (shell PID), `$?` (last command exit status), and `$SHELL`.
- **Dynamic Prompts**: Users can dynamically change the shell prompt using `PS1=<new_prompt>`.

## 🧠 Architecture

CoreShell is structured around several modular components that work together to parse, dispatch, and manage commands.

### 1. Input Scanner & Parser (`scan_input.c`)
- Acts as the main loop of the shell (REPL).
- Reads user input, tokenizes the string, and determines if the command is `BUILTIN` or `EXTERNAL`.
- Dispatches commands to the appropriate execution engine and handles shell-specific commands like `jobs`, `fg`, and `bg`.

### 2. Execution Engine (`function.c`)
- **Internal Commands**: Executes built-in functions directly within the parent shell process.
- **External Commands**: Utilizes the `fork()` and `execvp()` system calls to spawn child processes for external binaries.
- **Pipe Routing**: Dynamically creates `pipe()` file descriptors and uses `dup2()` to stitch together the `stdout` and `stdin` of chained commands.

### 3. Job Manager
- Maintains a linked list (`Slist`) of all running or suspended jobs.
- Tracks `job_id`, `pid`, and process state.
- Automatically cleans up terminated child processes upon receiving `SIGCHLD` via `waitpid(..., WNOHANG)`.

### 4. Signal Dispatcher
- Overrides default terminal signals.
- Forwards `SIGINT` and `SIGTSTP` to the currently active foreground child process (`fg_pid`), ensuring the shell remains interactive and resilient.

## 🛠️ Build and Installation

### Prerequisites
- Linux / UNIX-like environment
- `gcc` compiler
- `make`

### Compilation
The project includes a `Makefile` for streamlined compilation.

```bash
# Build the project
make

# Build with debug flags (AddressSanitizer)
make debug
```

### Running CoreShell
```bash
# Start the shell
./coreshell
```

## 📖 Usage Examples

**Basic Commands**
```bash
MiniShell:$ pwd
/home/user/CoreShell
MiniShell:$ cd src
MiniShell:$ echo $$
12345
```

**Piping**
```bash
MiniShell:$ ls -l | grep ".c" | wc -l
```

**Job Control**
```bash
# Run a long command
MiniShell:$ sleep 100
^Z
[1]+ Stopped sleep 100

MiniShell:$ jobs
[1] ^Z Stopped sleep 100

MiniShell:$ bg
Job running in background

MiniShell:$ fg
Running the cmd : sleep 100
```

**Customizing Prompt**
```bash
MiniShell:$ PS1=MyShell>
MyShell>
```

## 👨‍💻 Author
**Amitesh Singh**  
*Project created: April 2026*
