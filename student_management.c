/*
 * Student Management System Using a Stack
 * Language: C
 *
 * Main stack stores active student records. A separate history stack stores
 * records removed by the "Remove Last-Added Student" operation.
 * No built-in stack/collection is used; both stacks are implemented here
 * using fixed-size arrays and explicit push/pop/peek/isEmpty operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define MAX_STUDENTS 50
#define NAME_LENGTH 80
#define COURSE_LENGTH 80
#define INPUT_LENGTH 256

typedef struct {
    int rollNo;
    char name[NAME_LENGTH];
    int age;
    char course[COURSE_LENGTH];
    double cgpa;
} Student;

typedef struct {
    Student items[MAX_STUDENTS];
    int top;
} Stack;

/* ---------- Basic stack operations ---------- */

void initializeStack(Stack *stack) {
    stack->top = -1;
}

int isEmpty(const Stack *stack) {
    return stack->top == -1;
}

int isFull(const Stack *stack) {
    return stack->top == MAX_STUDENTS - 1;
}

/* push(student): add a record at the top of the stack. */
int push(Stack *stack, Student student) {
    if (isFull(stack)) {
        return 0;
    }
    stack->items[++stack->top] = student;
    return 1;
}

/* pop(): remove and return the top record. */
int pop(Stack *stack, Student *student) {
    if (isEmpty(stack)) {
        return 0;
    }
    *student = stack->items[stack->top--];
    return 1;
}

/* peek()/top(): read the top record without removing it. */
int peek(const Stack *stack, Student *student) {
    if (isEmpty(stack)) {
        return 0;
    }
    *student = stack->items[stack->top];
    return 1;
}

/* ---------- Safe input helpers ---------- */

int readLine(const char *prompt, char *buffer, size_t size) {
    int ch;
    size_t length;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        /* Discard the remainder of an overlong input line. */
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard */
        }
    }

    /* Trim leading spaces. */
    {
        char *start = buffer;
        while (*start == ' ' || *start == '\t' || *start == '\r') {
            start++;
        }
        if (start != buffer) {
            memmove(buffer, start, strlen(start) + 1);
        }
    }

    /* Trim trailing spaces. */
    length = strlen(buffer);
    while (length > 0 &&
           (buffer[length - 1] == ' ' || buffer[length - 1] == '\t' ||
            buffer[length - 1] == '\r')) {
        buffer[--length] = '\0';
    }
    return 1;
}

int readInt(const char *prompt, int *value) {
    char line[INPUT_LENGTH];
    char *end;
    long parsed;

    if (!readLine(prompt, line, sizeof(line))) {
        return 0;
    }
    errno = 0;
    parsed = strtol(line, &end, 10);
    if (line[0] == '\0' || *end != '\0' || errno == ERANGE ||
        parsed < INT_MIN || parsed > INT_MAX) {
        printf("Invalid integer. Please try again.\n");
        return -1;
    }
    *value = (int)parsed;
    return 1;
}

int readDouble(const char *prompt, double *value) {
    char line[INPUT_LENGTH];
    char *end;
    double parsed;

    if (!readLine(prompt, line, sizeof(line))) {
        return 0;
    }
    errno = 0;
    parsed = strtod(line, &end);
    if (line[0] == '\0' || *end != '\0' || errno == ERANGE) {
        printf("Invalid number. Please try again.\n");
        return -1;
    }
    *value = parsed;
    return 1;
}

int readNonEmptyString(const char *prompt, char *value, size_t size) {
    while (1) {
        if (!readLine(prompt, value, size)) {
            return 0;
        }
        if (value[0] == '\0') {
            printf("This field cannot be empty. Please try again.\n");
            continue;
        }
        return 1;
    }
}

/* ---------- Student utilities ---------- */

void printStudent(const Student *student) {
    printf("[Roll No: %d, Name: %s, Age: %d, Course: %s, CGPA: %.2f]",
           student->rollNo, student->name, student->age,
           student->course, student->cgpa);
}

void printTop(const Stack *stack) {
    Student topStudent;
    printf("Current top: ");
    if (peek(stack, &topStudent)) {
        printStudent(&topStudent);
    } else {
        printf("Stack is empty");
    }
    printf("\n");
}

/*
 * Search by roll number using only stack operations.
 * Records are moved to a temporary stack and then restored.
 */
int findStudentByRoll(Stack *mainStack, int rollNo, Student *foundStudent) {
    Stack temporary;
    Student current;
    int found = 0;

    initializeStack(&temporary);
    while (pop(mainStack, &current)) {
        if (current.rollNo == rollNo) {
            *foundStudent = current;
            found = 1;
        }
        /* The temporary stack cannot overflow because it receives
           records removed from a stack of the same maximum capacity. */
        push(&temporary, current);
    }

    while (pop(&temporary, &current)) {
        push(mainStack, current);
    }
    return found;
}

void displayAllStudents(Stack *mainStack) {
    Stack temporary;
    Student current;
    int count = 0;

    if (isEmpty(mainStack)) {
        printf("No student records to display. The main stack is empty.\n");
        return;
    }

    initializeStack(&temporary);
    printf("\n--- All Students (top to bottom) ---\n");

    while (pop(mainStack, &current)) {
        printStudent(&current);
        printf("\n");
        push(&temporary, current);
        count++;
    }

    /* Restore original order. */
    while (pop(&temporary, &current)) {
        push(mainStack, current);
    }

    printf("Total students: %d\n", count);
    printTop(mainStack);
}

void addStudent(Stack *mainStack, Stack *historyStack) {
    Student student;
    int result;

    if (isFull(mainStack)) {
        printf("Cannot add student: main stack is full (maximum %d).\n",
               MAX_STUDENTS);
        return;
    }

    result = readInt("Roll Number: ", &student.rollNo);
    if (result == 0) return;
    if (result < 0) return;
    if (student.rollNo <= 0) {
        printf("Roll number must be a positive integer.\n");
        return;
    }

    {
        Student existing;
        if (findStudentByRoll(mainStack, student.rollNo, &existing)) {
            printf("Duplicate roll number rejected. Roll number %d already exists.\n",
                   student.rollNo);
            return;
        }
    }

    if (!readNonEmptyString("Name: ", student.name, sizeof(student.name))) return;

    result = readInt("Age: ", &student.age);
    if (result == 0) return;
    if (result < 0) return;
    if (student.age <= 0 || student.age > 150) {
        printf("Age must be between 1 and 150.\n");
        return;
    }

    if (!readNonEmptyString("Course / Branch: ", student.course,
                            sizeof(student.course))) return;

    result = readDouble("CGPA (0.0 - 10.0): ", &student.cgpa);
    if (result == 0) return;
    if (result < 0) return;
    if (student.cgpa < 0.0 || student.cgpa > 10.0) {
        printf("Invalid CGPA. CGPA must be between 0.0 and 10.0.\n");
        return;
    }

    if (push(mainStack, student)) {
        /* A new add means the latest change is not a deletion.
           Clear stale deletion history so Undo cannot reverse an older action. */
        initializeStack(historyStack);
        printf("Pushed student successfully. ");
        printTop(mainStack);
    } else {
        printf("Cannot add student: main stack is full.\n");
    }
}

void removeLastAddedStudent(Stack *mainStack, Stack *historyStack) {
    Student removed;

    if (!pop(mainStack, &removed)) {
        printf("Cannot remove student: main stack is empty.\n");
        return;
    }

    if (!push(historyStack, removed)) {
        /* In normal use history cannot be full before the main stack has
           enough records removed, but handle the condition safely. */
        push(mainStack, removed);
        printf("Cannot save deletion history: history stack is full. No change made.\n");
        return;
    }

    printf("Removed ");
    printStudent(&removed);
    printf(" and saved to history. ");
    printTop(mainStack);
}

void viewMostRecentlyAdded(const Stack *mainStack) {
    Student student;

    if (peek(mainStack, &student)) {
        printf("Most recently added student (top of stack): ");
        printStudent(&student);
        printf("\n");
    } else {
        printf("No student available. The main stack is empty.\n");
    }
}

void searchStudent(Stack *mainStack) {
    int rollNo;
    int result;
    Student found;

    result = readInt("Enter roll number to search: ", &rollNo);
    if (result == 0) return;
    if (result < 0) return;

    if (findStudentByRoll(mainStack, rollNo, &found)) {
        printf("Student found: ");
        printStudent(&found);
        printf("\n");
    } else {
        printf("No student found with roll number %d.\n", rollNo);
    }
    printf("Search completed. ");
    printTop(mainStack);
}

void undoLastDelete(Stack *mainStack, Stack *historyStack) {
    Student lastDeleted;
    Student existing;

    if (!peek(historyStack, &lastDeleted)) {
        printf("Nothing to undo: deletion history is empty.\n");
        return;
    }

    if (findStudentByRoll(mainStack, lastDeleted.rollNo, &existing)) {
        printf("Undo blocked: roll number %d already exists in the main stack. "
               "Remove or resolve that record before undoing.\n", lastDeleted.rollNo);
        return;
    }

    if (isFull(mainStack)) {
        printf("Undo blocked: main stack is full.\n");
        return;
    }

    /* Pop from history only after validation, then push back to main stack. */
    pop(historyStack, &lastDeleted);
    push(mainStack, lastDeleted);
    printf("Undo successful. Restored ");
    printStudent(&lastDeleted);
    printf(". ");
    printTop(mainStack);
}

void clearStack(Stack *stack) {
    Student ignored;
    while (pop(stack, &ignored)) {
        /* Clear by repeated pop operations. */
    }
}

void displayMenu(void) {
    printf("\n===== Student Management System (Stack-based) =====\n");
    printf("1. Add Student\n");
    printf("2. Remove Last-Added Student\n");
    printf("3. View Most Recently Added Student\n");
    printf("4. Display All Students\n");
    printf("5. Search Student by Roll Number\n");
    printf("6. Undo Last Delete\n");
    printf("7. Exit\n");
}

int main(void) {
    Stack mainStack;
    Stack historyStack;
    int choice;
    int result;

    initializeStack(&mainStack);
    initializeStack(&historyStack);

    while (1) {
        displayMenu();
        result = readInt("Enter your choice: ", &choice);

        if (result == 0) {
            printf("\nEnd of input received. Clearing stacks and exiting.\n");
            break;
        }
        if (result < 0) {
            continue;
        }

        switch (choice) {
            case 1:
                addStudent(&mainStack, &historyStack);
                break;
            case 2:
                removeLastAddedStudent(&mainStack, &historyStack);
                break;
            case 3:
                viewMostRecentlyAdded(&mainStack);
                break;
            case 4:
                displayAllStudents(&mainStack);
                break;
            case 5:
                searchStudent(&mainStack);
                break;
            case 6:
                undoLastDelete(&mainStack, &historyStack);
                break;
            case 7:
                clearStack(&mainStack);
                clearStack(&historyStack);
                printf("Both stacks cleared. Exiting Student Management System.\n");
                return 0;
            default:
                printf("Invalid menu choice. Please enter a number from 1 to 7.\n");
        }
    }

    clearStack(&mainStack);
    clearStack(&historyStack);
    return 0;
}
