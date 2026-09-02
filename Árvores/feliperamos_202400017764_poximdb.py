import sys
import math

def criar_no_arvore_b(folha=False):
    return {"chaves": [], "filhos": [], "folha": folha}

def executar_select(arvore, chave_buscada, mapa_dados_arquivos, output_file):
    no_atual = arvore["raiz"]
    encontrado = False
    while True:
        indice = 0
        while indice < len(no_atual["chaves"]) and chave_buscada > no_atual["chaves"][indice]:
            indice += 1
        if indice < len(no_atual["chaves"]) and chave_buscada == no_atual["chaves"][indice]:
            encontrado = True
            break 

        if no_atual["folha"]:
            encontrado = False
            break 

        no_atual = no_atual["filhos"][indice]

    output_file.write(f"[{chave_buscada}]\n")
    if encontrado:
        for chave_no_no in no_atual["chaves"]:
            if chave_no_no in mapa_dados_arquivos:
                info_arquivo = mapa_dados_arquivos[chave_no_no]
                output_file.write(f"{info_arquivo['nome']}:size={info_arquivo['tamanho']},hash={chave_no_no}\n")
    else:
        output_file.write("-\n")

def dividir_filho(no_pai, indice_filho, ordem_arvore):
    filho_a_dividir = no_pai["filhos"][indice_filho]
    novo_filho = criar_no_arvore_b(folha=filho_a_dividir["folha"])
    indice_mediano = math.floor((ordem_arvore - 1) / 2)
    no_pai["chaves"].insert(indice_filho, filho_a_dividir["chaves"][indice_mediano])
    no_pai["filhos"].insert(indice_filho + 1, novo_filho)
    novo_filho["chaves"] = filho_a_dividir["chaves"][indice_mediano + 1:]
    filho_a_dividir["chaves"] = filho_a_dividir["chaves"][0:indice_mediano]
    if not filho_a_dividir["folha"]:
        novo_filho["filhos"] = filho_a_dividir["filhos"][indice_mediano + 1:]
        filho_a_dividir["filhos"] = filho_a_dividir["filhos"][0:indice_mediano + 1]

def inserir_em_no_nao_cheio(no, chave, ordem_arvore):
    indice = len(no["chaves"]) - 1
    if no["folha"]:
        no["chaves"].append(None)
        while indice >= 0 and chave < no["chaves"][indice]:
            no["chaves"][indice + 1] = no["chaves"][indice]
            indice -= 1
        no["chaves"][indice + 1] = chave
    else:
        while indice >= 0 and chave < no["chaves"][indice]:
            indice -= 1
        indice += 1
        if len(no["filhos"][indice]["chaves"]) == ordem_arvore - 1:
            dividir_filho(no, indice, ordem_arvore)
            if chave > no["chaves"][indice]:
                indice += 1
        inserir_em_no_nao_cheio(no["filhos"][indice], chave, ordem_arvore)

def inserir(arvore, chave):
    raiz = arvore["raiz"]
    ordem_arvore = arvore["ordem"]
    if len(raiz["chaves"]) == ordem_arvore - 1:
        nova_raiz = criar_no_arvore_b()
        arvore["raiz"] = nova_raiz
        nova_raiz["filhos"].insert(0, raiz)
        dividir_filho(nova_raiz, 0, ordem_arvore)
        inserir_em_no_nao_cheio(nova_raiz, chave, ordem_arvore)
    else:
        inserir_em_no_nao_cheio(raiz, chave, ordem_arvore)

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")
    ordem_entrada = int(input_file.readline().strip())
    num_arquivos = int(input_file.readline().strip())
    linhas_arquivos_iniciais = []
    for _ in range(num_arquivos):
        linhas_arquivos_iniciais.append(input_file.readline().strip())

    num_operacoes = int(input_file.readline().strip())
    linhas_operacoes = []
    for _ in range(num_operacoes):
        linhas_operacoes.append(input_file.readline().strip())

    dic_arquivos = {}
    arvore_b = {
        "raiz": criar_no_arvore_b(folha=True),
        "ordem": ordem_entrada
    }

    for linha_arquivo in linhas_arquivos_iniciais:
        nome, tamanho, valor_hash = linha_arquivo.split(" ")
        dic_arquivos[valor_hash] = {"tamanho": tamanho, "nome": nome}
        inserir(arvore_b, valor_hash)

    for linha_operacao_str in linhas_operacoes:    
        partes_operacao = linha_operacao_str.split(" ")
        comando = partes_operacao[0] 
        if comando == "INSERT":
            nome, tamanho, valor_hash = partes_operacao[1], partes_operacao[2], partes_operacao[3]
            dic_arquivos[valor_hash] = {"tamanho": tamanho, "nome": nome}
            inserir(arvore_b, valor_hash)  
        elif comando == "SELECT":
            valor_hash = partes_operacao[1]
            executar_select(arvore_b, valor_hash, dic_arquivos, output_file)

    input_file.close()
    output_file.close()

if __name__ == '__main__':
    main(sys.argv)