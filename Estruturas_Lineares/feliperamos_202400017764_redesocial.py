import sys

def main(args):
    print("Quantidade de argumentos (len(args)): " + str(len(args)))
    for i, arg in enumerate(args):
        print("Argumento " + str(i) + " (args[" + str(i) + "]): " + arg)

    input_file = open(sys.argv[1], "r")
    output_file = open(sys.argv[2], "w")
    rede_social = {}
    inicio = None
    final = None

    def add(nome):
        nonlocal rede_social, inicio, final
        if nome in rede_social:
            return f"[FAILURE]ADD={nome}"
        if not inicio:
            inicio = nome
            final = nome
            rede_social[nome] = {"anterior": nome, "proximo": nome}
        else:
            rede_social[nome] = {"anterior": final, "proximo": inicio}
            rede_social[final]["proximo"] = nome
            rede_social[inicio]["anterior"] = nome
            final = nome
        return f"[SUCCESS]ADD={nome}"

    def remove(nome):
        nonlocal rede_social, inicio, final
        if nome not in rede_social:
            return f"[FAILURE]REMOVE={nome}"
        if rede_social[nome]["proximo"] == nome:
            inicio = None
            final = None
        else:
            amigo_anterior = rede_social[nome]["anterior"]
            amigo_proximo = rede_social[nome]["proximo"]

            rede_social[amigo_anterior]["proximo"] = amigo_proximo
            rede_social[amigo_proximo]["anterior"] = amigo_anterior
            if nome == inicio:
                inicio = amigo_proximo
            if nome == final:
                final = amigo_anterior
        del rede_social[nome]
        return f"[SUCCESS]REMOVE={nome}"

    def show(nome):
        if nome not in rede_social:
            return f"[FAILURE]SHOW=?<-{nome}->?"
        amigo_anterior = rede_social[nome]["anterior"]
        amigo_proximo = rede_social[nome]["proximo"]
        return f"[SUCCESS]SHOW={amigo_anterior}<-{nome}->{amigo_proximo}"
    for linha in input_file:
        linha_limpa = linha.strip() 
        if not linha_limpa:
            continue
        
        comando, nome = linha_limpa.split(" ", 1)
        resultado = ""
        if comando == "ADD":
            resultado = add(nome)
        elif comando == "REMOVE":
            resultado = remove(nome)
        elif comando == "SHOW":
            resultado = show(nome)
        
        print(resultado.replace(" ","_"), file=output_file)

    input_file.close()
    output_file.close()
    
if __name__ == '__main__':
    main(sys.argv)