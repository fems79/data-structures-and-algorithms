import sys
def encontrar_impressora_com_menor_fila(impressoras_dicionario):
    pos_menor_fila = 0
    if not impressoras_dicionario:
        return -1 
    paginas_menor_fila = impressoras_dicionario[0]["paginas"]
    
    for i in range(1, len(impressoras_dicionario)):
        if impressoras_dicionario[i]["paginas"] < paginas_menor_fila:
            paginas_menor_fila = impressoras_dicionario[i]["paginas"]
            pos_menor_fila = i
    return pos_menor_fila

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")
    num_impressoras = int(input_file.readline().strip())
    impressoras = {}
    for i in range(num_impressoras):
        impressora_nome = input_file.readline().strip()
        impressoras[i] = {"documentos": [], "impressora": impressora_nome, "numero": i, "paginas": 0}

    num_documentos = int(input_file.readline().strip())
    documentos = {}
    for i in range(num_documentos):
        documento, paginas = input_file.readline().strip().split()
        paginas = int(paginas)
        documentos[i] = {"documento": documento, "paginas": paginas}

    somatorio_total_paginas = 0
    documentos_para_pilha_final = []
    for i in range(num_documentos):
        impressora_alvo_idx = encontrar_impressora_com_menor_fila(impressoras)
        doc_atual = documentos[i]
        impressora_alvo = impressoras[impressora_alvo_idx]
        impressora_alvo["documentos"].insert(0, doc_atual)
        impressora_alvo["paginas"] += doc_atual["paginas"]
        somatorio_total_paginas += doc_atual["paginas"]
        documentos_para_pilha_final.append(doc_atual)
        historico_formatado = []
        
        for doc in impressora_alvo["documentos"]:
            historico_formatado.append(f"{doc['documento']}-{doc['paginas']}p")
        
        output_file.write(f"{impressora_alvo['impressora']}:{','.join(historico_formatado)}\n")

    output_file.write(f"{somatorio_total_paginas}p\n")
    for doc in reversed(documentos_para_pilha_final):
        output_file.write(f"{doc['documento']}-{doc['paginas']}p\n")

    input_file.close()
    output_file.close()
if __name__ == '__main__':
    main(sys.argv)