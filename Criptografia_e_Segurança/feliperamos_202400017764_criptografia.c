#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BN_ARRAY_SIZE 128 

typedef struct {
    uint32_t data[BN_ARRAY_SIZE];
    int len;
} BigInt;

uint8_t hex_char_to_bin(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

int hex_str_to_bytes(const char *hex, uint8_t **out) {
    size_t len = strlen(hex);
    size_t bin_len = (len + 1) / 2;
    *out = (uint8_t*)calloc(bin_len, 1);
    
    for (size_t i = 0; i < len; i++) {
        uint8_t val = hex_char_to_bin(hex[len - 1 - i]);
        if (i % 2 == 0) (*out)[bin_len - 1 - (i/2)] |= val;
        else (*out)[bin_len - 1 - (i/2)] |= (val << 4);
    }
    return bin_len;
}

const uint8_t sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

const uint8_t Rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

#define Nb 4
#define Nk 4
#define Nr 10

typedef struct {
    uint32_t round_key[44];
} AES_CTX;

uint32_t rot_word(uint32_t w) {

    return (w << 8) | (w >> 24);
}

uint32_t sub_word(uint32_t w) {
    return (sbox[(w >> 24) & 0xFF] << 24) |
           (sbox[(w >> 16) & 0xFF] << 16) |
           (sbox[(w >> 8) & 0xFF] << 8) |
           (sbox[w & 0xFF]);
}

void key_expansion(const uint8_t *key, AES_CTX *ctx) {
    for (int i = 0; i < Nk; i++) {
        ctx->round_key[i] = (key[4*i] << 24) | (key[4*i+1] << 16) | (key[4*i+2] << 8) | key[4*i+3];
    }
    for (int i = Nk; i < Nb * (Nr + 1); i++) {
        uint32_t temp = ctx->round_key[i-1];
        if (i % Nk == 0) {
            temp = sub_word(rot_word(temp)) ^ (Rcon[i/Nk] << 24);
        }
        ctx->round_key[i] = ctx->round_key[i-Nk] ^ temp;
    }
}

void add_round_key(uint8_t *state, const uint32_t *round_key) {
    for (int i = 0; i < 4; i++) {
        uint32_t k = round_key[i];
        state[4*i] ^= (k >> 24) & 0xFF;
        state[4*i+1] ^= (k >> 16) & 0xFF;
        state[4*i+2] ^= (k >> 8) & 0xFF;
        state[4*i+3] ^= k & 0xFF;
    }
}

void sub_bytes(uint8_t *state) {
    for (int i = 0; i < 16; i++) state[i] = sbox[state[i]];
}

void inv_sub_bytes(uint8_t *state) {
    for (int i = 0; i < 16; i++) state[i] = rsbox[state[i]];
}

void shift_rows(uint8_t *state) {
    uint8_t tmp;
    tmp = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = tmp;
    tmp = state[2]; state[2] = state[10]; state[10] = tmp;
    tmp = state[6]; state[6] = state[14]; state[14] = tmp;
    tmp = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = state[3]; state[3] = tmp;
}

void inv_shift_rows(uint8_t *state) {
    uint8_t tmp;
    tmp = state[13]; state[13] = state[9]; state[9] = state[5]; state[5] = state[1]; state[1] = tmp;
    tmp = state[2]; state[2] = state[10]; state[10] = tmp;
    tmp = state[6]; state[6] = state[14]; state[14] = tmp;
    tmp = state[3]; state[3] = state[7]; state[7] = state[11]; state[11] = state[15]; state[15] = tmp;
}

uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        uint8_t hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

void mix_columns(uint8_t *state) {
    for (int i = 0; i < 4; i++) {
        uint8_t *c = state + 4*i;
        uint8_t a[4] = {c[0], c[1], c[2], c[3]};
        c[0] = gmul(a[0],2) ^ gmul(a[1],3) ^ a[2] ^ a[3];
        c[1] = a[0] ^ gmul(a[1],2) ^ gmul(a[2],3) ^ a[3];
        c[2] = a[0] ^ a[1] ^ gmul(a[2],2) ^ gmul(a[3],3);
        c[3] = gmul(a[0],3) ^ a[1] ^ a[2] ^ gmul(a[3],2);
    }
}

void inv_mix_columns(uint8_t *state) {
    for (int i = 0; i < 4; i++) {
        uint8_t *c = state + 4*i;
        uint8_t a[4] = {c[0], c[1], c[2], c[3]};
        c[0] = gmul(a[0],0x0e) ^ gmul(a[1],0x0b) ^ gmul(a[2],0x0d) ^ gmul(a[3],0x09);
        c[1] = gmul(a[0],0x09) ^ gmul(a[1],0x0e) ^ gmul(a[2],0x0b) ^ gmul(a[3],0x0d);
        c[2] = gmul(a[0],0x0d) ^ gmul(a[1],0x09) ^ gmul(a[2],0x0e) ^ gmul(a[3],0x0b);
        c[3] = gmul(a[0],0x0b) ^ gmul(a[1],0x0d) ^ gmul(a[2],0x09) ^ gmul(a[3],0x0e);
    }
}

void aes_encrypt_block(const uint8_t *in, uint8_t *out, AES_CTX *ctx) {
    memcpy(out, in, 16);
    add_round_key(out, ctx->round_key);
    for (int round = 1; round < Nr; round++) {
        sub_bytes(out);
        shift_rows(out);
        mix_columns(out);
        add_round_key(out, ctx->round_key + round*4);
    }
    sub_bytes(out);
    shift_rows(out);
    add_round_key(out, ctx->round_key + Nr*4);
}

void aes_decrypt_block(const uint8_t *in, uint8_t *out, AES_CTX *ctx) {
    memcpy(out, in, 16);
    add_round_key(out, ctx->round_key + Nr*4);
    for (int round = Nr - 1; round > 0; round--) {
        inv_shift_rows(out);
        inv_sub_bytes(out);
        add_round_key(out, ctx->round_key + round*4);
        inv_mix_columns(out);
    }
    inv_shift_rows(out);
    inv_sub_bytes(out);
    add_round_key(out, ctx->round_key);
}

void bn_from_hex(BigInt *bn, const char *hex) {
    memset(bn->data, 0, sizeof(bn->data));
    bn->len = 0;
    
    int hex_len = strlen(hex);
    for (int i = 0; i < hex_len; i++) {
        uint32_t val = hex_char_to_bin(hex[i]);
        uint64_t carry = val;
        for (int j = 0; j < bn->len || carry > 0; j++) {
            if (j == BN_ARRAY_SIZE) break;
            uint64_t tmp = ((uint64_t)bn->data[j] << 4) + carry;
            bn->data[j] = (uint32_t)(tmp & 0xFFFFFFFF);
            carry = tmp >> 32;
            if (j >= bn->len) bn->len = j + 1;
        }
    }
    if (bn->len == 0) bn->len = 1;
}

void bn_to_hex_str(BigInt *bn, char *str) {
    if (bn->len == 0) { strcpy(str, "0"); return; }
    char *ptr = str;
    for (int i = bn->len - 1; i >= 0; i--) {
        sprintf(ptr, "%08X", bn->data[i]);
        ptr += 8;
    }
    *ptr = 0;
    char *start = str;
    while (*start == '0' && *(start+1) != 0) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
}

void bn_mul_mod(BigInt *res, BigInt *a, BigInt *b, BigInt *mod) {
    BigInt temp_a = *a;
    memset(res->data, 0, sizeof(res->data));
    res->len = 1;

    for (int i = 0; i < b->len; i++) {
        for (int bit = 0; bit < 32; bit++) {
            if ((b->data[i] >> bit) & 1) {
                uint64_t carry = 0;
                for (int j = 0; j < res->len || j < temp_a.len || carry; j++) {
                    uint64_t sum = carry + (j < res->len ? res->data[j] : 0) + (j < temp_a.len ? temp_a.data[j] : 0);
                    res->data[j] = sum & 0xFFFFFFFF;
                    carry = sum >> 32;
                    if (j >= res->len) res->len = j + 1;
                }
                while (1) {
                     int cmp = 0;
                     if (res->len > mod->len) cmp = 1;
                     else if (res->len < mod->len) cmp = -1;
                     else {
                         for (int k = res->len - 1; k >= 0; k--) {
                             if (res->data[k] > mod->data[k]) { cmp = 1; break; }
                             if (res->data[k] < mod->data[k]) { cmp = -1; break; }
                         }
                     }
                     if (cmp < 0) break; 
                     uint64_t borrow = 0;
                     for (int j = 0; j < res->len; j++) {
                         uint64_t sub = res->data[j] - borrow - (j < mod->len ? mod->data[j] : 0);
                         res->data[j] = sub & 0xFFFFFFFF;
                         borrow = (sub >> 32) & 1;
                     }
                     while (res->len > 1 && res->data[res->len - 1] == 0) res->len--;
                }
            }
            uint64_t carry = 0;
            for (int j = 0; j < temp_a.len || carry; j++) {
                uint64_t val = ((uint64_t)temp_a.data[j] << 1) + carry;
                temp_a.data[j] = val & 0xFFFFFFFF;
                carry = val >> 32;
                if (j >= temp_a.len) temp_a.len = j + 1;
            }
            while (1) {
                 int cmp = 0;
                 if (temp_a.len > mod->len) cmp = 1;
                 else if (temp_a.len < mod->len) cmp = -1;
                 else {
                     for (int k = temp_a.len - 1; k >= 0; k--) {
                         if (temp_a.data[k] > mod->data[k]) { cmp = 1; break; }
                         if (temp_a.data[k] < mod->data[k]) { cmp = -1; break; }
                     }
                 }
                 if (cmp < 0) break;
                 uint64_t borrow = 0;
                 for (int j = 0; j < temp_a.len; j++) {
                     uint64_t sub = temp_a.data[j] - borrow - (j < mod->len ? mod->data[j] : 0);
                     temp_a.data[j] = sub & 0xFFFFFFFF;
                     borrow = (sub >> 32) & 1;
                 }
                 while (temp_a.len > 1 && temp_a.data[temp_a.len - 1] == 0) temp_a.len--;
            }
        }
    }
}

void bn_pow_mod(BigInt *res, BigInt *base, BigInt *exp, BigInt *mod) {
    BigInt b = *base;
    BigInt e = *exp;
    memset(res->data, 0, sizeof(res->data));
    res->data[0] = 1; res->len = 1;
    for (int i = 0; i < e.len; i++) {
        for (int bit = 0; bit < 32; bit++) {
            if ((e.data[i] >> bit) & 1) {
                BigInt tmp = *res;
                bn_mul_mod(res, &tmp, &b, mod);
            }
            if (i == e.len - 1 && (e.data[i] >> (bit+1)) == 0) break; 
            BigInt tmp_b = b;
            bn_mul_mod(&b, &tmp_b, &tmp_b, mod);
        }
    }
}

char* alinhamento(const char *entrada, int *out_len) {
    int len_hex = strlen(entrada);
    int target_len_hex = ((len_hex >> 5) + 1) << 5;
    char *nova = (char*)malloc(target_len_hex + 1);
    memset(nova, '0', target_len_hex);
    nova[target_len_hex] = 0;
    memcpy(nova, entrada, len_hex);
    *out_len = target_len_hex;
    return nova;
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
    
    char line[8192];
    if (fgets(line, sizeof(line), input)) {
        int n = atoi(line);

        char shared_secret_hex[2048] = {0}; 

        for (int i = 0; i < n; i++) {
            if (!fgets(line, sizeof(line), input)) break;
            line[strcspn(line, "\r\n")] = 0;
            
            char *cmd = strtok(line, " ");
            if (!cmd) continue;

            if (strcmp(cmd, "dh") == 0) {
                char *p_a = strtok(NULL, " ");
                char *p_b = strtok(NULL, " ");
                char *p_g = strtok(NULL, " ");
                char *p_p = strtok(NULL, " ");

                BigInt a, b, g, p, s_tmp, s_final;
                bn_from_hex(&a, p_a);
                bn_from_hex(&b, p_b);
                bn_from_hex(&g, p_g);
                bn_from_hex(&p, p_p);

                bn_pow_mod(&s_tmp, &g, &a, &p);
                bn_pow_mod(&s_final, &s_tmp, &b, &p);
                
                char temp_hex[2048];
                bn_to_hex_str(&s_final, temp_hex);
                
                int len_s = strlen(temp_hex);
                int len_a = strlen(p_a);
                int start_idx = len_s - len_a;
                if (start_idx < 0) start_idx = 0;

                strcpy(shared_secret_hex, temp_hex + start_idx);
                fprintf(output, "s=%s\n", shared_secret_hex);
            }
            else if (strcmp(cmd, "e") == 0 || strcmp(cmd, "d") == 0) {
                char *val_str = strtok(NULL, " ");
                int padded_len_hex;
                char *padded_hex = alinhamento(val_str, &padded_len_hex);
                
                uint8_t *data;
                int data_len = hex_str_to_bytes(padded_hex, &data);
                
                uint8_t *key_bytes;
                hex_str_to_bytes(shared_secret_hex, &key_bytes);
                
                AES_CTX ctx;
                key_expansion(key_bytes, &ctx);

                fprintf(output, "%s=", strcmp(cmd, "e") == 0 ? "c" : "m");
                
                uint8_t block[16];
                uint8_t out_block[16];
                
                for (int j = 0; j < data_len; j += 16) {
                    memcpy(block, data + j, 16);
                    if (strcmp(cmd, "e") == 0) aes_encrypt_block(block, out_block, &ctx);
                    else aes_decrypt_block(block, out_block, &ctx);
                    
                    for(int k=0; k<16; k++) fprintf(output, "%02X", out_block[k]);
                }
                fprintf(output, "\n");

                free(padded_hex);
                free(data);
                free(key_bytes);
            }
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}