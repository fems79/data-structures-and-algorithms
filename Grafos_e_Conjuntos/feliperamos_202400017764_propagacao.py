import sys

next_val = 1

def myrand():
    global next_val
    next_val = (next_val * 1103515245 + 12345) & 0xFFFFFFFF
    return next_val

def myrand_shuffle(lista):
    n = len(lista)
    for i in range(n - 1, 0, -1):
        j = myrand() % (i + 1)
        lista[i], lista[j] = lista[j], lista[i]

parent = {}
rank = {}

def make_set(v):
    parent[v] = v
    rank[v] = 0

def find_set(v):
    if v == parent[v]:
        return v
    parent[v] = find_set(parent[v])
    return parent[v]

def union_sets(a, b):
    a = find_set(a)
    b = find_set(b)
    if a != b:
        if rank[a] < rank[b]:
            a, b = b, a
        parent[b] = a
        if rank[a] == rank[b]:
            rank[a] += 1

def executar_simulacao(altura, largura, x_inicial, y_inicial):
    global parent, rank
    parent.clear()
    rank.clear()

    todas_as_pessoas = [(x, y) for y in range(altura) for x in range(largura)]
    for pessoa in todas_as_pessoas:
        make_set(pessoa)

    conexoes = []
    for y in range(altura):
        for x in range(largura):
            if y + 1 < altura:
                conexoes.append(((x, y), (x, y + 1)))
            if x + 1 < largura:
                conexoes.append(((x, y), (x + 1, y)))

    myrand_shuffle(conexoes)

    adjacencia = {pessoa: [] for pessoa in todas_as_pessoas}
    num_conexoes_feitas = 0
    total_pessoas = len(todas_as_pessoas)

    for pessoa1, pessoa2 in conexoes:
        if find_set(pessoa1) != find_set(pessoa2):
            union_sets(pessoa1, pessoa2)
            adjacencia[pessoa1].append(pessoa2)
            adjacencia[pessoa2].append(pessoa1)
            num_conexoes_feitas += 1
            if num_conexoes_feitas == total_pessoas - 1:
                break

    paciente_zero = (x_inicial, y_inicial)
    caminho_final = []
    visitados = set()
    pilha = [paciente_zero]

    while pilha:
        pessoa_atual = pilha[-1]
        del pilha[-1]

        if pessoa_atual not in visitados:
            visitados.add(pessoa_atual)
            caminho_final.append(pessoa_atual)
            for vizinho in sorted(adjacencia[pessoa_atual], reverse=True):
                if vizinho not in visitados:
                    pilha.append(vizinho)
    
    return caminho_final

def main(args):
    if len(args) < 3:
        print("Uso: python script.py <arquivo_de_entrada> <arquivo_de_saida>")
        return

    input_file = open(args[1], 'r')
    output_file = open(args[2], 'w')

    global next_val
    next_val = 1 

    num_regioes_str = input_file.readline()
    if not num_regioes_str.strip():
        input_file.close()
        output_file.close()
        return
    
    num_regioes = int(num_regioes_str.strip())
    resultados_finais = []

    for i in range(num_regioes):
        linha_regiao = input_file.readline()
        if not linha_regiao.strip():
            break
        
        partes = linha_regiao.strip().split()
        altura, largura, x1, y1 = [int(p) for p in partes]

        caminho = executar_simulacao(altura, largura, x1, y1)
        
        caminho_str = ";".join([f"({x},{y})" for x, y in caminho])
        resultados_finais.append(f"{i + 1}:{caminho_str}")

    for resultado in resultados_finais:
        output_file.write(resultado + "\n")

    input_file.close()
    output_file.close()

if __name__ == "__main__":
    main(sys.argv)
