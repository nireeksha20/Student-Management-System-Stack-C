# Student Management System Using a Stack (C)

## 1. Objective
This menu-driven console application manages student records using a stack and demonstrates the Last-In-First-Out (LIFO) principle. A second stack stores deleted records so the latest deletion can be undone.

## 2. Requirements
- C compiler such as GCC (MinGW-w64 on Windows is suitable).
- The program uses a fixed-size array implementation, with a maximum of 50 active student records.
- No built-in stack or collection class is used.

## 3. Files
- `student_management.c` — complete source code.
- `sample_run.txt` — sample transcript demonstrating three adds, one remove, one undo, one search, and a full display.

## 4. Compile and run

### Windows (GCC / MinGW)
Open a terminal in this folder and run:

```bash
gcc student_management.c -o student_management
student_management.exe
```

### Linux / macOS
```bash
gcc -std=c11 -Wall -Wextra -pedantic student_management.c -o student_management
./student_management
```

## 5. Stack implementation
The program defines a `Stack` structure containing an array of 50 `Student` records and an integer `top`. `top` is `-1` when the stack is empty. The program implements these operations itself:
- `push()` — adds a student to the top.
- `pop()` — removes and returns the top student.
- `peek()` — reads the top student without removing it.
- `isEmpty()` — checks whether the stack is empty.
- `isFull()` — checks whether the fixed-size stack is full.

The main stack stores active students. The history stack stores records removed by the remove operation. The undo operation pops the most recently deleted record from history and pushes it back onto the main stack.

Display and search temporarily pop records into an auxiliary stack and then push them back, preserving the main stack's original order. Stack internals use array indexing only inside the basic stack operations; features use stack operations rather than directly indexing into the underlying array.

## 6. Why not use a plain array without stack discipline?
A plain array can store student records, but it does not itself enforce LIFO operations or provide a clear record of the latest deletion. The stack's `push()` and `pop()` operations make the most recently added record the first one removed. The separate history stack preserves removed records in LIFO order, allowing the most recent deletion to be restored by popping history and pushing the record onto the main stack. A plain array could implement undo too, but it would require additional tracking and custom logic; the stack-based design makes the intended behavior explicit.

## 7. Validation and edge cases
- Rejects duplicate roll numbers among active students.
- Requires a positive roll number and age between 1 and 150.
- Validates CGPA from 0.0 through 10.0.
- Handles invalid integer/decimal input, invalid menu options, empty stacks, and a full main stack with messages.
- Add clears old deletion history so Undo does not reverse a deletion from before a newer Add.
- Undo checks for a duplicate roll number and available main-stack capacity before changing either stack.
- Exit clears both stacks.

## 8. Menu features
1. Add Student
2. Remove Last-Added Student
3. View Most Recently Added Student
4. Display All Students
5. Search Student by Roll Number
6. Undo Last Delete
7. Exit
