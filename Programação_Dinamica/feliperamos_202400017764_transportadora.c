#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PACOTES 10000
#define MAX_VEICULOS 5000
#define MAX_COD 32
#define MAX_ESTADOS 4000000 


#define SAFE_PESO 3000   
#define SAFE_VOL  15000  

typedef struct {
    char cod[MAX_COD];
    double valor;
    int peso;
    int volume;
} Pacote;

typedef struct {
    int peso;
    int volume;
    double valor;
    int pai_idx;
    int item_idx;
} EstadoNo;

typedef struct {
    int peso;
    int volume;
    double valor;
    int itens[MAX_PACOTES]; 
    int qtd_itens;
} ResultadoMochila;

EstadoNo g_estados[MAX_ESTADOS];
Pacote g_pacotes[MAX_PACOTES];
Pacote g_candidatos[MAX_PACOTES];
int g_idx_global[MAX_PACOTES];
int g_usados[MAX_PACOTES];

char g_placas[MAX_VEICULOS][MAX_COD];
int g_peso_max[MAX_VEICULOS];
int g_vol_max[MAX_VEICULOS];

int g_lookup[SAFE_PESO][SAFE_VOL];

void reset_lookup() {
    memset(g_lookup, -1, sizeof(g_lookup));
}

ResultadoMochila resolver_mochila(int cap_peso, int cap_vol, int n_cand) {
    
    int n_estados = 0;

    g_estados[0].peso = 0;
    g_estados[0].volume = 0;
    g_estados[0].valor = 0.0;
    g_estados[0].pai_idx = -1;
    g_estados[0].item_idx = -1;
    
    if (0 < SAFE_PESO && 0 < SAFE_VOL) g_lookup[0][0] = 0;
    
    n_estados++;

    for (int i = 0; i < n_cand; i++) {
        int p_peso = g_candidatos[i].peso;
        int p_vol  = g_candidatos[i].volume;
        double p_val = g_candidatos[i].valor;

        int limite_estados = n_estados;

        for (int e = 0; e < limite_estados; e++) {
            
            int novo_peso = g_estados[e].peso + p_peso;
            int novo_vol  = g_estados[e].volume + p_vol;

            if (novo_peso <= cap_peso && novo_vol <= cap_vol) {
                double novo_val = g_estados[e].valor + p_val;
                
                int existe_idx = -1;

                if (novo_peso < SAFE_PESO && novo_vol < SAFE_VOL) {
                    existe_idx = g_lookup[novo_peso][novo_vol];
                } else {
                    for (int k = n_estados - 1; k >= 0; k--) {
                        if (g_estados[k].peso == novo_peso && g_estados[k].volume == novo_vol) {
                            existe_idx = k;
                            break;
                        }
                    }
                }

                int criar_novo = 0;
                int atualizar_existente = 0;

                if (existe_idx == -1) {
                    criar_novo = 1;
                } else {
                    if (novo_val > g_estados[existe_idx].valor) {
                        if (existe_idx >= limite_estados) {
                            atualizar_existente = 1;
                        } else {
                            criar_novo = 1; 
                        }
                    }
                }

                if (criar_novo) {
                    if (n_estados < MAX_ESTADOS) {
                        g_estados[n_estados].peso = novo_peso;
                        g_estados[n_estados].volume = novo_vol;
                        g_estados[n_estados].valor = novo_val;
                        g_estados[n_estados].pai_idx = e;
                        g_estados[n_estados].item_idx = i;
                        
                        if (novo_peso < SAFE_PESO && novo_vol < SAFE_VOL) {
                            g_lookup[novo_peso][novo_vol] = n_estados;
                        }
                        n_estados++;
                    }
                } 
                else if (atualizar_existente) {
                    g_estados[existe_idx].valor = novo_val;
                    g_estados[existe_idx].pai_idx = e;
                    g_estados[existe_idx].item_idx = i;
                }
            }
        }
    }

    int idx_melhor = 0;
    double max_valor = -1.0;

    for (int i = 0; i < n_estados; i++) {
        if (g_estados[i].valor > max_valor) {
            max_valor = g_estados[i].valor;
            idx_melhor = i;
        }
    }

    ResultadoMochila res;
    res.valor = g_estados[idx_melhor].valor;
    res.peso = g_estados[idx_melhor].peso;
    res.volume = g_estados[idx_melhor].volume;
    res.qtd_itens = 0;

    int atual = idx_melhor;
    while (g_estados[atual].pai_idx != -1) {
        res.itens[res.qtd_itens++] = g_estados[atual].item_idx;
        atual = g_estados[atual].pai_idx;
    }
    
    return res;
}

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    FILE *out = stdout;

    if (argc >= 3) {
        in = fopen(argv[1], "r");
        out = fopen(argv[2], "w");
    } else {
        FILE *f_in = fopen("transportadora.input.txt", "r");
        if (f_in) {
            in = f_in;
            out = fopen("transportadora.output.txt", "w");
        }
    }

    if (!in || !out) {
        fprintf(stderr, "Erro ao abrir arquivos.\n");
        return 1;
    }

    int n_veiculos_lidos;
    if (fscanf(in, "%d", &n_veiculos_lidos) != 1) n_veiculos_lidos = 0;
    if (n_veiculos_lidos > MAX_VEICULOS) n_veiculos_lidos = MAX_VEICULOS;

    for (int i = 0; i < n_veiculos_lidos; i++) {
        fscanf(in, "%s %d %d", g_placas[i], &g_peso_max[i], &g_vol_max[i]);
    }

    int n_pacotes_lidos;
    if (fscanf(in, "%d", &n_pacotes_lidos) != 1) n_pacotes_lidos = 0;
    if (n_pacotes_lidos > MAX_PACOTES) n_pacotes_lidos = MAX_PACOTES;

    for (int i = 0; i < n_pacotes_lidos; i++) {
        fscanf(in, "%s %lf %d %d",
               g_pacotes[i].cod,
               &g_pacotes[i].valor,
               &g_pacotes[i].peso,
               &g_pacotes[i].volume);
    }

    for(int k=0; k<MAX_PACOTES; k++) g_usados[k] = 0;

    for (int v = 0; v < n_veiculos_lidos; v++) {
        
        int n_cand = 0;
        for (int i = 0; i < n_pacotes_lidos; i++) {
            if (!g_usados[i]) {
                g_candidatos[n_cand] = g_pacotes[i];
                g_idx_global[n_cand] = i;
                n_cand++;
            }
        }

        reset_lookup(); 

        ResultadoMochila sol = resolver_mochila(g_peso_max[v], g_vol_max[v], n_cand);

        char codigos[MAX_PACOTES][MAX_COD];
        int qtd_cod = 0;

        for (int i = 0; i < sol.qtd_itens; i++) {
            int idx_local = sol.itens[i];
            int idx_real = g_idx_global[idx_local];
            
            g_usados[idx_real] = 1;
            strcpy(codigos[qtd_cod++], g_pacotes[idx_real].cod);
        }

        int perc_p = g_peso_max[v] ? (int)((double)sol.peso / g_peso_max[v] * 100 + 0.5) : 0;
        int perc_v = g_vol_max[v]  ? (int)((double)sol.volume / g_vol_max[v] * 100 + 0.5) : 0;

        fprintf(out, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->",
                g_placas[v], sol.valor, sol.peso, perc_p, sol.volume, perc_v);

        for (int i = qtd_cod - 1; i >= 0; i--) {
            fprintf(out, "%s%s", codigos[i], (i > 0) ? "," : "");
        }
        fprintf(out, "\n");
    }

    double p_val = 0.0;
    int p_peso = 0, p_vol = 0;
    char pendentes[MAX_PACOTES][MAX_COD];
    int qtd_pend = 0;

    for (int i = 0; i < n_pacotes_lidos; i++) {
        if (!g_usados[i]) {
            p_val += g_pacotes[i].valor;
            p_peso += g_pacotes[i].peso;
            p_vol += g_pacotes[i].volume;
            strcpy(pendentes[qtd_pend++], g_pacotes[i].cod);
        }
    }

    fprintf(out, "PENDENTE:R$%.2f,%dKG,%dL->", p_val, p_peso, p_vol);
    for (int i = 0; i < qtd_pend; i++) {
        fprintf(out, "%s%s", pendentes[i], (i + 1 < qtd_pend) ? "," : "");
    }
    fprintf(out, "\n");

    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    
    return 0;
}