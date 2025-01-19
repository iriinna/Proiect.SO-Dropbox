#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h> 

#define BAZA "baza" 

// Functie pentru crearea directorului de baza
void initializeaza_baza() {
    struct stat st;
    if (stat(BAZA, &st) == -1) { 
        mkdir(BAZA, 0777);       
    }
}

// Construieste calea completa a unui fisier/director
void construieste_cale(char *cale_completa, const char *cale_relativa) {
    snprintf(cale_completa, 1024, "%s/%s", BAZA, cale_relativa);
}

// Functie copiere fisiere
void copiaza_fisier(const char *sursa, const char *destinatie) {
    char cale_sursa[1024], cale_destinatie[1024];
    int fisier_sursa, fisier_destinatie;
    char buffer[1024];
    ssize_t bytes;

    // Construieste calea completa pentru fisierul sursa
    if (sursa[0] == '/' || strstr(sursa, "../") == sursa || strstr(sursa, "./") == sursa) {
        snprintf(cale_sursa, sizeof(cale_sursa), "%s", sursa);
    } else {
        construieste_cale(cale_sursa, sursa);
    }

    // Construieste calea completa pentru fisierul destinatie
    if (destinatie[0] == '/' || strstr(destinatie, "../") == destinatie || strstr(destinatie, "./") == destinatie) {
        snprintf(cale_destinatie, sizeof(cale_destinatie), "%s", destinatie);
    } else {
        construieste_cale(cale_destinatie, destinatie);
    }

    //Erori
    fisier_sursa = open(cale_sursa, O_RDONLY);
    if (fisier_sursa == -1) {
        perror("Eroare la deschiderea fisierului sursa");
        return;
    }

    fisier_destinatie = open(cale_destinatie, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fisier_destinatie == -1) {
        perror("Eroare la crearea fisierului destinatie");
        close(fisier_sursa);
        return;
    }

    while ((bytes = read(fisier_sursa, buffer, sizeof(buffer))) > 0) {
        write(fisier_destinatie, buffer, bytes);
    }

    close(fisier_sursa);
    close(fisier_destinatie);

    printf("Fisier copiat cu succes din %s in %s\n", cale_sursa, cale_destinatie);
}

// Functie mutare fisiere
void muta_fisier(const char *sursa, const char *destinatie) {
    copiaza_fisier(sursa, destinatie); 
    if (remove(sursa) == -1) { 
        perror("Eroare la stergerea fisierului sursa");
    } else {
        printf("Fisier mutat cu succes din %s in %s\n", sursa, destinatie);
    }
}
// Functie creare director
void creeaza_director(const char *cale) {
    char cale_completa[1024];

    if (cale[0] == '/' || strstr(cale, "../") == cale || strstr(cale, "./") == cale) {
        snprintf(cale_completa, sizeof(cale_completa), "%s", cale);
    } else {
        construieste_cale(cale_completa, cale);
    }

    if (mkdir(cale_completa, 0777) == -1) {
        perror("Eroare la crearea directorului");
    } else {
        printf("Director creat cu succes: %s\n", cale_completa);
    }
}

// Functie stergere
void sterge_element(const char *cale) {
    char cale_completa[1024];

    if (cale[0] == '/' || strstr(cale, "../") == cale || strstr(cale, "./") == cale) {
        snprintf(cale_completa, sizeof(cale_completa), "%s", cale);
    } else {
        construieste_cale(cale_completa, cale);
    }

    if (remove(cale_completa) == -1) {
        perror("Eroare la stergerea fisierului sau directorului");
    } else {
        printf("Fisier sau director sters cu succes: %s\n", cale_completa);
    }
}

// Functie ls
void listeaza_director(const char *cale) {
    char cale_completa[1024];
    DIR *d;
    struct dirent *entry;

    if (cale[0] == '/' || strstr(cale, "../") == cale || strstr(cale, "./") == cale) {
        snprintf(cale_completa, sizeof(cale_completa), "%s", cale);
    } else {
        construieste_cale(cale_completa, cale);
    }

    d = opendir(cale_completa);
    if (d == NULL) {
        perror("Eroare la deschiderea directorului");
        return;
    }

    printf("Continutul directorului %s:\n", cale_completa);
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(d);
}

// Meniu
void afiseaza_ajutor() {
    printf("Comenzi disponibile:\n");
    printf("  cp <sursa> <destinatie> - Copiaza un fisier\n");
    printf("  mv <sursa> <destinatie> - Muta un fisier\n");
    printf("  mkdir <cale>            - Creeaza un director\n");
    printf("  rm <cale>               - Sterge un fisier sau director\n");
    printf("  ls <cale>               - Listeaza continutul unui director\n");
    printf("  exit                    - Iesire\n");
}


void interpreteaza_comanda(char *comanda) {
    char *argumente[3];
    int i = 0;

    char *token = strtok(comanda, " ");
    while (token != NULL) {
        argumente[i++] = token;
        token = strtok(NULL, " ");
    }

    if (i == 0) return;

    if (strcmp(argumente[0], "help") == 0) {
        afiseaza_ajutor();
    } else if (strcmp(argumente[0], "cp") == 0 && i == 3) {
        copiaza_fisier(argumente[1], argumente[2]);
    } else if (strcmp(argumente[0], "mv") == 0 && i == 3) {
        muta_fisier(argumente[1], argumente[2]);
    } else if (strcmp(argumente[0], "mkdir") == 0 && i == 2) {
        creeaza_director(argumente[1]);
    } else if (strcmp(argumente[0], "rm") == 0 && i == 2) {
        sterge_element(argumente[1]);
    } else if (strcmp(argumente[0], "ls") == 0 && i == 2) {
        listeaza_director(argumente[1]);
    } else {
        printf("Comanda necunoscuta sau argumente incorecte. Scrie 'help' pentru ajutor.\n");
    }
}

int main() {
    char comanda[1024];

    initializeaza_baza();

    printf("Bine ai venit in gestionarul de fisiere! Scrie 'help' pentru a vedea comenzile disponibile.\n");

    while (1) {
        printf("dbxcli> ");
        fgets(comanda, sizeof(comanda), stdin);

        comanda[strcspn(comanda, "\n")] = 0;

        if (strcmp(comanda, "exit") == 0) {
            break;
        }

        interpreteaza_comanda(comanda);
    }

    printf("La revedere!\n");
    return 0;
}
