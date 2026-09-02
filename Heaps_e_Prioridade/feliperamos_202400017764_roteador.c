#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **dados;   
    int qtd;        
    int tamanho;    
} DadosPacote;

typedef struct {
    int prioridade; 
    int ordem;
    DadosPacote pacote;
} HeapItem;

#define ESQ(i) (2*(i) + 1)
#define DIR(i) (2*(i) + 2)

void trocar(HeapItem *a, HeapItem *b) {
    HeapItem tmp = *a;
    *a = *b;
    *b = tmp;
}

void max_heapify(HeapItem *arr, int n, int i) {
    int maior = i;
    int e = ESQ(i);
    int d = DIR(i);

    if (e < n) {
        if (arr[e].prioridade < arr[maior].prioridade) maior = e;
        else if (arr[e].prioridade == arr[maior].prioridade &&
                 arr[e].ordem < arr[maior].ordem) maior = e;
    }

    if (d < n) {
        if (arr[d].prioridade < arr[maior].prioridade) maior = d;
        else if (arr[d].prioridade == arr[maior].prioridade &&
                 arr[d].ordem < arr[maior].ordem) maior = d;
    }

    if (maior != i) {
        trocar(&arr[i], &arr[maior]);
        max_heapify(arr, n, maior);
    }
}

void construir_max_heap(HeapItem *arr, int n) {
    for (int i = n/2 - 1; i >= 0; i--)
        max_heapify(arr, n, i);
}

HeapItem heappop_manual(HeapItem *arr, int *n) {
    trocar(&arr[0], &arr[*n - 1]);
    HeapItem max_item = arr[*n - 1];
    (*n)--;

    if (*n > 0)
        max_heapify(arr, *n, 0);

    return max_item;
}

void escrever_linha(FILE *f, HeapItem *batch, int count) {
    fprintf(f, "|");

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < batch[i].pacote.qtd; j++) {
            fprintf(f, "%s", batch[i].pacote.dados[j]);
            if (j + 1 < batch[i].pacote.qtd)
                fprintf(f, ",");
        }
        if (i + 1 < count)
            fprintf(f, "|");
    }

    fprintf(f, "|\n");
}


int main(int argc, char **argv) {
    printf("#ARGS = %d\n", argc);
    printf("PROGRAMA = %s\n", argv[0]);

    if (argc < 3) {
        printf("Uso correto: program <input> <output>\n");
        return 1;
    }

    printf("ARG1 = %s, ARG2 = %s\n", argv[1], argv[2]);

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    if (!input || !output) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }

    int total_pacotes, limite_bytes;
    fscanf(input, "%d %d", &total_pacotes, &limite_bytes);
    fgetc(input); 

    HeapItem *pacotes = malloc(total_pacotes * sizeof(HeapItem));
    int ordem_global = 0;


    for (int i = 0; i < total_pacotes; i++) {
        int prioridade, tamanho;
        fscanf(input, "%d %d", &prioridade, &tamanho);
        fgetc(input); 

        char linha[4096];
        fgets(linha, sizeof(linha), input); 

        DadosPacote dp;
        dp.tamanho = tamanho;
        dp.qtd = 0;
        dp.dados = malloc(512 * sizeof(char*));

        char *token = strtok(linha, " \n");
        while (token != NULL) {
            dp.dados[dp.qtd] = malloc(strlen(token) + 1);
            strcpy(dp.dados[dp.qtd], token);
            dp.qtd++;
            token = strtok(NULL, " \n");
        }

        pacotes[i].prioridade = -prioridade;  
        pacotes[i].ordem = ordem_global++;
        pacotes[i].pacote = dp;
    }

    int pos = 0;

    while (pos < total_pacotes) {
        int bytes = 0;
        int fim = pos;

        while (fim < total_pacotes &&
               bytes + pacotes[fim].pacote.tamanho <= limite_bytes) {
            bytes += pacotes[fim].pacote.tamanho;
            fim++;
        }

        if (fim == pos) break;

        int janela_tam = fim - pos;
        HeapItem *heap = malloc(janela_tam * sizeof(HeapItem));

        for (int i = 0; i < janela_tam; i++)
            heap[i] = pacotes[pos + i];

        construir_max_heap(heap, janela_tam);

        HeapItem *ordenado = malloc(janela_tam * sizeof(HeapItem));
        int qtd = 0;

        while (janela_tam > 0)
            ordenado[qtd++] = heappop_manual(heap, &janela_tam);

        escrever_linha(output, ordenado, qtd);

        free(heap);
        free(ordenado);

        pos = fim;
    }

    fclose(input);
    fclose(output);
    return 0;
}