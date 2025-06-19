#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <folder_fs> <punto_montaje>\n", argv[0]);
        return 1;
    }

    printf("Montando BWFS desde: %s en %s\n", argv[1], argv[2]);
    // Aquí iría el uso de FUSE para montar el FS
    return 0;
}
