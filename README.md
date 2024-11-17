# COMP310/ECSE427 Fall 2024 Assignment Repository

Yongru Pan, 261001758
Yixuan Qin, 261010963

[A1 README](https://github.com/WYRP/ECSE-427-Assignments/blob/main/a1/A1_README.md) <br>
[A2 README](https://github.com/WYRP/ECSE-427-Assignments/blob/main/a2/A2_README.md) <br>
[A3 README](https://github.com/WYRP/ECSE-427-Assignments/blob/main/a3/A3_README.md) <br>



This repository contains assignments completed for the ECSE 427: Operating Systems course at McGill University. Each assignment focuses on different aspects of operating systems, providing practical experience in system-level programming.

## Assignments Overview

1. **Assignment 1: Simple Shell Implementation**
   - **Objective:** Develop a basic command-line shell capable of executing user commands.
   - **Key Features:**
     - Command parsing and execution
     - Handling built-in commands
     - Implementing input/output redirection
     - Managing background processes

2. **Assignment 2: Shared Memory and Synchronization**
   - **Objective:** Implement inter-process communication using shared memory and synchronization mechanisms.
   - **Key Features:**
     - Creating and managing shared memory segments
     - Implementing producer-consumer problem
     - Utilizing semaphores for synchronization
     - Ensuring data consistency between processes

3. **Assignment 3: Simple File System (SSFS)**
   - **Objective:** Design and implement a simple file system to understand file management in operating systems.
   - **Key Features:**
     - Creating a virtual disk
     - Implementing file creation, deletion, and reading
     - Managing directories and file metadata
     - Ensuring data integrity and handling errors

## Getting Started

To explore the assignments:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/WYRP/ECSE-427-Assignments.git
   ```
2. **Navigate to the Desired Assignment:**
   ```bash
   cd ECSE-427-Assignments/AssignmentX
   ```
   Replace `AssignmentX` with the specific assignment folder (e.g., `Assignment1`).

3. **Compile the Code:**
   Each assignment includes a `Makefile` for compilation. Run:
   ```bash
   make
   ```
   This will compile the source code and generate the executable.

4. **Run the Program:**
   Execute the compiled program:
   ```bash
   ./program_name
   ```
   Replace `program_name` with the actual name of the executable.

## Prerequisites

- **Operating System:** Linux-based system (e.g., Ubuntu)
- **Compiler:** GCC
- **Tools:** Make, Git

Ensure these tools are installed and properly configured on your system.

## Usage

Each assignment directory contains a `README.md` file with detailed instructions on usage, including:

- Command-line arguments
- Expected input and output
- Examples of typical usage scenarios

Refer to these files for specific guidance on running and testing each assignment.

## Contributing

Contributions to this repository are welcome. To contribute:

1. **Fork the Repository**
2. **Create a New Branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Commit Your Changes:**
   ```bash
   git commit -m "Add your descriptive commit message"
   ```
4. **Push to the Branch:**
   ```bash
   git push origin feature/your-feature-name
   ```
5. **Open a Pull Request**

Please ensure your code follows the project's coding standards and includes appropriate documentation.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- McGill University
- ECSE 427 Course Instructors and Teaching Assistants
- Open-source community for tools and resources

For any questions or further information, please contact the repository owner or refer to the course materials provided by McGill University.
