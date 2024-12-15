#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROCESSES 10 // Maximum number of processes the program can handle
//#define TIME_QUANTUM 3   // Fixed time quantum for Round Robin scheduling - have to change this to ask for input
int timeQuantum;

// Define process statuses as an enumeration
typedef enum {
    READY,      // Process is ready and waiting for CPU time
    RUNNING,    // Process is currently using the CPU
    BLOCKED,    // Process is waiting for I/O to complete
    COMPLETED   // Process has finished execution
} ProcessStatus;

// Define Gantt chart entry
typedef struct {
    int processID;
    int startTime;
    int endTime;
} GanttEntry;

// Define Process structure to hold all attributes of a process
typedef struct {
    int processID;
    int arrivalTime;
    int cpuBurstTime;        // Total CPU time required by the process
    int ioWaitTime;          // I/O wait time during execution
    int remainingTime;       // Remaining CPU burst time
    int ioWaitRemaining;     // Remaining I/O wait time if the process is BLOCKED
    int startTime;           // Time the process first started execution
    int completionTime;      // Time when the process completes
    int waitingTime;         // Time spent waiting in the ready queue
    int turnaroundTime;      // Turnaround time for the process
    ProcessStatus status;    // Current process status (READY, RUNNING, BLOCKED, COMPLETED)
} Process;

// Function to create and initialize a process
Process createProcess(int id, int arrival, int burst, int ioWait) {
    Process p;
    p.processID = id;
    p.arrivalTime = arrival;        // Set the process's arrival time
    p.cpuBurstTime = burst;         // Set the total CPU time required
    p.ioWaitTime = ioWait;          // Set the I/O wait time
    p.remainingTime = burst;        // Initialize remaining CPU time as the full burst time
    p.ioWaitRemaining = 0;          // Initialize remaining I/O wait time to zero
    p.startTime = -1;               // Mark start time as uninitialized
    p.completionTime = 0;           // Initialize completion time
    p.waitingTime = 0;              // Initialize waiting time
    p.turnaroundTime = 0;           // Initialize turnaround time
    p.status = READY;               // Set the initial status to READY
    return p;
}

// Function to display input error messages
void displayInputError(const char* message) {
    printf("\n[ERROR] %s. Please try again.\n\n", message);
}

// Function to display the details of a process
void displayProcess(Process p) {
    const char* statusStrings[] = {"READY", "RUNNING", "BLOCKED", "COMPLETED"};
    printf("Process ID: %d\n", p.processID);
    printf("Arrival Time: %d\n", p.arrivalTime);
    printf("CPU Burst Time: %d\n", p.cpuBurstTime);
    printf("Remaining Time: %d\n", p.remainingTime);
    printf("I/O Wait Time: %d\n", p.ioWaitTime);
    printf("Status: %s\n\n", statusStrings[p.status]);
}

// Function to validate input
int validateInput(int value, int min, int max) {
    return value >= min && value <= max;
}

// Function to get validated integer input
int getValidatedInput(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            displayInputError("Invalid input. Enter an integer");
            while (getchar() != '\n'); // Clear input buffer
        } else if (!validateInput(value, min, max)) {
            displayInputError("Value out of range");
        } else {
            return value;
        }
    }
}

// Function to find the next process arrival time
int findNextArrival(Process processes[], int n, int currentTime) {
    int nextArrival = -1;
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalTime > currentTime && processes[i].status != COMPLETED) {
            if (nextArrival == -1 || processes[i].arrivalTime < nextArrival) {
                nextArrival = processes[i].arrivalTime;
            }
        }
    }
    return nextArrival;
}

// Function to simulate Round Robin scheduling
//void roundRobinScheduling(Process processes[], int n) {

//    GanttEntry ganttChart[MAX_PROCESSES * 10]; // To store Gantt chart entries
//    int ganttIndex = 0;  // Track Gantt chart entries
//
//    int time = 0;           // Current simulation time
//    int completed = 0;      // Number of processes that have completed execution
//
//    while (completed < n) { // Continue until all processes are completed
//        int processExecuted = 0;
//
//        for (int i = 0; i < n; i++) {
//            // Skip processes that are already completed or not yet arrived
//            if (processes[i].status == COMPLETED || processes[i].arrivalTime > time)
//                continue;
//
//            // Handle processes in the BLOCKED state
//            if (processes[i].status == BLOCKED && processes[i].ioWaitRemaining > 0) {
//                processes[i].ioWaitRemaining--; // Decrease the I/O wait time
//                if (processes[i].ioWaitRemaining == 0) {
//                    processes[i].status = READY; // Move back to READY once I/O wait is done
//                }
//                continue;
//            }
//
//            // Handle processes in the READY or RUNNING state
//            if (processes[i].remainingTime > 0 && processes[i].status != COMPLETED) {
//                if (processes[i].startTime == -1) {
//                    processes[i].startTime = time;  // Record first execution time
//                }
//                processes[i].status = RUNNING; // Set the status to RUNNING
//
//                // Determine how much time the process will run in this time slice
//                int execTime = processes[i].remainingTime > timeQuantum ? timeQuantum : processes[i].remainingTime;
//                processes[i].remainingTime -= execTime; // Reduce the remaining time
//                time += execTime;                       // Advance the simulation time
//                processExecuted = 1; // A process was executed
//
//                // Add to Gantt chart
//                ganttChart[ganttIndex].processID = processes[i].processID;
//                ganttChart[ganttIndex].startTime = time - execTime;
//                ganttChart[ganttIndex].endTime = time;
//                ganttIndex++;
//
//                // Check if the process is completed
//                if (processes[i].remainingTime <= 0) {
//                    processes[i].status = COMPLETED;
//                    processes[i].completionTime = time; // Record completion time
//                    processes[i].turnaroundTime = time - processes[i].arrivalTime;
//                    processes[i].waitingTime = processes[i].turnaroundTime - processes[i].cpuBurstTime;
//                    completed++;
//                } else if (processes[i].ioWaitTime > 0) {
//                    processes[i].status = BLOCKED;          // Move to BLOCKED if I/O is required
//                    processes[i].ioWaitRemaining = processes[i].ioWaitTime; // Set I/O wait time
//                } else {
//                    processes[i].status = READY; // Move back to READY if not completed
//                }
//                displayProcess(processes[i]);
//            }
//        }
//
//        if (!processExecuted) {
//            int nextArrival = findNextArrival(processes, n, time);
//            if (nextArrival != -1) {
//                time = nextArrival;
//            }
//        }
//    }

// Function to display process details in table format
void displayProcessInTable(int time, Process p) {
    const char* statusStrings[] = {"READY", "RUNNING", "BLOCKED", "COMPLETED"};
    printf("%-5d %-10d %-10s %-15d\n", time, p.processID, statusStrings[p.status], p.remainingTime);
}

void roundRobinScheduling(Process processes[], int n) {
    GanttEntry ganttChart[MAX_PROCESSES * 10]; // To store Gantt chart entries
    int ganttIndex = 0;  // Track Gantt chart entries

    int time = 0;           // Current simulation time
    int completed = 0;      // Number of processes that have completed execution

    printf("\nExecution Table:\n\n");
    printf("%-5s %-10s %-10s %-15s\n", "Time", "Process ID", "Status", "Remaining Time");
    printf("-----------------------------------------------------\n");

    while (completed < n) { // Continue until all processes are completed
        int processExecuted = 0;

        for (int i = 0; i < n; i++) {
            // Skip processes that are already completed or not yet arrived
            if (processes[i].status == COMPLETED || processes[i].arrivalTime > time)
                continue;

            // Handle processes in the BLOCKED state
            if (processes[i].status == BLOCKED && processes[i].ioWaitRemaining > 0) {
                processes[i].ioWaitRemaining--; // Decrease the I/O wait time
                if (processes[i].ioWaitRemaining == 0) {
                    processes[i].status = READY; // Move back to READY once I/O wait is done
                }
                continue;
            }

            // Handle processes in the READY or RUNNING state
            if (processes[i].remainingTime > 0 && processes[i].status != COMPLETED) {
                if (processes[i].startTime == -1) {
                    processes[i].startTime = time;  // Record first execution time
                }
                processes[i].status = RUNNING; // Set the status to RUNNING

                // Determine how much time the process will run in this time slice
                int execTime = processes[i].remainingTime > timeQuantum ? timeQuantum : processes[i].remainingTime;
                processes[i].remainingTime -= execTime; // Reduce the remaining time
                time += execTime;                       // Advance the simulation time
                processExecuted = 1; // A process was executed

                // Add to Gantt chart
                ganttChart[ganttIndex].processID = processes[i].processID;
                ganttChart[ganttIndex].startTime = time - execTime;
                ganttChart[ganttIndex].endTime = time;
                ganttIndex++;

                // Check if the process is completed
                if (processes[i].remainingTime <= 0) {
                    processes[i].status = COMPLETED;
                    processes[i].completionTime = time; // Record completion time
                    processes[i].turnaroundTime = time - processes[i].arrivalTime;
                    processes[i].waitingTime = processes[i].turnaroundTime - processes[i].cpuBurstTime;
                    completed++;
                } else if (processes[i].ioWaitTime > 0) {
                    processes[i].status = BLOCKED;          // Move to BLOCKED if I/O is required
                    processes[i].ioWaitRemaining = processes[i].ioWaitTime; // Set I/O wait time
                } else {
                    processes[i].status = READY; // Move back to READY if not completed
                }
                // Display process in table format
                displayProcessInTable(time, processes[i]);
            }
        }

        if (!processExecuted) {
            int nextArrival = findNextArrival(processes, n, time);
            if (nextArrival != -1) {
                time = nextArrival;
            }
        }
    }


    printf("\nGantt Chart:\n");
    for (int i = 0; i < ganttIndex; i++) {
        printf("| P%d [%d - %d] ", ganttChart[i].processID, ganttChart[i].startTime, ganttChart[i].endTime);
    }
    printf("\n\nPerformance Metrics:\n");

    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0, totalCpuTime = 0;
    for (int i = 0; i < n; i++) {
        int responseTime = processes[i].startTime - processes[i].arrivalTime;
        printf("Process ID: %d\n", processes[i].processID);
        printf("Turnaround Time: %d\n", processes[i].turnaroundTime);
        printf("Waiting Time: %d\n", processes[i].waitingTime);
        printf("Response Time: %d\n\n", responseTime);

        totalTurnaround += processes[i].turnaroundTime;
        totalWaiting += processes[i].waitingTime;
        totalResponse += responseTime;
        totalCpuTime += processes[i].cpuBurstTime;
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTurnaround / n);
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
    printf("Average Response Time: %.2f\n", (float)totalResponse / n);
    printf("CPU Utilization: %.2f%%\n", (float)totalCpuTime / time * 100);
}


int main() {
    int n = getValidatedInput("Enter number of processes (1-10): ", 1, MAX_PROCESSES);
    timeQuantum = getValidatedInput ("Enter Time Quantum (TQ): ", 1, 10);
    Process processes[MAX_PROCESSES]; // Array to hold process information

    // Input process details
    for (int i = 0; i < n; i++) {
        printf("\nProcess %d", i+1);
        int arrival = getValidatedInput("\nEnter arrival time: ", 0, 1000);
        int burst = getValidatedInput("Enter CPU burst time: ", 1, 1000);
        int ioWait = getValidatedInput("Enter I/O wait time: ", 0, 1000);
        printf("\n");

        processes[i] = createProcess(i + 1, arrival, burst, ioWait); // Create and store process
    }

    roundRobinScheduling(processes, n);
    return 0;
}
