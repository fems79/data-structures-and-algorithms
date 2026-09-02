import sys

def checksum(palavra):
    resposta = 0
    for a in palavra:
 
        if a == " ":
            continue
        resposta ^= ord(a)
    return resposta
def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)
    
    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")

    servidores, capacidade = map(int, input_file.readline().split())
    def h1(palavra):
        return 7919 * checksum(palavra) % servidores
    def h2(palavra):
        calculo = (104729 * checksum(palavra) + 123) % servidores
        if calculo == 0:
            return 1 
        else:
            return calculo       
    def dbhash(palavra, i):
        return (h1(palavra) + i*h2(palavra)) % servidores

    requisicoes = int(input_file.readline())
    conteudos = []
    for _ in range(requisicoes):
        linha = input_file.readline().strip()
        conteudos.append(linha.split(" ", 1)[1])

    servidor_e_conteudo = {i: [] for i in range(servidores)}
    for conteudo in conteudos:
        servidor_primario = h1(conteudo)
        servidor_atual = servidor_primario
        i = 0
        while len(servidor_e_conteudo[servidor_atual]) >= capacidade:
            i += 1
            servidor_atual = dbhash(conteudo, i)
        if servidor_atual != servidor_primario:
            print(f"S{servidor_primario}->S{servidor_atual}".replace(" ","_"), file=output_file) 
        
        conteudo_existente = servidor_e_conteudo[servidor_atual]
        
        if len(conteudo_existente) == 0:
            print(f"S{servidor_atual}:{conteudo}".replace(" ","_"), file=output_file)
        else:
            conteudo_utilizado = ",".join(conteudo_existente)
            print(f"S{servidor_atual}:{conteudo_utilizado},{conteudo}".replace(" ","_"), file=output_file)
            
        servidor_e_conteudo[servidor_atual].append(conteudo)
    
    input_file.close()
    output_file.close()

if __name__ == '__main__':

    main(sys.argv)