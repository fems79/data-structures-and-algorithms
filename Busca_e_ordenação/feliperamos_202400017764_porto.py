import sys

def mergesort(arr):
    if len(arr) > 1:
        mid = len(arr) // 2
        L = arr[:mid]
        R = arr[mid:]

        mergesort(L)
        mergesort(R)

        i = j = k = 0

        while i < len(L) and j < len(R):
            item_L = L[i]
            item_R = R[j]
            l_tem_div_cnpj = item_L[5]
            l_percent = item_L[4]
            l_ordem_cad = item_L[6]

            r_tem_div_cnpj = item_R[5]
            r_percent = item_R[4]
            r_ordem_cad = item_R[6]

            if l_tem_div_cnpj and not r_tem_div_cnpj:
                arr[k] = item_L
                i += 1
            elif not l_tem_div_cnpj and r_tem_div_cnpj:
                arr[k] = item_R
                j += 1
            
            elif l_tem_div_cnpj:
                if l_ordem_cad < r_ordem_cad:
                    arr[k] = item_L
                    i += 1
                else:
                    arr[k] = item_R
                    j += 1
            
            else:
                if l_percent > r_percent:
                    arr[k] = item_L
                    i += 1
                elif l_percent < r_percent:
                    arr[k] = item_R
                    j += 1
                
                else:
                    if l_ordem_cad < r_ordem_cad:
                        arr[k] = item_L
                        i += 1
                    else:
                        arr[k] = item_R
                        j += 1
            k += 1

        while i < len(L):
            arr[k] = L[i]
            i += 1
            k += 1

        while j < len(R):
            arr[k] = R[j]
            j += 1
            k += 1

def main(args):
    print("#ARGS = %i"%len((args)))
    print("PROGRAMA = %s"%(args[0]))
    print("ARG1 = %s, ARG2 = %s" %(args[1], args[2]))
    
    golden_input = open(args[1], "r")
    golden_output = open(args[2], "w")
    
    n = int(golden_input.readline().strip())
    conteiners_cadastrados = {}

    for i in range(n):
        entrada = golden_input.readline().strip().split()
        codigo = entrada[0]
        cnpj = entrada[1]
        peso = int(entrada[2])
        conteiners_cadastrados[codigo] = (cnpj, peso, i)

    m = int(golden_input.readline().strip())
    a_fiscalizar = []

    for i in range(m):
        entrada = golden_input.readline().strip().split()
        sel_codigo = entrada[0]
        sel_cnpj = entrada[1]
        sel_peso = int(entrada[2])

        if sel_codigo in conteiners_cadastrados:
            reg_cnpj, reg_peso, reg_ordem = conteiners_cadastrados[sel_codigo]

            tem_div_cnpj = (sel_cnpj != reg_cnpj)
            
            tem_div_peso = False
            percent_diff = 0
            peso_diff_abs = abs(sel_peso - reg_peso)

            if reg_peso == 0:
                if sel_peso != 0:
                    tem_div_peso = True
                    percent_diff = 100
            else:
                ratio_diff = peso_diff_abs / reg_peso
                if ratio_diff > 0.10:
                    tem_div_peso = True
                percent_diff = int((ratio_diff * 100) + 0.5)
            
            if tem_div_cnpj or tem_div_peso:
                item = (sel_codigo, sel_cnpj, reg_cnpj, peso_diff_abs, percent_diff, tem_div_cnpj, reg_ordem)
                a_fiscalizar.append(item)

    mergesort(a_fiscalizar)

    for item in a_fiscalizar:
        (codigo, cnpj_sel, cnpj_reg, diff_peso, percent, tem_div_cnpj, _) = item
        
        if tem_div_cnpj:
            golden_output.write(f"{codigo}:{cnpj_reg}<->{cnpj_sel}\n")
        else:
            golden_output.write(f"{codigo}:{diff_peso}kg({percent}%)\n")
            
    golden_input.close()
    golden_output.close()

if __name__ == "__main__":
    main(sys.argv)