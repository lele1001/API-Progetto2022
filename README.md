# ProgetttoAPI_2022
Progetto di Algoritmi e Principi dell'Informatica A.A. 2021-2022

## WordChecker

L'obiettivo è realizzare un sistema che controlla la corrispondenza tra le lettere di 2 parole, che sono sequenze di simboli di ugual lunghezza che possono essere:
- caratteri alfabetici minuscoli (a-z), 
- caratteri alfabetici maiuscoli (A-Z), 
- cifre numeriche (0-9),
- i simboli - (trattino) e _ ("underscore")

Il sistema legge da _stdin_ una sequenza di informazioni e istruzioni, e produce delle stringhe su _stdout_. Più precisamente, il sistema legge:
- un valore k, che indica la lunghezza delle parole
- una sequenza arbitraria di parole ammissibili di lunghezza k (non contiene duplicati)

A quel punto, viene letta da _stdin_ una sequenza di "partite", marcate dal comando `+nuova_partita`. Le sequenze di stringhe in input per ogni partita sono fatte nel seguente modo:
- parola di riferimento (di lunghezza k caratteri), che appartiene all'insieme di parole ammissibili
- numero n massimo di parole da confrontare con la parola di riferimento
- sequenza di parole (ognuna di k caratteri) da confrontare con la parola di riferimento

Ogni tanto, nella sequenza di stringhe in input, può comparire il comando `+stampa_filtrate` ed il programma deve produrre su _stdout_, in ordine 
lessicografico, l'insieme delle parole ammissibili che sono compatibili con i vincoli appresi fino a quel momento, scritte una per riga. I vincoli appresi riguardano, per ogni simbolo:
1. se il simbolo non appartiene a r
2. posti in cui quel simbolo deve comparire in r
3. posti in cui quel simbolo non può comparire in r
4. numero minimo di volte che il simbolo compare in r
5. numero esatto di volte che il simbolo compare in r

Dove il vincolo 5 è più forte del 4, l'ordine dei simboli (per stabilire l'ordine lessicografico) è quello standard ASCII e, dopo ogni confronto, 
il programma deve stampare su _stdout_ il numero di parole ammissibili e compatibili con i vincoli appresi tranne nel caso di un confronto con 
esito `not_exists`.

Sia durante una partita, che tra una partita e l'altra, possono comparire i comandi `+inserisci_inizio` e `+inserisci_fine` che racchiudono tra di 
loro una sequenza di nuove parole ammissibili da aggiungere.

Per ogni parola letta p, i cui caratteri sono p[1] ... p[k], che viene confrontata con la parola di riferimento r, i cui caratteri sono r[1] ... 
r[k], il programma scrive su stdout una sequenza di k caratteri res[1] ... res[k] che per ogni 1 ≤ i ≤ k, res[i] è 
• il carattere + se p[i] = r[i], quindi p[i] è "in posizione corretta"
• il carattere / p[i] non compare da nessuna parte in r
• il carattere | p[i] compare in r, ma non in posizione i-esima

Se in r compaiono n_i istanze di p[i], se c_i ≤ n_i sono le istanze corrette di p[i], e se prima di p[i] ci sono almeno (n_i - c_i) caratteri 
uguali a p[i] in posizione scorretta, allora res[i] deve essere / invece che |. Se da _stdin_ viene letta una parola che non appartiene all'insieme di quelle ammissibili, il programma scrive su stdout la stringa `not_exists`. Se invece viene letta la parola r = p, il programma scrive `ok` (senza stampare il confronto) e la partita termina.

Se, dopo avere letto n parole ammissibili (con n numero massimo di parole da confrontare con r), nessuna di queste era uguale a r, il programma 
scrive `ko` (dopo avere stampato il risultato del confronto dell'ultima parola), e la partita termina.

Dopo che la partita è finita:
• ci potrebbe essere l'inserimento di nuove parole ammissibili
• se in input c'è il comando `+nuova_partita`, ha inizio una nuova partita
