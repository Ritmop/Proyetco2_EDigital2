# Abre el archivo .txt en modo lectura de texto
with open('c:/Users/judah/OneDrive/Escritorio/Proyectos Varios/ElectronicaDigital2/Energia-TivaC/Proyetco2_EDigital2/valores_hex.txt', 'r') as file_txt:
    # Lee el contenido del archivo y elimina los caracteres especiales y la cadena 0x
    mi_string = file_txt.read().replace('\n', '').replace('\r', '').replace(' ', '').replace("0x", "")

# Separa los valores hexadecimales en una lista
valores_hex = mi_string.split(',')

ascii_string = ""  # Inicializa el string ASCII vacío

for valor_hex in valores_hex:
    # Divide el valor hexadecimal en sus nibbles individuales
    nibble1 = valor_hex[0]
    nibble2 = valor_hex[1]

    # Convierte cada nibble en un número decimal
    numero_decimal1 = int(nibble1, 16)
    numero_decimal2 = int(nibble2, 16)

    # Convierte los números decimales en caracteres ASCII legibles y agrégalo al string ASCII (48 = '0')
    valor_ascii1 = chr(numero_decimal1 + 48)
    valor_ascii2 = chr(numero_decimal2 + 48)    

    ascii_string += valor_ascii1 + valor_ascii2    
    print(f"H {nibble1} > {valor_ascii1} --- L {nibble2} > {valor_ascii2}")

#print("Valores ASCII:", ascii_string)

# Guarda el string ASCII en un archivo de texto
with open('c:/Users/judah/OneDrive/Escritorio/Proyectos Varios/ElectronicaDigital2/Energia-TivaC/Proyetco2_EDigital2/archivo_ascii.txt', 'w') as file_ascii:
    file_ascii.write(ascii_string)

print("Valores ASCII guardados en archivo_ascii.txt")
print(len(ascii_string))
