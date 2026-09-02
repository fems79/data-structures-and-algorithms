import sys

def get_altura(arvore, palavra):
    if palavra is None or palavra not in arvore:
        return 0
    return arvore[palavra]['altura']

def get_balanceamento(arvore, palavra):
    if palavra is None:
        return 0
    altura_esq = get_altura(arvore, arvore[palavra]['esquerda'])
    altura_dir = get_altura(arvore, arvore[palavra]['direita'])
    return altura_esq - altura_dir

def rotacao_direita(arvore, z):
    y = arvore[z]['esquerda']
    t3 = arvore[y]['direita']

    arvore[y]['direita'] = z
    arvore[z]['esquerda'] = t3

    altura_z_esq = get_altura(arvore, arvore[z]['esquerda'])
    altura_z_dir = get_altura(arvore, arvore[z]['direita'])
    arvore[z]['altura'] = 1 + max(altura_z_esq, altura_z_dir)

    altura_y_esq = get_altura(arvore, arvore[y]['esquerda'])
    altura_y_dir = get_altura(arvore, arvore[y]['direita'])
    arvore[y]['altura'] = 1 + max(altura_y_esq, altura_y_dir)
    return y

def rotacao_esquerda(arvore, z):
    y = arvore[z]['direita']
    t2 = arvore[y]['esquerda']

    arvore[y]['esquerda'] = z
    arvore[z]['direita'] = t2

    altura_z_esq = get_altura(arvore, arvore[z]['esquerda'])
    altura_z_dir = get_altura(arvore, arvore[z]['direita'])
    arvore[z]['altura'] = 1 + max(altura_z_esq, altura_z_dir)

    altura_y_esq = get_altura(arvore, arvore[y]['esquerda'])
    altura_y_dir = get_altura(arvore, arvore[y]['direita'])
    arvore[y]['altura'] = 1 + max(altura_y_esq, altura_y_dir)
    return y

def inserir(arvore, raiz_palavra, nova_palavra):
    if not raiz_palavra:
        return nova_palavra

    if nova_palavra == raiz_palavra:
        return raiz_palavra

    if nova_palavra < raiz_palavra:
        filho_esq = inserir(arvore, arvore[raiz_palavra]['esquerda'], nova_palavra)
        arvore[raiz_palavra]['esquerda'] = filho_esq
    else: 
        filho_dir = inserir(arvore, arvore[raiz_palavra]['direita'], nova_palavra)
        arvore[raiz_palavra]['direita'] = filho_dir

    altura_esq = get_altura(arvore, arvore[raiz_palavra]['esquerda'])
    altura_dir = get_altura(arvore, arvore[raiz_palavra]['direita'])
    arvore[raiz_palavra]['altura'] = 1 + max(altura_esq, altura_dir)
    balanceamento = get_balanceamento(arvore, raiz_palavra)

    if balanceamento > 1 and nova_palavra < arvore[raiz_palavra]['esquerda']:
        return rotacao_direita(arvore, raiz_palavra)

    if balanceamento < -1 and nova_palavra > arvore[raiz_palavra]['direita']:
        return rotacao_esquerda(arvore, raiz_palavra)

    if balanceamento > 1 and nova_palavra > arvore[raiz_palavra]['esquerda']:
        filho_esq = rotacao_esquerda(arvore, arvore[raiz_palavra]['esquerda'])
        arvore[raiz_palavra]['esquerda'] = filho_esq
        return rotacao_direita(arvore, raiz_palavra)

    if balanceamento < -1 and nova_palavra < arvore[raiz_palavra]['direita']:
        filho_dir = rotacao_direita(arvore, arvore[raiz_palavra]['direita'])
        arvore[raiz_palavra]['direita'] = filho_dir
        return rotacao_esquerda(arvore, raiz_palavra)
    return raiz_palavra

def buscar(arvore, dic_sinonimos, raiz_palavra, palavra_consulta):
    if not raiz_palavra:
        return ["?"], None

    caminho = []
    palavra_atual = raiz_palavra
    while palavra_atual is not None:
        caminho.append(palavra_atual)
        if palavra_consulta == palavra_atual:
            return caminho, dic_sinonimos[palavra_consulta]['sinonimos']
        elif palavra_consulta < palavra_atual:
            palavra_atual = arvore[palavra_atual]['esquerda']
        else:
            palavra_atual = arvore[palavra_atual]['direita']

    caminho.append("?")
    return caminho, None

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)
    
    input = open(sys.argv[1], "r")
    output = open(sys.argv[2], "w")

    arvore_avl = {}
    dic_sinonimos = {}
    raiz = None

    num_palavras_str = input.readline()
    if num_palavras_str and num_palavras_str.strip().isdigit():
        num_palavras = int(num_palavras_str.strip())
        for _ in range(num_palavras):
            linha = input.readline().strip()
            if not linha:
                continue
            
            partes = linha.split(" ", 2)
            if len(partes) < 3:
                continue
            
            palavra = partes[0]
            sinonimos_str = ",".join(partes[2].split())

            if palavra not in dic_sinonimos:
                dic_sinonimos[palavra] = {"sinonimos": sinonimos_str}
                arvore_avl[palavra] = {"esquerda": None, "direita": None, "altura": 1}
                raiz = inserir(arvore_avl, raiz, palavra)

    lista_consultas = []
    num_consultas_str = input.readline()
    if num_consultas_str and num_consultas_str.strip().isdigit():
        num_consultas = int(num_consultas_str.strip())
        for _ in range(num_consultas):
            consulta = input.readline().strip()
            if consulta:
                lista_consultas.append(consulta)

    for consulta in lista_consultas:
        caminho, sinonimos_encontrados = buscar(arvore_avl, dic_sinonimos, raiz, consulta)
        output.write(f"[{'->'.join(caminho)}]\n")
        if sinonimos_encontrados:
            output.write(sinonimos_encontrados + "\n")
        else:
            output.write("-\n")
            
    input.close()
    output.close()


if __name__ == '__main__':
    main(sys.argv)
