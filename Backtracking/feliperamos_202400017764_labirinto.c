#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nome[2];
    int dl;
    int dc;
} Movimento;

const Movimento MOVIMENTOS[] = {
    {"D", 0, 1},
    {"F", -1, 0},
    {"E", 0, -1},
    {"T", 1, 0}
};

typedef struct {
    char **entries;
    int count;
    int capacity;
} LogList;

void init_log(LogList *log) {
    log->count = 0;
    log->capacity = 100;
    log->entries = (char **)malloc(log->capacity * sizeof(char *));
}

void add_log(LogList *log, const char *str) {
    if (log->count >= log->capacity) {
        log->capacity *= 2;
        log->entries = (char **)realloc(log->entries, log->capacity * sizeof(char *));
    }

    log->entries[log->count] = (char *)malloc(strlen(str) + 1);
    strcpy(log->entries[log->count], str);
    log->count++;
}

void free_log(LogList *log) {
    for (int i = 0; i < log->count; i++) {
        free(log->entries[i]);
    }
    free(log->entries);
    log->count = 0;
}

int eh_saida(int l, int c, int w, int h, int inicio_l, int inicio_c) {
    int na_borda = (l == 0 || l == h - 1 || c == 0 || c == w - 1);
    int nao_eh_inicio = (l != inicio_l || c != inicio_c);
    return na_borda && nao_eh_inicio;
}


int resolver_backtracking(char **grid, int l, int c, int w, int h, 
                          unsigned char *visitados, LogList *log, 
                          int inicio_l, int inicio_c) {
    
    char buffer[128]; 

    if (eh_saida(l, c, w, h, inicio_l, inicio_c)) {
        sprintf(buffer, "FIM@%d,%d", l, c);
        add_log(log, buffer);
        return 1; 
    }

    for (int i = 0; i < 4; i++) {
        const char *nome_mov = MOVIMENTOS[i].nome;
        int dl = MOVIMENTOS[i].dl;
        int dc = MOVIMENTOS[i].dc;

        int nl = l + dl;
        int nc = c + dc;

        if (nl >= 0 && nl < h && nc >= 0 && nc < w) {
            
            char val = grid[nl][nc];
            int idx = nl * w + nc;

            if (val != '1' && !visitados[idx]) {
                
                visitados[idx] = 1;
                
                sprintf(buffer, "%s->%d,%d", nome_mov, nl, nc);
                add_log(log, buffer);

                if (resolver_backtracking(grid, nl, nc, w, h, visitados, log, inicio_l, inicio_c)) {
                    return 1;
                }

                sprintf(buffer, "BT@%d,%d->%d,%d", nl, nc, l, c);
                add_log(log, buffer);
            }
        }
    }

    return 0; 
}

int main(int argc, char* argv[]) {
    printf("#ARGS = %i\n", argc);
    printf("PROGRAMA = %s\n", argv[0]);
    
    if (argc > 2) {
        printf("ARG1 = %s, ARG2 = %s\n", argv[1], argv[2]);
    } else {
        return 1; 
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    if (!input || !output) {
        if (input) fclose(input);
        if (output) fclose(output);
        return 1;
    }

    int num_labirintos;
    if (fscanf(input, "%d", &num_labirintos) == 1) {

        for (int i = 0; i < num_labirintos; i++) {
            int largura, altura;
            
            if (fscanf(input, "%d %d", &largura, &altura) != 2) break;

            char **grid = (char **)malloc(altura * sizeof(char *));
            for(int r = 0; r < altura; r++) {
                grid[r] = (char *)malloc(largura * sizeof(char));
            }

            unsigned char *visitados = (unsigned char *)calloc(largura * altura, sizeof(unsigned char));

            int inicio_l = -1, inicio_c = -1;
            char token[100]; 

            for (int l = 0; l < altura; l++) {
                for (int c = 0; c < largura; c++) {
                    fscanf(input, "%s", token);
                    grid[l][c] = token[0];
                    
                    if (grid[l][c] == 'X') {
                        inicio_l = l;
                        inicio_c = c;
                    }
                }
            }

            LogList log;
            init_log(&log);
            if (inicio_l != -1) {
            visitados[inicio_l * largura + inicio_c] = 1;
            
            char buffer[128]; 
            sprintf(buffer, "INI@%d,%d", inicio_l, inicio_c);
            add_log(&log, buffer);
            
                int encontrou = resolver_backtracking(grid, inicio_l, inicio_c, largura, altura, 
                                                      visitados, &log, inicio_l, inicio_c);
                
                fprintf(output, "L%d:", i);
                for (int k = 0; k < log.count; k++) {
                    fprintf(output, "%s", log.entries[k]);
                    if (k < log.count - 1) {
                        fprintf(output, "|");
                    }
                }

                if (!encontrou) {
                    if (log.count > 0) fprintf(output, "|");
                    fprintf(output, "FIM@-,-");
                }
                fprintf(output, "\n");

            } else {
                fprintf(output, "L%d:ERRO: Sem ponto de partida\n", i);
            }

            for(int r = 0; r < altura; r++) {
                free(grid[r]);
            }
            free(grid);
            free(visitados);
            free_log(&log);
        }
    }
    
    fclose(input);
    fclose(output);
    return 0;
}