# Abre el archivo .txt en modo lectura de texto
with open('c:/Users/judah/OneDrive/Escritorio/Proyectos Varios/ElectronicaDigital2/Energia-TivaC/Proyetco2_EDigital2/valores_hex.txt', 'r') as file_txt:
    # Lee el contenido del archivo y elimina los saltos de línea
    mi_string = file_txt.read().replace('\n', '').replace('\r', '')

# Reemplaza "0x" por una cadena vacía en todo el string
mi_string_sin_0x = mi_string.replace("0x", "")

# Separa los valores hexadecimales en una lista
valores_hex = mi_string_sin_0x.split(', ')

ascii_string = ""  # Inicializa el string ASCII vacío

for valor_hex in valores_hex:
    # Convierte el valor hexadecimal en un número decimal
    numero_decimal = int(valor_hex, 16)
    # Convierte el número decimal en un carácter ASCII y agrégalo al string ASCII
    valor_ascii = chr(numero_decimal)
    ascii_string += valor_ascii

print("Valores ASCII:", ascii_string)

# Guarda el string ASCII en un archivo de texto
with open('c:/Users/judah/OneDrive/Escritorio/Proyectos Varios/ElectronicaDigital2/Energia-TivaC/Proyetco2_EDigital2/archivo_ascii.txt', 'w', encoding='ISO 8859-1') as file_ascii:
    file_ascii.write(ascii_string)

print("Valores ASCII guardados en archivo_ascii.txt")
print(len(ascii_string))
