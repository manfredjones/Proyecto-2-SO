#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio>\n", argv[0]);
        return 1;
    }

    printf("Chequeando consistencia del FS en: %s\n", argv[1]);
    // Aquí iría la lógica de verificación del FS
    return 0;
}
