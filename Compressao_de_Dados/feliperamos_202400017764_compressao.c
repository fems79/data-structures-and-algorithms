#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DATA   200000
#define MAX_HEAP   1024
#define MAX_CODES  512
#define MAX_BITS   2000000

typedef struct Node {
    int freq;
    int min_symbol; // O menor simbolo contido nesta subarvore (criterio de desempate)
    struct Node *left;
    struct Node *right;
    unsigned char symbol;
    int is_leaf;
} Node;

Node* heap[MAX_HEAP];
int heap_size = 0;

// Criterio de desempate robusto: Frequencia ASC -> Menor Simbolo da Subarvore ASC
int node_less(Node *a, Node *b) {
    if (a->freq != b->freq) 
        return a->freq < b->freq;
    
    // Se frequencias iguais, prioriza o no que contem o menor simbolo ASCII
    // Isso garante deterministicidade independente da ordem de criacao ou altura
    return a->min_symbol < b->min_symbol;
}

void heap_push(Node *n) {
    int i = heap_size++;
    heap[i] = n;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (node_less(heap[i], heap[p])) {
            Node *tmp = heap[i];
            heap[i] = heap[p];
            heap[p] = tmp;
            i = p;
        } else break;
    }
}

Node* heap_pop() {
    if (heap_size == 0) return NULL;
    Node *root = heap[0];
    heap[0] = heap[--heap_size];

    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, s = i;
        if (l < heap_size && node_less(heap[l], heap[s])) s = l;
        if (r < heap_size && node_less(heap[r], heap[s])) s = r;
        if (s != i) {
            Node *tmp = heap[i];
            heap[i] = heap[s];
            heap[s] = tmp;
            i = s;
        } else break;
    }
    return root;
}

int run_length_encoding(unsigned char *data, int n, char *out) {
    if (n == 0) return 0;

    int cnt = 1, pos = 0;
    unsigned char cur = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] == cur && cnt < 255) {
            cnt++;
        } else {
            sprintf(out + pos, "%02X%02X", cnt, cur);
            pos += 4;
            cur = data[i];
            cnt = 1;
        }
    }
    sprintf(out + pos, "%02X%02X", cnt, cur);
    pos += 4;

    return pos / 2; 
}

char codes[MAX_CODES][MAX_BITS];

void build_codes(Node *node, char *bits) {
    if (node->is_leaf) {
        strcpy(codes[node->symbol], bits);
        return;
    }

    char lbits[512], rbits[512];

    // Esquerda = 0, Direita = 1
    if (node->left) {
        sprintf(lbits, "%s0", bits);
        build_codes(node->left, lbits);
    }
    if (node->right) {
        sprintf(rbits, "%s1", bits);
        build_codes(node->right, rbits);
    }
}

int huffman_encoding(unsigned char *data, int n, char *out) {
    int freq[256] = {0};

    for (int i = 0; i < n; i++)
        freq[data[i]]++;

    int unique = 0;
    for (int i = 0; i < 256; i++)
        if (freq[i]) unique++;

    if (unique == 0) {
        out[0] = '\0';
        return 0;
    }

    if (unique == 1) {
        int bits = n;
        int pad = (8 - bits % 8) % 8;
        bits += pad;
        int bytes = bits / 8;
        for (int i = 0; i < bytes; i++)
            sprintf(out + i*2, "%02X", 0);
        return bytes;
    }

    heap_size = 0;
    static Node nodes[MAX_HEAP];
    int node_count = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i]) {
            // Inicializa folhas
            nodes[node_count].freq = freq[i];
            nodes[node_count].min_symbol = i; // Para folhas, min_symbol e o proprio valor
            nodes[node_count].left = NULL;
            nodes[node_count].right = NULL;
            nodes[node_count].symbol = (unsigned char)i;
            nodes[node_count].is_leaf = 1;
            heap_push(&nodes[node_count++]);
        }
    }

    while (heap_size > 1) {
        Node *a = heap_pop(); // Menor (vai para a esquerda 0)
        Node *b = heap_pop(); // Segundo menor (vai para a direita 1)
        
        nodes[node_count].freq = a->freq + b->freq;
        // O pai herda o min_symbol do filho "menor" (que ja foi ordenado pelo heap)
        // Como 'a' saiu antes de 'b', a->min_symbol <= b->min_symbol
        nodes[node_count].min_symbol = (a->min_symbol < b->min_symbol) ? a->min_symbol : b->min_symbol;
        
        nodes[node_count].left = a;  
        nodes[node_count].right = b; 
        nodes[node_count].symbol = 0;
        nodes[node_count].is_leaf = 0;
        
        heap_push(&nodes[node_count++]);
    }

    for (int i = 0; i < 256; i++)
        codes[i][0] = '\0';

    char empty[1] = "";
    build_codes(heap[0], empty);

    static char bits[MAX_BITS];
    
    int current_len = 0;
    for (int i = 0; i < n; i++) {
        char *c = codes[data[i]];
        int len = strlen(c);
        strcpy(bits + current_len, c); 
        current_len += len;
    }
    bits[current_len] = '\0';

    int pad = (8 - current_len % 8) % 8;
    for (int i = 0; i < pad; i++)
        bits[current_len++] = '0';
    bits[current_len] = '\0';

    int bytes = current_len / 8;
    for (int i = 0; i < bytes; i++) {
        int val = 0;
        for (int j = 0; j < 8; j++)
            val = (val << 1) | (bits[i*8 + j] - '0');
        sprintf(out + i*2, "%02X", val);
    }

    return bytes;
}

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    FILE *out = stdout;
    
    if (argc >= 3) {
        in = fopen(argv[1], "r");
        out = fopen(argv[2], "w");
    } else {
        FILE *f_test = fopen("compressao.input.txt", "r");
        if (f_test) {
            in = f_test;
            out = fopen("compressao.output.txt", "w");
        }
    }

    if (!in || !out) return 1;

    int n;
    if (fscanf(in, "%d", &n) != 1) return 0;

    for (int i = 0; i < n; i++) {
        int size;
        fscanf(in, "%d", &size);

        static unsigned char data[MAX_DATA];
        
        for (int j = 0; j < size; j++) {
            char tmp[4];
            unsigned int temp_val; 
            fscanf(in, "%s", tmp);
            sscanf(tmp, "%X", &temp_val); 
            data[j] = (unsigned char)temp_val; 
        }

        static char rle_out[MAX_BITS];
        static char huf_out[MAX_BITS];

        int rle_sz = run_length_encoding(data, size, rle_out);
        int huf_sz = huffman_encoding(data, size, huf_out);

        if (huf_sz <= rle_sz)
            fprintf(out, "%d->HUF(%.2f%%)=%s\n",
                    i, (huf_sz * 100.0) / size, huf_out);
        else 
            fprintf(out, "%d->RLE(%.2f%%)=%s\n",
                    i, (rle_sz * 100.0) / size, rle_out);
    }

    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    return 0;
}