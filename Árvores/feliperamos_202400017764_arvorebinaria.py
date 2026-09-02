import sys

def formatar_saida(nome, dicionario):
    info_arquivo = dicionario[nome]
    sufixo_bytes = "byte" if info_arquivo["tamanho"] == "1" else "bytes"
    return f"{info_arquivo['posicao']}:{nome}|{info_arquivo['tipo']}|{info_arquivo['tamanho']}_{sufixo_bytes}"

def percurso_em_ordem(nome_no, dicionario, resultado):
    if nome_no is not None:
        info_no = dicionario[nome_no]
        percurso_em_ordem(info_no["esquerda"], dicionario, resultado)
        resultado.append(formatar_saida(nome_no, dicionario))
        percurso_em_ordem(info_no["direita"], dicionario, resultado)

def percurso_pre_ordem(nome_no, dicionario, resultado):
    if nome_no is not None:
        info_no = dicionario[nome_no]
        resultado.append(formatar_saida(nome_no, dicionario))
        percurso_pre_ordem(info_no["esquerda"], dicionario, resultado)
        percurso_pre_ordem(info_no["direita"], dicionario, resultado)

def percurso_pos_ordem(nome_no, dicionario, resultado):
    if nome_no is not None:
        info_no = dicionario[nome_no]
        percurso_pos_ordem(info_no["esquerda"], dicionario, resultado)
        percurso_pos_ordem(info_no["direita"], dicionario, resultado)
        resultado.append(formatar_saida(nome_no, dicionario))

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")
    num_arquivos = int(input_file.readline().strip())
    dicionario_arquivos = {}
    raiz = None 
    for i in range(num_arquivos):
        entrada = input_file.readline().strip()
        partes = entrada.rsplit(' ', 2)
        nome, tipo, tamanho = partes[0], partes[1], partes[2]
        if nome in dicionario_arquivos:
            if dicionario_arquivos[nome]["tipo"] == "rw":
                dicionario_arquivos[nome]["tipo"] = tipo
                dicionario_arquivos[nome]["tamanho"] = tamanho
                dicionario_arquivos[nome]["indice_final"] = i
            continue

        dicionario_arquivos[nome] = {
            "esquerda": None, 
            "direita": None, 
            "tipo": tipo, 
            "tamanho": tamanho, 
            "indice_final": i 
        }

        if raiz is None:
            raiz = nome
        else:
            nome_atual = raiz
            while True:
                if nome < nome_atual:
                    if dicionario_arquivos[nome_atual]["esquerda"] is None:
                        dicionario_arquivos[nome_atual]["esquerda"] = nome
                        break
                    else:
                        nome_atual = dicionario_arquivos[nome_atual]["esquerda"]
                else:
                    if dicionario_arquivos[nome_atual]["direita"] is None:
                        dicionario_arquivos[nome_atual]["direita"] = nome
                        break
                    else:
                        nome_atual = dicionario_arquivos[nome_atual]["direita"]

    arquivos_ordenados_por_finalizacao = sorted(dicionario_arquivos.values(), key=lambda x: x["indice_final"])
    for pos_final, info_arquivo in enumerate(arquivos_ordenados_por_finalizacao):
        info_arquivo["posicao"] = pos_final

    saida_epd = []
    percurso_em_ordem(raiz, dicionario_arquivos, saida_epd)
    output_file.write("[EPD]\n")
    for linha in saida_epd:
        output_file.write(linha + "\n")

    saida_ped = []
    percurso_pre_ordem(raiz, dicionario_arquivos, saida_ped)
    output_file.write("[PED]\n")
    for linha in saida_ped:
        output_file.write(linha + "\n")

    saida_edp = []
    percurso_pos_ordem(raiz, dicionario_arquivos, saida_edp)
    output_file.write("[EDP]\n")
    for linha in saida_edp:
        output_file.write(linha + "\n")

    input_file.close()
    output_file.close()

if __name__ == '__main__':
    main(sys.argv)