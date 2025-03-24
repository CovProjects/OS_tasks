#include <stdio.h>
#include <stdlib.h>

#define TIME_QUANTUM 3

typedef struct Process 
{
    int id;
    int start_time;
    int end_time;
    int burst_time;
    int remaining_time;
    int state; // 0 = not started, 1 = started, 2 = finished
} Process;

void fcfs(Process* processes, int n) 
{
    int current_time = 0;

    for (int i = 0; i < n; i++) 
    {
        if (current_time < processes[i].start_time)
            current_time = processes[i].start_time;

        current_time += processes[i].burst_time;
        processes[i].end_time = current_time;
        processes[i].state = 2;
    }
}

void sjf(Process* processes, int n) 
{
    int completed = 0, current_time = 0;

    while (completed < n) 
    {
        int shortest_index = -1, min_burst_time = -1;

        for (int i = 0; i < n; i++) 
        {
            if (processes[i].state == 0 && processes[i].start_time <= current_time) 
            {
                if (min_burst_time == -1 || processes[i].burst_time < min_burst_time) 
                {
                    min_burst_time = processes[i].burst_time;
                    shortest_index = i;
                }
            }
        }

        if (shortest_index == -1) 
        {
            current_time++;
            continue;
        }

        Process* p = &processes[shortest_index];
        current_time += p->burst_time;
        p->end_time = current_time;
        p->state = 2;
        completed++;
    }
}

void round_robin(Process* processes, int processes_count) 
{
    int current_time = 0;
    int remaining_processes = processes_count; 

    while (remaining_processes > 0) 
    {
        int executed = 0;

        for (int i = 0; i < processes_count; i++) 
        {
            if (processes[i].remaining_time > 0 && processes[i].start_time <= current_time) 
            {
                int execute_time = (processes[i].remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : processes[i].remaining_time;
                current_time += execute_time;
                processes[i].remaining_time -= execute_time;
                executed = 1;

                if (processes[i].remaining_time == 0) 
                {
                    processes[i].end_time = current_time;
                    processes[i].state = 2;
                    remaining_processes--; 
                }
            }
        }

        if (!executed) 
        {
            int next_start_time = 999;
            for (int i = 0; i < processes_count; i++) 
            {
                if (processes[i].remaining_time > 0 && processes[i].start_time > current_time) 
                {
                    next_start_time = (processes[i].start_time < next_start_time) ? processes[i].start_time : next_start_time;
                }
            }
            if (next_start_time != 999) 
            {
                current_time = next_start_time;
            }
        }
    }
}

void reset_processes(Process* processes, int n) 
{
    for (int i = 0; i < n; i++) 
    {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].end_time = 0;
        processes[i].state = 0;
    }
}

void calculate_avg_times(Process* processes, int n) 
{
    int total_turnaround_time = 0;
    int total_wait_time = 0;

    for (int i = 0; i < n; i++) 
    {
        int turnaround_time = processes[i].end_time - processes[i].start_time;
        int wait_time = turnaround_time - processes[i].burst_time;
        total_turnaround_time += turnaround_time;
        total_wait_time += wait_time;
    }

    printf("Average Wait Time: %d\n", total_wait_time / n);
}

int main() {
    Process processes[] = 
    {
        {1, 4, 0, 8, 3, 0},
        {2, 1, 0, 4, 5, 0},
        {3, 2, 0, 9, 1, 0},
        {4, 3, 0, 5, 3, 0}
    };
    int n = sizeof(processes) / sizeof(processes[0]);

    printf("FCFS: \n");
    reset_processes(processes, n);
    fcfs(processes, n);
    calculate_avg_times(processes, n);

    printf("SJF: \n");
    reset_processes(processes, n);
    sjf(processes, n);
    calculate_avg_times(processes, n);

    printf("Round Robin: \n");
    reset_processes(processes, n);
    round_robin(processes, n);
    calculate_avg_times(processes, n);

    return 0;
}
