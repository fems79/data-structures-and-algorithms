import sys

def criar_no_trie():
    return {'filhos': {}, 'fim_de_palavra': False, 'palavra': None}

def inserir(raiz, palavra):
    no = raiz
    for caractere in palavra:
        if caractere not in no['filhos']:
            no['filhos'][caractere] = criar_no_trie()
        no = no['filhos'][caractere]
    no['fim_de_palavra'] = True
    no['palavra'] = palavra

def _coletar_palavras_dfs(no, lista_de_palavras):
    if no['fim_de_palavra']:
        lista_de_palavras.append(no['palavra'])
    
    for no_filho in no['filhos'].values():
        _coletar_palavras_dfs(no_filho, lista_de_palavras)

def buscar(raiz, requisicao):
    palavras_candidatas = []
    no = raiz
    tamanho_prefixo_comum = 0

    for caractere in requisicao:
        if caractere in no['filhos']:
            no = no['filhos'][caractere]  
            tamanho_prefixo_comum += 1
            if no['fim_de_palavra']:
                palavras_candidatas.append(no['palavra'])
        else:
            break

    palavras_subarvore = []
    _coletar_palavras_dfs(no, palavras_subarvore)
    for palavra in palavras_subarvore:
        if palavra not in palavras_candidatas:
            palavras_candidatas.append(palavra)

    sugestoes = set()
    for termo in palavras_candidatas:
        condicao_prefixo_req = tamanho_prefixo_comum >= len(requisicao) / 2
        condicao_prefixo_termo = tamanho_prefixo_comum >= len(termo) / 2
        condicao_tamanho_maximo = len(termo) <= 2 * len(requisicao)

        if condicao_prefixo_req and condicao_prefixo_termo and condicao_tamanho_maximo:
            sugestoes.add(termo)
    
    return sorted(list(sugestoes))

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")

    raiz = criar_no_trie()

    qntd_termos = int(input_file.readline().strip())
    for _ in range(qntd_termos):
        termo = input_file.readline().strip()
        inserir(raiz, termo)

    num_requisicoes = int(input_file.readline().strip())
    lista_requisicoes = []
    for _ in range(num_requisicoes):
        lista_requisicoes.append(input_file.readline().strip())

    resultados = []
    for requisicao in lista_requisicoes:
        sugestoes = buscar(raiz, requisicao)
        
        if not sugestoes:
            resultados.append(f"{requisicao}:-")
        else:
            sugestoes_formatadas = ",".join(sugestoes)
            resultados.append(f"{requisicao}:{sugestoes_formatadas}")

    for linha_de_saida in resultados:
        output_file.write(linha_de_saida + "\n")

    input_file.close()
    output_file.close()

if __name__ == "__main__":
    main(sys.argv)