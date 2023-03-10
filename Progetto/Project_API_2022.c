#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NERO 'N'
#define ROSSO 'R'
#define NCAR 123

typedef struct confronto {
    char carattere;
    int occorrenze;

    struct confronto *next;
    struct confronto *fake;
} lista_confronto;

typedef struct caratteri {
    short int presente;
    short int *vett_pos;
} vettore_caratteri;

typedef struct nodo_rb {
    bool filtro;
    char colore;
    unsigned long chiave;

    struct nodo_rb *padre;
    struct nodo_rb *sinistro;
    struct nodo_rb *destro;
    struct nodo_rb *next;

    char parola[];
} nodo_rb_t;

//inizializzo le variabili globali
nodo_rb_t *NIL, *r_p_ammesse = NULL, *ltrue = NULL, *lfalse = NULL;
int contatore, k;
lista_confronto *vincoli_p = NULL, *short_p = NULL;
char *jolly, lettura [BUFSIZ];
bool mod_jolly, esistono_vincoli;
vettore_caratteri vett_car [NCAR];

//funzioni che gestiscono i singoli nodi
void nodo_nil ();
nodo_rb_t *Crea_Nodo (char *parola);
int Conta_Occ (char *parola, char carattere);
int node_compare (nodo_rb_t *new, nodo_rb_t *curr);

//funzioni che gestiscono l'albero
nodo_rb_t *Inserisci_Nodo (nodo_rb_t **radice_ptr, char *string);
void Inserisci_Ripara (nodo_rb_t **radice, nodo_rb_t *ins);
nodo_rb_t *Trova_Nodo (char *string);
void LeftRotate (nodo_rb_t **radice, nodo_rb_t *da_ruotare);
void RightRotate (nodo_rb_t **radice, nodo_rb_t *da_ruotare);

//funzioni che gestiscono i vincoli appresi
lista_confronto *Cerca_Vincolo (lista_confronto *testa, char c);
void Inserisci_Vincolo (lista_confronto **head, lista_confronto *ins);
void Conta_Totali (char *parola, short int *vett);

//funzioni del gioco
char Partita (char *parola_letta);
char Lettura (int comando, char *parola_letta);
char Inserimento ();
void Stampa_Filtrate (nodo_rb_t *radice);
void Ripristina_Filtri (lista_confronto **head);
void Confronto (char *rif, nodo_rb_t *letta, short int *occ_rif);
void Visita_Vincoli ();

int main () {
    contatore = 0;
    //leggo il numero che indica la lunghezza delle parole
    if (fgets (lettura, BUFSIZ, stdin) == NULL) {
        return 0;
    }
    sscanf (lettura, "%d", &k);

    char parola_letta [k + 1], comando = 'I';

    jolly = (char *) malloc (k * sizeof (char));
    nodo_nil ();

    while (comando != 'E') {
        if (comando == 'S') {
            Stampa_Filtrate (r_p_ammesse);
            comando = 'I';
        }
        else if (comando == 'P') {
            comando = Partita (parola_letta);
        }
        else if (comando == 'F') {
            comando = Lettura (0, parola_letta);
        }
        else if (comando == 'T') {
            comando = Lettura (0, parola_letta);
        }
        else {
            comando = Inserimento ();
        }
    }
    return 0;
}

void nodo_nil () {
    nodo_rb_t *new = (nodo_rb_t *) malloc (sizeof (nodo_rb_t) + k + 1);

    NIL = new;
    NIL->filtro = true;
    NIL->padre = NIL;
    NIL->sinistro = NIL;
    NIL->destro = NIL;
    memcpy(NIL->parola, "NIL", sizeof ("NIL"));

    NIL->chiave = 0;
    NIL->next = NULL;
    NIL->colore = NERO;
}

nodo_rb_t *Crea_Nodo (char *parola) {
    nodo_rb_t *new = (nodo_rb_t *) malloc (sizeof (nodo_rb_t) + k + 1);

    new->filtro = true;
    new->padre = NIL;
    new->sinistro = NIL;
    new->destro = NIL;
    memcpy(new->parola, parola,k);
    new->parola[k] = '\0';
    new->next = NULL;
    new->colore = ROSSO;
    new->chiave = new->parola [0] * 474552 + new->parola [1] * 6084 + new->parola [2] * 78 + new->parola [3];

    //se conosco già dei vincoli etichetto direttamente la nuova parola inserita
    if (esistono_vincoli == true) {
        int myvet, myocc;
        short int i, occ;

        //confronto con il vettore delle giuste al posto giusto e verifico se ho delle lettere che non dovrei avere
        for (i = 0; i < k; i++) {
            if (jolly[i] != '/') {
                if (new->parola[i] != jolly[i]) {
                    new->filtro = false;
                    goto END;
                }
            }
            else {
                myvet = vett_car[(int) new->parola[i]].presente;

                if ((myvet == -1) || ((myvet == 1) && (vett_car[(int) new->parola[i]].vett_pos[i] == -1))) {
                    new->filtro = false;
                    goto END;
                }
            }
        }

        //verifico se ho tutte le lettere della parola
        lista_confronto *curr = vincoli_p;

        while (curr != NULL) {
            occ = Conta_Occ (new->parola, curr->carattere);
            myocc = curr->occorrenze;

            if (occ != 0) {
                if (((myocc > 0) && (occ != myocc)) || ((myocc < 0) && (occ < -myocc))) {
                    new->filtro = false;
                    goto END;
                }
            }
            else {
                new->filtro = false;
                goto END;
            }

            curr = curr->next;
        }
    }

    contatore++;
    new->next = ltrue;
    ltrue = new;
    return new;

    END:
    new->next = lfalse;
    lfalse = new;
    return new;
}

nodo_rb_t *Inserisci_Nodo (nodo_rb_t **radice_ptr, char *string) {
    // Albero vuoto ==> creo la radice
    if (*radice_ptr == NULL) {
        *radice_ptr = Crea_Nodo (string);
        (*radice_ptr)->colore = NERO;
        return *radice_ptr;
    }

    // Albero non vuoto ==> scorro nell'albero, inserisco il nodo in fondo e poi riparo l'albero
    nodo_rb_t *prec = NIL, *curr = *radice_ptr;
    nodo_rb_t *new = Crea_Nodo (string);

    //scendo nell'albero finchè arrivo alla foglia giusta per inserire il nuovo nodo
    while (curr != NIL) {
        prec = curr;

        if (node_compare (new, curr) < 0) {
            curr = curr->sinistro;
        }
        else {
            curr = curr->destro;
        }
    }

    if (node_compare (new, prec) < 0) {
        prec->sinistro = new;
    }
    else {
        prec->destro = new;
    }

    new->padre = prec;

    return new;
}

int node_compare (nodo_rb_t *new, nodo_rb_t *curr) {
    //restituisce -1 se new è < di curr, 0 se è uguale, 1 se è maggiore
    if (new->chiave < curr->chiave) {
        return -1;
    }
    else if (new->chiave > curr->chiave) {
        return 1;
    }
    else {
        return strcmp (new->parola, curr->parola);
    }
}

void Inserisci_Ripara (nodo_rb_t **radice, nodo_rb_t *ins) {
    nodo_rb_t *z, *a, *p = ins->padre;

    if ((p != NIL) && (p->colore == ROSSO)) {
        a = p->padre;
        //il padre del nodo da inserire è figlio sinistro
        if (p == a->sinistro) {
            z = a->destro;

            //il nodo da inserire ha uno zio rosso
            if ((z != NIL) && (z->colore == ROSSO)) {
                p->colore = NERO;
                z->colore = NERO;
                a->colore = ROSSO;
                Inserisci_Ripara (&(*radice), a);
            }
            //il nodo da inserire non ha uno zio oppure lo zio è nero
            else {
                if (ins == p->destro) {
                    ins = p;
                    LeftRotate (radice, ins);
                    p = ins->padre;
                }

                p->colore = NERO;
                a->colore = ROSSO;
                RightRotate (radice, a);
            }
        }
        //il padre del nodo da inserire è figlio destro
        else {
            z = a->sinistro;

            //il nodo da inserire ha uno zio rosso
            if ((z != NIL) && (z->colore == ROSSO)) {
                p->colore = NERO;
                z->colore = NERO;
                a->colore = ROSSO;
                Inserisci_Ripara (&(*radice), a);
            }
            //il nodo da inserire non ha uno zio oppure lo zio è nero
            else {
                if (ins == p->sinistro) {
                    ins = p;
                    RightRotate (radice, ins);
                    p = ins->padre;
                }

                p->colore = NERO;
                a->colore = ROSSO;
                LeftRotate (radice, a);
            }
        }
    }

    (*radice)->colore = NERO;
}

nodo_rb_t *Trova_Nodo (char *string) {
    nodo_rb_t *curr = r_p_ammesse;
    long int chiave = string[0] * 474552 + string[1] * 6084 + string[2] * 78 + string[3];
    short int res;

    while (curr != NIL) {
        if (curr->chiave == chiave) {
            res = strcmp (curr->parola, string);

            if (res == 0) {
                return curr;
            }
            else if (res > 0) {
                curr = curr->sinistro;
            }
            else {
                curr = curr->destro;
            }
        }
        else if (curr->chiave > chiave) {
            curr = curr->sinistro;
        }
        else {
            curr = curr->destro;
        }
    }

    return NIL;
}

void LeftRotate (nodo_rb_t **radice, nodo_rb_t *da_ruotare) {
    nodo_rb_t *helper = da_ruotare->destro, *a = da_ruotare->padre;
    da_ruotare->destro = helper->sinistro;

    if (helper == NIL) {
        return;
    }

    if (helper->sinistro != NIL) {
        helper->sinistro->padre = da_ruotare;
    }

    helper->padre = a;

    if (a == NIL) {
        *radice = helper;
    }
    else if (da_ruotare == a->sinistro) {
        a->sinistro = helper;
    }
    else {
        a->destro = helper;
    }

    helper->sinistro = da_ruotare;
    da_ruotare->padre = helper;
}

void RightRotate (nodo_rb_t **radice, nodo_rb_t *da_ruotare) {
    nodo_rb_t *helper = da_ruotare->sinistro, *a = da_ruotare->padre;
    da_ruotare->sinistro = helper->destro;

    if (helper == NIL) {
        return;
    }

    if (helper->destro != NIL) {
        helper->destro->padre = da_ruotare;
    }

    helper->padre = a;

    if (da_ruotare->padre == NIL) {
        *radice = helper;
    }
    else if (da_ruotare == a->destro) {
        a->destro = helper;
    }
    else {
        a->sinistro = helper;
    }

    helper->destro = da_ruotare;
    da_ruotare->padre = helper;
}

lista_confronto *Cerca_Vincolo (lista_confronto *testa, char c) {
    lista_confronto *curr = testa;

    while (curr != NULL) {
        if (curr->carattere == c) {
            return curr;
        }
        else if ((curr->next != NULL) && (curr->next->carattere > c)) {
            break;
        }

        curr = curr->next;
    }

    return NULL;
}

void Inserisci_Vincolo (lista_confronto **head, lista_confronto *ins) {
    //la lista è vuota o l'elemento va inserito in testa alla lista
    if ((*head == NULL) || ((*head)->carattere > ins->carattere))  {
        ins->next = (*head);
        *head = ins;
        return;
    }
    //l'elemento va inserito in mezzo o alla fine della lista
    lista_confronto *curr = *head;

    while (curr->next != NULL) {
        if (curr->next->carattere > ins->carattere) {
            lista_confronto *succ = curr->next;

            curr->next = ins;
            ins->next = succ;
            return;
        }

        curr = curr->next;
    }

    //l'elemento va inserito in coda
    curr->next = ins;
    ins->next = NULL;
}

char Partita (char *parola_letta) {
    int n = 0, i;
    nodo_rb_t *diz;
    char comando, rif[k + 1];
    short int occ_rif [NCAR];

    esistono_vincoli = false;
    Ripristina_Filtri (&vincoli_p);

    memset (jolly, '/', k * sizeof (char));

    comando = Lettura (2, parola_letta);

    while (comando != 'C') {
        if (comando == 'E') {
            return 'E';
        }
        else if (comando == 'S') {
            Stampa_Filtrate (ltrue);
            comando = 'P';
        }
        else if (comando == 'I') {
            comando = Inserimento ();
        }
        else {
            return 'P';
        }
    }

    memcpy(rif, parola_letta, k);
    rif[k] = '\0';
    Conta_Totali (rif, occ_rif);

    //leggo il numero di tentativi
    if (fgets (lettura, BUFSIZ, stdin) == NULL) {
        return 'E';
    }
    sscanf (lettura, "%d", &n);

    //leggo le parole per i tentativi
    for (i = 0; i < n; i++) {
        comando = 'F';

        while (comando != 'C') {
            if (comando == 'E') {
                return 'E';
            }
            else if (comando == 'S') {
                Stampa_Filtrate (r_p_ammesse);
                comando = 'F';
            }
            else if (comando == 'I') {
                comando = Inserimento ();
            }
            else if (comando == 'P') {
                return 'P';
            }
            else {
                comando = Lettura (2, parola_letta);
            }
        }

        diz = Trova_Nodo (parola_letta);

        if (diz != NIL) {
            //leggo la parola vincente
            if (strncmp (rif, diz->parola, k) == 0) {
                puts ("ok");
                return 'T';
            }

            Confronto (rif, diz, occ_rif);
        }
        else {
            puts ("not_exists");
            i--;
        }
    }

    puts ("ko");
    return 'T';
}

char Lettura (int comando, char *parola_letta) {
    //ho terminato una partita con un ok o un ko
    if (comando == 0) {
        //leggo finchè non trovo un "+...."
         do {
            if (fgets (lettura, BUFSIZ, stdin) == NULL) {
                //comando per dare errore
                return 'E';
            }
        } while (lettura [0] != '+');

        //comando stampa_filtrate
        if (lettura [1] == 's') {
            return 'S';
        }
        //comando nuova_partita
        else if (lettura [1] == 'n') {
            return 'P';
        }
        //comando inserisci_inizio
        else if (lettura [11] == 'i') {
            return 'I';
        }
        //comando inserisci_fine
        else {
            return 'F';
        }
    }
    //sono all'inizio del gioco oppure in mezzo a una partita, dovrei leggere delle parole
    else {
        if (fgets (lettura, BUFSIZ, stdin) == NULL) {
            return 'E';
        }

        if (lettura [0] == '+') {
            if (lettura [1] == 's') {
                return 'S';
            }
            else if (lettura [1] == 'n') {
                return 'P';
            }
            else if (lettura [11] == 'i') {
                return 'I';
            }
            else {
                return 'F';
            }
        }
        else {
            //ritorno la parola letta
            memcpy (parola_letta, lettura, k);
            parola_letta[k] = '\0';
            return 'C';
        }
    }
}

char Inserimento () {
    char parola [k + 1], comando = Lettura (2, parola);

    while (comando == 'C') {
        Inserisci_Ripara (&r_p_ammesse, Inserisci_Nodo (&r_p_ammesse, parola));
        comando = Lettura (2, parola);
    }

    return comando;
}

void Stampa_Filtrate (nodo_rb_t *radice) {
    if (radice->sinistro != NIL) {
        Stampa_Filtrate (radice->sinistro);
    }

    if (radice->filtro == true) {
        puts (radice->parola);
    }

    if (radice->destro != NIL) {
        Stampa_Filtrate (radice->destro);
    }
}

void Ripristina_Filtri (lista_confronto **head) {
    //resetto il vettore dei caratteri
    int i;

    for (i = 0; i < NCAR; i++) {
        if (vett_car[i].presente == 1) {
            free (vett_car[i].vett_pos);
        }

        vett_car[i].presente = 0;
    }

    //resetto la lista dei caratteri necessari
    lista_confronto *prec_conf;

    while (*head != NULL) {
        prec_conf = *head;
        *head = (*head)->next;
        free (prec_conf);
    }

    nodo_rb_t *curr = lfalse, *prec = NULL;

    while (curr != NULL) {
        contatore++;
        curr->filtro = true;
        prec = curr;
        curr = curr->next;
    }

    if (prec != NULL) {
        prec->next = ltrue;
        ltrue = lfalse;
        lfalse = NULL;
    }
}

void Confronto (char *rif, nodo_rb_t *letta, short int *occ_rif) {
    char conf [k + 1], carattere, mychar;
    int i, j, occ, ind, l_occ, r_occ, indice, myocc;
    conf [k] = '\0';
    lista_confronto *trova;

    mod_jolly = false;
    esistono_vincoli = true;

    //inizializzo il vettore confronto
    memset (conf, '-', k * sizeof (char));

    for (i = k - 1; i >= 0; i--) {
        carattere = letta->parola[i];
        indice = carattere;

        for (j = 0; j < k; j++) {
            if (rif[j] == carattere) {
                break;
            }
        }

        if (conf[i] == '-') {
            if (j != k) {
                l_occ = Conta_Occ (letta->parola, carattere);
                r_occ = occ_rif[indice];

                if (l_occ > r_occ) {
                    occ = r_occ;
                }
                else {
                    occ = -l_occ;
                }

                trova = Cerca_Vincolo (vincoli_p, carattere);
                if (trova == NULL) {
                    lista_confronto *new = (lista_confronto *) malloc (sizeof (lista_confronto));
                    new->carattere = carattere;
                    new->occorrenze = occ;
                    new->next = NULL;
                    new->fake = short_p;
                    short_p = new;

                    Inserisci_Vincolo (&vincoli_p, new);
                }
                else {
                    myocc = trova->occorrenze;

                    if ((myocc <= 0) && ((occ > 0) || (occ < myocc))) {
                        trova->occorrenze = occ;
                        trova->fake = short_p;
                        short_p = trova;
                    }
                }

                ind = i;

                while (l_occ > 0) {
                    mychar = letta->parola[ind];

                    if (mychar == carattere) {
                        if (mychar == rif[ind]) {
                            conf[ind] = '+';
                            mod_jolly = true;
                            jolly[ind] = mychar;
                            l_occ--;
                            r_occ--;
                        }
                        else {
                            if (l_occ > r_occ) {
                                conf[ind] = '/';
                                l_occ--;
                            }
                            else {
                                conf[ind] = '|';
                                r_occ--;
                                l_occ--;
                            }

                            if (vett_car[indice].presente == 0) {
                                vett_car[indice].presente = 1;
                                vett_car[indice].vett_pos = (short int *) malloc (k * sizeof (short int));
                                memset (vett_car[indice].vett_pos, 0, k * sizeof (short int));
                            }

                            if (vett_car[indice].vett_pos[ind] == 0) {
                                vett_car[indice].vett_pos[ind] = -1;
                            }

                        }
                    }

                    ind--;
                }
            }
            else {
                conf[i] = '/';

                if (vett_car[indice].presente == 0) {
                    vett_car[indice].presente = -1;
                }

            }
        }
    }

    Visita_Vincoli ();

    puts (conf);
    printf ("%d\n", contatore);
}

void Visita_Vincoli () {
    nodo_rb_t *curr = ltrue, *prec = NULL, *succ;
    int i, occ, myvet, myocc;
    char myjol;

    while (curr != NULL) {
        succ = curr->next;

        if (mod_jolly == true) {
            for (i = 0; i < k; i++) {
                myjol = jolly[i];

                if (myjol != '/') {
                    if (curr->parola[i] != myjol) {
                        curr->filtro = false;
                        goto OUT;
                    }
                }
                else {
                    myvet = vett_car[(int) curr->parola[i]].presente;

                    if ((myvet == -1) || ((myvet == 1) && (vett_car[(int) curr->parola[i]].vett_pos[i] == -1))) {
                        curr->filtro = false;
                        goto OUT;
                    }
                }
            }
        }
        else {
            //verifico con la tabella
            for (i = 0; i < k; i++) {
                myvet = vett_car[(int) curr->parola[i]].presente;

                if ((myvet == -1) || ((myvet == 1) && (vett_car[(int) curr->parola[i]].vett_pos[i] == -1))) {
                    curr->filtro = false;
                    goto OUT;
                }
            }
        }

        //verifico se ho tutte le lettere della parola
        lista_confronto *curr_v = short_p;

        while (curr_v != NULL) {
            occ = Conta_Occ (curr->parola, curr_v->carattere);
            myocc = curr_v->occorrenze;

            if (occ == 0) {
                curr->filtro = false;
                goto OUT;
            }
            else {
                if (((myocc > 0) && (occ != myocc)) || ((myocc < 0) && (occ < -myocc))) {
                    curr->filtro = false;
                    goto OUT;
                }
            }

            curr_v = curr_v->fake;
        }

        prec = curr;
        curr = succ;
        continue;

        OUT:
        contatore--;

        if (curr == ltrue) {
            ltrue = curr->next;
        }
        else {
            prec->next = curr->next;
        }

        curr->next = lfalse;
        lfalse = curr;
        curr = succ;
    }

    lista_confronto *temp = short_p;
    while (short_p != NULL) {
        short_p = short_p->fake;
        temp->fake = NULL;
        temp = short_p;
    }
}

int Conta_Occ (char *parola, char carattere) {
    int i, occ = 0;

    for (i = 0; i < k; i++) {
        if (parola [i] == carattere) {
            occ++;
        }
    }

    return occ;
}

void Conta_Totali (char *parola, short int *vett) {
    int i, indice;
    memset (vett, 0, NCAR * sizeof (short int));

    for (i = 0; i < k; i++) {
        indice = parola[i];
        vett[indice]++;
    }
}