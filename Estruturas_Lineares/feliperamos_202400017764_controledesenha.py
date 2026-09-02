import sys

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(args[1], 'r')
    output_file = open(args[2], 'w')
    linhas = input_file.read().strip().split('\n')
    if not linhas or linhas == ['']:
        input_file.close()
        output_file.close()
        return

    indice_linha_atual = 0
    agencias = {}
    ordem_agencias = []

    num_agencias = int(linhas[indice_linha_atual])
    indice_linha_atual += 1

    for _ in range(num_agencias):
        linha = linhas[indice_linha_atual]
        nome_agencia, num_atendentes_str = linha.split()
        num_atendentes = int(num_atendentes_str)

        if nome_agencia not in agencias:
            agencias[nome_agencia] = {
                'atendentes': num_atendentes,
                'preferencial': [],
                'convencional': [],
                'indice_preferencial': 0, 
                'indice_convencional': 0   
            }
            ordem_agencias.append(nome_agencia)
        indice_linha_atual += 1
    try:
        num_pessoas = int(linhas[indice_linha_atual])
        indice_linha_atual += 1
        total_pessoas_esperando = num_pessoas

        for _ in range(num_pessoas):
            linha = linhas[indice_linha_atual]
            nome_agencia, nome_pessoa, idade_str = linha.split('|')
            idade = int(idade_str)

            if idade >= 60:
                agencias[nome_agencia]['preferencial'].append(nome_pessoa)
            else:
                agencias[nome_agencia]['convencional'].append(nome_pessoa)
            indice_linha_atual += 1
    except (IndexError, ValueError):
        total_pessoas_esperando = 0

    while total_pessoas_esperando > 0:
        for nome_agencia in ordem_agencias:
            agencia_atual = agencias[nome_agencia]
            chamados_na_rodada = []
            num_atendentes_disponiveis = agencia_atual['atendentes']
            fila_pref = agencia_atual['preferencial']
            fila_conv = agencia_atual['convencional']
            indice_pref = agencia_atual['indice_preferencial']
            indice_conv = agencia_atual['indice_convencional']

            for _ in range(num_atendentes_disponiveis):
                if indice_pref < len(fila_pref):
                    pessoa = fila_pref[indice_pref]
                    chamados_na_rodada.append(pessoa)
                    indice_pref += 1 
                elif indice_conv < len(fila_conv):
                    pessoa = fila_conv[indice_conv]
                    chamados_na_rodada.append(pessoa)
                    indice_conv += 1 
                else:
                    break

            agencia_atual['indice_preferencial'] = indice_pref
            agencia_atual['indice_convencional'] = indice_conv
            
            if chamados_na_rodada:
                total_pessoas_esperando -= len(chamados_na_rodada)
                nomes_formatados = [nome.replace(' ', '_') for nome in chamados_na_rodada]
                
                output_file.write(f"{nome_agencia}:{','.join(nomes_formatados)}\n")

    input_file.close()
    output_file.close()

if __name__ == "__main__":
    main(sys.argv)

