#include <stdio.h>
#include <stdbool.h>

int main() {
    FILE *inputs = fopen("inputs.txt", "r");
    if (!inputs) {
        perror("Failed to open file");
        return 1;
    }

    int P; // Number of processes
    int R; // Number of process types
    fscanf(inputs, "P = %d\n", &P);
    fscanf(inputs, "R = %d\n", &R);

    int E[R];            // Number of resource of each type
    int C_matrix[P][R];  // Current allocation matrix
    int R_matrix[P][R];  // Request matrix
    int available[R];    // Available resources
    int work[R];
    bool marked[P];

    fscanf(inputs, "E =");
    for (int i = 0; i < R; i++) {
        fscanf(inputs, "%d", &E[i]);
    }

    fscanf(inputs, "\nC =");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            fscanf(inputs, "%d", &C_matrix[i][j]);
        }
    }

    fscanf(inputs, "\nR =");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            fscanf(inputs, "%d", &R_matrix[i][j]);
        }
    }

    for (int i = 0; i < R; i++) {
        available[i] = E[i];
        for (int j = 0; j < P; j++) {
            available[i] -= C_matrix[j][i];
        }
        printf("Available R%i : %d \n", i, available[i]);
    }

    printf("P = %d\n", P);
    printf("R = %d\n", R);

    printf("E = "); { 
    for (int i = 0; i < R; i++) printf("%d ", E[i]);
    }

    printf("\nC =\n");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) printf("%d ", C_matrix[i][j]);
        printf("\n");
    }

    printf("R =\n");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) printf("%d ", R_matrix[i][j]);
        printf("\n");
    }


    // Processes with 0 allocation
    for (int i = 0; i < P; i++) {  
        int count = 0;  
        for (int j = 0; j < R; j++) {  
            if (C_matrix[i][j] == 0) {  
                count++;
            }
            else {  
                break;  
            }  
        }
        if (count == R) {  
        marked[i] = true;  
        }  
    }

    // Initializing work with available
    for (int i = 0; i < R; i++) {
        work[i] = available[i];
    }

    // Mark processes with requests <= W
    for (int i = 0; i < P; i++) {
        bool processable = false;
        if (!marked[i]) {
            for (int j = 0; j < R; j++) {
                if (R_matrix[i][j] <= work[j]) {
                    processable = true;
                }
                else {
                    break;
                }
            }
            if (processable) {
                marked[i] = true;

                for (int j = 0; j < R; j++) {
                    work[j] += C_matrix[i][j];
                }
            }
        }
    }

    for (int i = 0; i < P; i++) {
        if (!marked[i]) {
            printf("Deadlock at process %d \n", i);
        }
    }

    fclose(inputs);
    return 0;
}
