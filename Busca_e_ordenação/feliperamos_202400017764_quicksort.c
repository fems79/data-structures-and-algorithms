#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long trocas;
    long chamadas;
} Estatisticas;

Estatisticas EST;

static inline void trocar(int *arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    EST.trocas++;
}

int pivo_mediana_de_3(int *arr, int inicio, int fim) {
    int tam = fim - inicio + 1;
    if (tam < 3) return fim;

    int ind1 = inicio + (tam / 4);
    int ind2 = inicio + (tam / 2);
    int ind3 = inicio + ((3 * tam) / 4);

    if (ind1 < inicio) ind1 = inicio;
    if (ind1 > fim) ind1 = fim;
    if (ind2 < inicio) ind2 = inicio;
    if (ind2 > fim) ind2 = fim;
    if (ind3 < inicio) ind3 = inicio;
    if (ind3 > fim) ind3 = fim;

    int vals[3] = { arr[ind1], arr[ind2], arr[ind3] };
    int inds[3] = { ind1, ind2, ind3 };

    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 2 - a; b++) {
            if (vals[b] > vals[b+1]) {
                int tv = vals[b]; vals[b] = vals[b+1]; vals[b+1] = tv;
                int ti = inds[b]; inds[b] = inds[b+1]; inds[b+1] = ti;
            }
        }
    }

    return inds[1];
}

int pivot_fim(int *arr, int inicio, int fim) {
    (void)arr;
    (void)inicio;
    return fim;
}
int pivot_inicio(int *arr, int inicio, int fim) {
    (void)arr;
    (void)fim;
    return inicio;
}

int pivot_aleatorio(int *arr, int inicio, int fim) {
    if (inicio >= fim) return inicio;
    int tam = fim - inicio + 1;
    int desloc = abs(arr[inicio]) % tam;
    return inicio + desloc;
}

int particao_lomuto(int *arr, int inicio, int fim) {
    int pivo = arr[fim];
    int i = inicio - 1;

    for (int j = inicio; j < fim; j++) {
        if (arr[j] <= pivo) {
            i++;
            trocar(arr, i, j);
        }
    }

    trocar(arr, i + 1, fim);
    return i + 1;
}

int particao_hoare(int *arr, int inicio, int fim) {
    int pivo = arr[inicio];
    int i = inicio - 1;
    int j = fim + 1;

    while (1) {
        do { i++; } while (arr[i] < pivo);
        do { j--; } while (arr[j] > pivo);

        if (i >= j)
            return j;

        trocar(arr, i, j);
    }
}

void quicksort_lomuto(int *arr, int inicio, int fim,
                      int (*pivot_func)(int*,int,int), const char *nome_pivo) {
    while (inicio < fim) {
        EST.chamadas++;

        int ind_pivo = pivot_func(arr, inicio, fim);

        if (!(nome_pivo[0] == 'L' && nome_pivo[1] == 'P')) {
            if (ind_pivo < inicio || ind_pivo > fim)
                ind_pivo = fim;
            trocar(arr, ind_pivo, fim);
        }

        int p = particao_lomuto(arr, inicio, fim);

        if ((p - 1 - inicio) < (fim - (p + 1))) {
            quicksort_lomuto(arr, inicio, p - 1, pivot_func, nome_pivo);
            inicio = p + 1;
        } else {
            quicksort_lomuto(arr, p + 1, fim, pivot_func, nome_pivo);
            fim = p - 1;
        }
    }
    EST.chamadas++;
}

void quicksort_hoare(int *arr, int inicio, int fim,
                     int (*pivot_func)(int*,int,int), const char *nome_pivo) {
    while (inicio < fim) {
        EST.chamadas++;

        int ind_pivo = pivot_func(arr, inicio, fim);


        if (!(nome_pivo[0] == 'H' && nome_pivo[1] == 'P')) {
            if (ind_pivo < inicio || ind_pivo > fim)
                ind_pivo = inicio;
            trocar(arr, ind_pivo, inicio);
        }

        int p = particao_hoare(arr, inicio, fim);

        if ((p - inicio) < (fim - (p + 1))) {
            quicksort_hoare(arr, inicio, p, pivot_func, nome_pivo);
            inicio = p + 1;
        } else {
            quicksort_hoare(arr, p + 1, fim, pivot_func, nome_pivo);
            fim = p;
        }
    }
    EST.chamadas++;
}

long executar_variante(int *arr, int n, char esquema,
                      int (*pivot)(int*,int,int), const char *nome) {
    EST.trocas = 0;
    EST.chamadas = 0;

    int *copia = malloc(n * sizeof(int));
    if (!copia) {
        fprintf(stderr, "Erro de memória\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) copia[i] = arr[i];

    if (n > 0) {
        if (esquema == 'L')
            quicksort_lomuto(copia, 0, n - 1, pivot, nome);
        else
            quicksort_hoare(copia, 0, n - 1, pivot, nome);
    } else {
        EST.chamadas++;
    }

    free(copia);

    return EST.trocas + EST.chamadas;
}

typedef struct {
    char nome[4]; 
    long ops;
} Resultado;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");

    if (!fin) {
        fprintf(stderr, "Erro ao abrir arquivo de entrada.\n");
        return 1;
    }
    if (!fout) {
        fprintf(stderr, "Erro ao abrir arquivo de saída.\n");
        fclose(fin);
        return 1;
    }

    int num_vetores = 0;
    if (fscanf(fin, "%d", &num_vetores) != 1) {
        fprintf(stderr, "Erro ao ler número de vetores.\n");
        fclose(fin);
        fclose(fout);
        return 1;
    }

    for (int i = 0; i < num_vetores; i++) {
        int n;
        if (fscanf(fin, "%d", &n) != 1) {
            fprintf(stderr, "Aviso: Linha %d do vetor mal formatada. Pulando.\n", i+1);
            char buffer[4096];
            if (!fgets(buffer, sizeof(buffer), fin)) { }
            continue;
        }

        int *arr = NULL;
        if (n > 0) {
            arr = malloc(n * sizeof(int));
            if (!arr) { fprintf(stderr, "Erro de memória\n"); fclose(fin); fclose(fout); return 1; }
            for (int k = 0; k < n; k++) {
                if (fscanf(fin, "%d", &arr[k]) != 1) {
                    fprintf(stderr, "Aviso: vetor %d mal formatado. Pulando.\n", i+1);
                    free(arr);
                    arr = NULL;
                    break;
                }
            }
        } else {
            arr = NULL;
        }

        if (!arr && n > 0) {
            continue;
        }

        Resultado r[6];

        strcpy(r[0].nome, "LP");
        r[0].ops = executar_variante(arr ? arr : (int[]){0}, n, 'L', pivot_fim, "LP");

        strcpy(r[1].nome, "LM");
        r[1].ops = executar_variante(arr ? arr : (int[]){0}, n, 'L', pivo_mediana_de_3, "LM");

        strcpy(r[2].nome, "LA");
        r[2].ops = executar_variante(arr ? arr : (int[]){0}, n, 'L', pivot_aleatorio, "LA");

        strcpy(r[3].nome, "HP");
        r[3].ops = executar_variante(arr ? arr : (int[]){0}, n, 'H', pivot_inicio, "HP");

        strcpy(r[4].nome, "HM");
        r[4].ops = executar_variante(arr ? arr : (int[]){0}, n, 'H', pivo_mediana_de_3, "HM");

        strcpy(r[5].nome, "HA");
        r[5].ops = executar_variante(arr ? arr : (int[]){0}, n, 'H', pivot_aleatorio, "HA");

        for (int j = 1; j < 6; j++) {
            Resultado chave = r[j];
            int k = j - 1;
            while (k >= 0 && r[k].ops > chave.ops) {
                r[k + 1] = r[k];
                k--;
            }
            r[k + 1] = chave;
        }

        fprintf(fout, "[%d]:", n);
        for (int j = 0; j < 6; j++) {
            fprintf(fout, "%s(%ld)", r[j].nome, r[j].ops);
            if (j < 5) fprintf(fout, ",");
        }

        if (i < num_vetores - 1) fprintf(fout, "\n");

        free(arr);
    }

    fprintf(fout, "\n");
    fclose(fin);
    fclose(fout);
    return 0;
}
