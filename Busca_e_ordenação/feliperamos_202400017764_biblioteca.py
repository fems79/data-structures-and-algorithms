import sys

def busca_binaria(lista_isbns, isbn_alvo):
    i, j = 0, len(lista_isbns) - 1
    if i > j:
        return -1, 0
    p = (i + j) // 2
    contador = 1
    while j >= i and lista_isbns[p] != isbn_alvo:
        if isbn_alvo < lista_isbns[p]:
            j = p - 1
        else:
            i = p + 1
        contador += 1
        if i > j:
            break
        p = (i + j) // 2

    if j >= i and lista_isbns[p] == isbn_alvo:
        return p, contador
    else:
        return -1, contador
def busca_interpolacao(lista_isbns, isbn_alvo):
    ii, jj = 0, len(lista_isbns) - 1
    if ii > jj:
        return -1, 0
    contador = 1
    if jj > ii:
        pp = ii + (lista_isbns[jj] - lista_isbns[ii]) % (jj - ii + 1)
    else:
        pp = ii  
    while jj >= ii and lista_isbns[pp] != isbn_alvo:
        if isbn_alvo < lista_isbns[pp]:
            jj = pp - 1
        else:
            ii = pp + 1
        contador += 1
        if ii > jj:
            contador -= 1
            break
        if jj > ii:
            pp = ii + (lista_isbns[jj] - lista_isbns[ii]) % (jj - ii + 1)
        else:
            pp = ii
    if jj >= ii and lista_isbns[pp] == isbn_alvo:
        return pp, contador
    else:
        return -1, contador
def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)
    
    input_file = open(args[1], "r", encoding='utf-8')
    output_file = open(args[2], "w", encoding='utf-8')
    quantidade = int(input_file.readline().strip())
    ISBNs = []
    for i in range(quantidade):
        entrada = input_file.readline().strip()
        if entrada:
            partes = entrada.split('&', 1)
            isbn_autor = partes[0]
            titulo = partes[1]
            isbn_str, autor = isbn_autor.split(' ', 1)
            ISBNs.append((int(isbn_str), autor, titulo))
    
    ISBNs_ordenados = sorted(ISBNs)
    isbns_para_busca = [livro[0] for livro in ISBNs_ordenados]
    mapa_livros = {livro[0]: (livro[1], livro[2]) for livro in ISBNs_ordenados}
    consultas = int(input_file.readline().strip())
    isbn_consulta = []
    for i in range(consultas):
        isbn_linha = input_file.readline().strip()
        if isbn_linha:
            isbn_consulta.append(int(isbn_linha))

    vitorias_bi = 0
    vitorias_bb = 0
    contador_total_bb = 0
    contador_total_bi = 0
    for k in range(consultas):
        isbn_alvo = isbn_consulta[k]  
        idx_encontrado_bb, contador_bb = busca_binaria(isbns_para_busca, isbn_alvo)
        idx_encontrado_bi, contador_bi = busca_interpolacao(isbns_para_busca, isbn_alvo) 
        if idx_encontrado_bb != -1:
            autor, titulo = mapa_livros[isbn_alvo]
            Autor = autor.replace(" ","_")
            Titulo = titulo.replace(" ","_")
            output_file.write(f"[{isbn_alvo}]B={contador_bb}|I={contador_bi}|Author:{Autor},Title:{Titulo}\n")
        else:
            output_file.write(f"[{isbn_alvo}]B={contador_bb}|I={contador_bi}|ISBN_NOT_FOUND\n")
        
        contador_total_bb += contador_bb
        contador_total_bi += contador_bi
        if contador_bi <= contador_bb:
            vitorias_bi += 1
        else:
            vitorias_bb += 1
    
    if consultas > 0:
        output_file.write(f"BINARY={vitorias_bb}:{contador_total_bb//consultas}\n")
        output_file.write(f"INTERPOLATION={vitorias_bi}:{contador_total_bi//consultas}\n")
    
    input_file.close()
    output_file.close()

if __name__ == '__main__':
    main(sys.argv)