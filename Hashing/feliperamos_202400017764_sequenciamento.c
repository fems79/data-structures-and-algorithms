#include <stdio.h>
#include <string.h>

#define MAX_TEXTO 2000005    
#define MAX_GENE 20005      
#define MAX_DOENCAS 200005   
#define MAX_NOME 100        
#define TABLE_SIZE 4000037  

char dna_paciente[MAX_TEXTO];
int k_global;

typedef struct {
    int start_index;
    int occupied; 
} HashEntry;

HashEntry hashTable[TABLE_SIZE];

typedef struct {
    char nome[MAX_NOME];
    double prob;
    int ordem_original; 
} Resultado;

Resultado resultados[MAX_DOENCAS]; 
Resultado temp_array[MAX_DOENCAS]; 

unsigned long hash_func(char *str, int len) {
    unsigned long hash = 5381;
    for (int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash % TABLE_SIZE;
}

void hash_insert(int index) {
    unsigned long h = hash_func(&dna_paciente[index], k_global);
    
    while (hashTable[h].occupied) {
        if (strncmp(&dna_paciente[hashTable[h].start_index], &dna_paciente[index], k_global) == 0) {
            return;
        }
        h = (h + 1) % TABLE_SIZE;
    }
    
    hashTable[h].occupied = 1;
    hashTable[h].start_index = index;
}

int hash_search(char *padrao) {
    unsigned long h = hash_func(padrao, k_global);
    
    while (hashTable[h].occupied) {
        if (strncmp(&dna_paciente[hashTable[h].start_index], padrao, k_global) == 0) {
            return 1; 
        }
        h = (h + 1) % TABLE_SIZE;
    }
    return 0; 
}

void mesclar(Resultado *v, int ini, int meio, int fim) {
    int i = ini;
    int j = meio + 1;
    int k = ini; 

    while (i <= meio && j <= fim) {
        int p1 = (int)(v[i].prob + 0.5);
        int p2 = (int)(v[j].prob + 0.5);

        if (p1 > p2) {
            temp_array[k++] = v[i++];
        } else if (p1 < p2) {
            temp_array[k++] = v[j++];
        } else {
            if (v[i].ordem_original < v[j].ordem_original)
                temp_array[k++] = v[i++];
            else
                temp_array[k++] = v[j++];
        }
    }

    while (i <= meio) temp_array[k++] = v[i++];
    while (j <= fim) temp_array[k++] = v[j++];

    for (i = ini; i <= fim; i++) {
        v[i] = temp_array[i];
    }
}

void merge_sort(Resultado *v, int ini, int fim) {
    if (ini < fim) {
        int meio = (ini + fim) / 2;
        merge_sort(v, ini, meio);
        merge_sort(v, meio + 1, fim);
        mesclar(v, ini, meio, fim);
    }
}

int main(int argc, char *argv[]) {
    FILE *input = stdin; 
    FILE *output = stdout;

    if (argc >= 2) input = fopen(argv[1], "r");
    if (argc >= 3) output = fopen(argv[2], "w");

    if (!input) return 1;

    int num_doencas;

    if (fscanf(input, "%d", &k_global) != 1) return 0;
    if (fscanf(input, "%s", dna_paciente) != 1) return 0;
    if (fscanf(input, "%d", &num_doencas) != 1) return 0;

    int len_dna = strlen(dna_paciente);

    if (len_dna >= k_global) {
        for (int i = 0; i <= len_dna - k_global; i++) {
            hash_insert(i);
        }
    }

    int idx_resultados = 0;

    for (int d = 0; d < num_doencas; d++) {
        char nome_doenca[MAX_NOME];
        int qtd_genes;

        fscanf(input, "%s %d", nome_doenca, &qtd_genes);

        int genes_validos = 0;

        for (int g = 0; g < qtd_genes; g++) {
            char gene[MAX_GENE];
            fscanf(input, "%s", gene);

            int tamanho_gene = strlen(gene);
            
            if (tamanho_gene < k_global) continue;

            long bases_cobertas = 0;
            int ultima_pos_coberta = -1; 

            for (int cursor = 0; cursor <= tamanho_gene - k_global; cursor++) {
                if (hash_search(gene + cursor)) {
                    int inicio_atual = cursor;
                    int fim_atual = cursor + k_global; 


                    if (inicio_atual < ultima_pos_coberta) {
                        inicio_atual = ultima_pos_coberta;
                    }

                    if (fim_atual > inicio_atual) {
                        bases_cobertas += (fim_atual - inicio_atual);
                        ultima_pos_coberta = fim_atual;
                    }
                }
            }

            if (bases_cobertas * 100 >= 90 * tamanho_gene) {
                genes_validos++;
            }
        }

        if (idx_resultados < MAX_DOENCAS) {
            strcpy(resultados[idx_resultados].nome, nome_doenca);
            resultados[idx_resultados].prob = ((double)genes_validos / qtd_genes) * 100.0;
            resultados[idx_resultados].ordem_original = d;
            idx_resultados++;
        }
    }

    merge_sort(resultados, 0, idx_resultados - 1);

    for (int i = 0; i < idx_resultados; i++) {
        fprintf(output, "%s->%.0f%%\n",
                resultados[i].nome,
                resultados[i].prob);
    }

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return 0;
}