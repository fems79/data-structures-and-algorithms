import sys

def criar_no_arvore_b_mais(folha=False):
    return {"chaves": [], "filhos": [], "folha": folha, "proximo": None}

def dividir_no_b_mais(no_pai, indice_filho, ordem_arvore):
    filho_a_dividir = no_pai["filhos"][indice_filho]
    novo_filho = criar_no_arvore_b_mais(folha=filho_a_dividir["folha"])
    if filho_a_dividir["folha"]:
        ponto_divisao = ordem_arvore // 2
        novo_filho["chaves"] = filho_a_dividir["chaves"][ponto_divisao:]
        filho_a_dividir["chaves"] = filho_a_dividir["chaves"][:ponto_divisao]
        chave_promovida = novo_filho["chaves"][0]
        novo_filho["proximo"] = filho_a_dividir["proximo"]
        filho_a_dividir["proximo"] = novo_filho
    else: 
        ponto_divisao = (ordem_arvore - 1) // 2
        chave_promovida = filho_a_dividir["chaves"][ponto_divisao]
        novo_filho["chaves"] = filho_a_dividir["chaves"][ponto_divisao + 1:]
        filho_a_dividir["chaves"] = filho_a_dividir["chaves"][:ponto_divisao]
        novo_filho["filhos"] = filho_a_dividir["filhos"][ponto_divisao + 1:]
        filho_a_dividir["filhos"] = filho_a_dividir["filhos"][:ponto_divisao + 1]

    no_pai["chaves"].insert(indice_filho, chave_promovida)
    no_pai["filhos"].insert(indice_filho + 1, novo_filho)

def inserir_em_no_nao_cheio(no, chave, ordem_arvore):
    if no["folha"]:
        indice = 0
        while indice < len(no["chaves"]) and chave > no["chaves"][indice]:
            indice += 1
        no["chaves"].insert(indice, chave)
    else:
        indice = 0
        while indice < len(no["chaves"]) and chave >= no["chaves"][indice]:
            indice += 1

        if len(no["filhos"][indice]["chaves"]) == ordem_arvore:
            dividir_no_b_mais(no, indice, ordem_arvore)
            if chave >= no["chaves"][indice]:
                indice += 1
        
        inserir_em_no_nao_cheio(no["filhos"][indice], chave, ordem_arvore)

def inserir(arvore, chave):
    raiz = arvore["raiz"]
    ordem_arvore = arvore["ordem"]
    if len(raiz["chaves"]) == ordem_arvore:
        nova_raiz = criar_no_arvore_b_mais(folha=False)
        arvore["raiz"] = nova_raiz
        nova_raiz["filhos"].append(raiz)
        dividir_no_b_mais(nova_raiz, 0, ordem_arvore)
        inserir_em_no_nao_cheio(nova_raiz, chave, ordem_arvore)
    else:
        inserir_em_no_nao_cheio(raiz, chave, ordem_arvore)

def executar_select(arvore, chave_buscada, mapa_dados_arquivos, output_file):
    no_atual = arvore["raiz"]
    while not no_atual["folha"]:
        indice = 0
        while indice < len(no_atual["chaves"]) and chave_buscada >= no_atual["chaves"][indice]:
            indice += 1
        no_atual = no_atual["filhos"][indice]

    encontrado = chave_buscada in no_atual["chaves"]
    output_file.write(f"[{chave_buscada}]\n")
    if encontrado:
        for chave_no_no in no_atual["chaves"]:
            if chave_no_no in mapa_dados_arquivos:
                info_arquivo = mapa_dados_arquivos[chave_no_no]
                output_file.write(f"{info_arquivo['nome']}:size={info_arquivo['tamanho']},hash={chave_no_no}\n")
    else:
        output_file.write("-\n")

def executar_select_range(arvore, chave_inicio, chave_fim, mapa_dados_arquivos, output_file):
    if chave_inicio > chave_fim:
        chave_inicio, chave_fim = chave_fim, chave_inicio

    output_file.write(f"[{chave_inicio},{chave_fim}]\n")
    no_atual = arvore["raiz"]
    while not no_atual["folha"]:
        i = 0
        while i < len(no_atual["chaves"]) and chave_inicio >= no_atual["chaves"][i]:
            i += 1
        no_atual = no_atual["filhos"][i]

    resultados_encontrados = False
    while no_atual:
        folha_chaves = no_atual["chaves"]
        imprimir_folha = False
        for chave in folha_chaves:
            if chave_inicio <= chave:
                imprimir_folha = True
                break
        
        if not imprimir_folha:
            if no_atual["proximo"] and no_atual["proximo"]["chaves"]:
                if chave_inicio <= no_atual["proximo"]["chaves"][0]:
                     imprimir_folha = True

        if imprimir_folha:
            for chave in folha_chaves:
                if chave in mapa_dados_arquivos:
                    info = mapa_dados_arquivos[chave]
                    output_file.write(f"{info['nome']}:size={info['tamanho']},hash={chave}\n")
                    resultados_encontrados = True

        if folha_chaves and folha_chaves[-1] >= chave_fim:
            break
        
        no_atual = no_atual["proximo"]

    if not resultados_encontrados:
        output_file.write("-\n")

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)
    
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    with open(input_filename, "r") as input_file, open(output_filename, "w") as output_file:
        ordem_entrada = int(input_file.readline().strip())
        num_arquivos = int(input_file.readline().strip())
        
        linhas_arquivos_iniciais = [input_file.readline().strip() for _ in range(num_arquivos)]

        num_operacoes = int(input_file.readline().strip())
        linhas_operacoes = [input_file.readline().strip() for _ in range(num_operacoes)]

        dic_arquivos = {}
        arvore_b_mais = {
            "raiz": criar_no_arvore_b_mais(folha=True),
            "ordem": ordem_entrada
        }

        for linha_arquivo in linhas_arquivos_iniciais:
            nome, tamanho, valor_hash = linha_arquivo.split(" ")
            dic_arquivos[valor_hash] = {"tamanho": tamanho, "nome": nome}
            inserir(arvore_b_mais, valor_hash)
        
        for linha_operacao_str in linhas_operacoes:
            partes_operacao = linha_operacao_str.split(" ")
            comando = partes_operacao[0]
            
            if comando == "INSERT":
                nome, tamanho, valor_hash = partes_operacao[1], partes_operacao[2], partes_operacao[3]
                dic_arquivos[valor_hash] = {"tamanho": tamanho, "nome": nome}
                inserir(arvore_b_mais, valor_hash)
            elif comando == "SELECT":
                if len(partes_operacao) > 2 and partes_operacao[1] == "RANGE":
                    hash1 = partes_operacao[2]
                    hash2 = partes_operacao[3]
                    executar_select_range(arvore_b_mais, hash1, hash2, dic_arquivos, output_file)
                else:
                    valor_hash = partes_operacao[1]
                    executar_select(arvore_b_mais, valor_hash, dic_arquivos, output_file)

if __name__ == '__main__':
    main(sys.argv)