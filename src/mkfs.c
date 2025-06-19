#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio>\n", argv[0]);
        return 1;
    }

    printf("Creando sistema de archivos BWFS en: %s\n", argv[1]);
    // Aquí iría la lógica de creación del FS
    return 0;
}
