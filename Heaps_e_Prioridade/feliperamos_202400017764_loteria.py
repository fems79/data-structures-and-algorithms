import sys

def _sift_down(heap, i, tamanho):
    min_index = i
    filho_e = 2 * i + 1
    filho_d = 2 * i + 2

    if filho_e < tamanho and heap[filho_e] < heap[min_index]:
        min_index = filho_e

    if filho_d < tamanho and heap[filho_d] < heap[min_index]:
        min_index = filho_d

    if i != min_index:
        heap[i], heap[min_index] = heap[min_index], heap[i]
        _sift_down(heap, min_index, tamanho)

def heapify(x):
    n = len(x)
    for i in range(n // 2 - 1, -1, -1):
        _sift_down(x, i, n)

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")

    premio = int(input_file.readline().strip())
    apostas = int(input_file.readline().strip())
    numeros_sorteados = set(input_file.readline().strip().split())

    todas_as_apostas = []
    for _ in range(apostas):
        linha_completa = input_file.readline().strip()
        partes = linha_completa.split(' ', 1)
        codigo = partes[0]
        numeros_jogados = partes[1].split()
        
        acertos = 0
        for numero in numeros_jogados:
            if numero in numeros_sorteados:
                acertos += 1
        
        todas_as_apostas.append((acertos, codigo))

    if not todas_as_apostas:
        input_file.close()
        output_file.close()
        return

    min_heap_candidatos = list(todas_as_apostas)
    heapify(min_heap_candidatos)
    min_acertos = min_heap_candidatos[0][0]

    max_heap_candidatos = [(-acertos, codigo) for acertos, codigo in todas_as_apostas]
    heapify(max_heap_candidatos)
    max_acertos = -max_heap_candidatos[0][0]

    if min_acertos == max_acertos:
        ganhadores_codigos = [codigo for acertos, codigo in todas_as_apostas]
        num_ganhadores = len(ganhadores_codigos)
        
        if num_ganhadores > 0:
            premio_individual = int(premio / num_ganhadores)
            output_file.write(f"[{num_ganhadores}:{max_acertos}:{premio_individual}]\n")
            for codigo in ganhadores_codigos:
                output_file.write(codigo + "\n")
    else:
        min_ganhadores_codigos = []
        max_ganhadores_codigos = []
        
        for acertos, codigo in todas_as_apostas:
            if acertos == min_acertos:
                min_ganhadores_codigos.append(codigo)
            elif acertos == max_acertos:
                max_ganhadores_codigos.append(codigo)
        
        metade_premio = premio / 2
        
        num_max_ganhadores = len(max_ganhadores_codigos)
        if num_max_ganhadores > 0:
            premio_individual_max = int(metade_premio / num_max_ganhadores)
            output_file.write(f"[{num_max_ganhadores}:{max_acertos}:{premio_individual_max}]\n")
            for codigo in max_ganhadores_codigos:
                output_file.write(codigo + "\n")
        
        num_min_ganhadores = len(min_ganhadores_codigos)
        if num_min_ganhadores > 0:
            premio_individual_min = int(metade_premio / num_min_ganhadores)
            output_file.write(f"[{num_min_ganhadores}:{min_acertos}:{premio_individual_min}]\n")
            for codigo in min_ganhadores_codigos:
                output_file.write(codigo + "\n")

    input_file.close()
    output_file.close()

if __name__ == "__main__":
    main(sys.argv)