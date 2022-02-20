#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

using namespace std;

//extern int errno;

int port;

int Zar(){
    srand (time(NULL));
    int valoare_zar = rand() % 6 + 1;
    return valoare_zar;
}

char line[450], table[15][30];

void FromMatToStr(){
	int i, j, cnt = 0;
	for(i = 0; i <= 14; i++){
        for(j = 0; j <= 29; j++)
			line[cnt] = table[i][j];
			cnt ++;
		}
	line[cnt] = '\0';
}

void FromStrToMat(){
	int i, l = 0, c = 0;
	for(i = 0; i <= 450; i++){
        if(i % 30 == 0 && i){
			l ++;
			c = 0;
		}
		table[l][c] = line[i];
		c ++;
	}	
}

void PrintTable(){
    int i, j;

    for(i = 0; i <= 14; i++){
        for(j = 0; j <= 29; j++)
            cout << table[i][j];
        cout << '\n';
    }
}

int main (int argc, char *argv[]){
    int sd;
    struct sockaddr_in server;
    char msg[100], username[64];

    if (argc != 3){
        printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi (argv[2]);

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
        perror ("[client] Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = inet_addr(argv[1]);

    server.sin_port = htons (port);

    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1){
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    //primul mesaj
    bzero(msg, 100);
    if(read(sd, msg, 100) < 0){
        perror("[client] Eroare la read\n");
        return errno;
    }

    if(strstr(msg, "inceput")){//daca jocul a inceput nu mai continui
        cout << "Jocul a inceput deja\n";
        close(sd);
    }
    else{

        printf("%s\n", msg);

        //se aleg culorile
        bzero(msg, 100);
        if(read(sd, msg, 100) < 0){
            perror("[client] Eroare la read\n");
            return errno;
        }

        printf("%s\n", msg);

        cout << "Introduceti username-ul pe care vreti sa-l folositi in joc: ";
        fflush(stdout);
        bzero(username, 64);
        cin >> username;

        if (write (sd, username, 64) <= 0){
            perror ("[client]Eroare la write() spre server.\n");
            return errno;
        }

        cout << "Ati ales numele " << username << '\n';

        char culoare[1];

        int ok = 0;
        while(ok == 0){
            bzero (culoare, 1);
            printf ("Alegeti o culoare(G, R, V, A): ");
            fflush (stdout);
            //citesc culoarea
            cin >> culoare;
            
            if (write (sd, culoare, 1) <= 0){//server-ul verifica daca este valida
                perror ("[client]Eroare la write() spre server.\n");
                return errno;
            }
            
            bzero(msg, 100);
            printf("Astept raspuns de la server...\n");
            
            if(read (sd, msg, 100) < 0){//iau mesajul de la server
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("%s\n", msg);

            if(strstr(msg, "Ati ales culoarea") != 0){//daca am ales o culoare valida, am terminat
                ok = 1;
            }
        }

        //incepe jocul
        int joc = 1, z, m, val_zar;
        char c, mutare[1];
        while(joc == 1){
            //imi astept randul
            bzero(msg, 100);
            if(read(sd, msg, 100) < 0){
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf("%s\n", msg);

            if(strstr(msg, "termini")){
                joc = 0;
            }
            else{

                //citesc matricea
                bzero(line, 450);
                if(read(sd, line, 450) < 0){
                    perror ("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                
                //transform matricea
                FromStrToMat();

                //afisez matricea
                PrintTable();

                //dau cu zarul
                c = ' ';
                while(c != 'z' && c != 's'){
                    cout << "Scrieti 'z' ca sa dati cu zarul sau 's' ca sa iesiti din joc\n";
                    cin >> c;
                    fflush (stdout);
                    if(c == 'z')
                        z = Zar();
                }

                if(c == 'z')
                    printf("Zar: %d\n", z);
                else{
                        z = 7;
                        joc = 0;
                }

                //in server verific daca e mutare posibila (trimit valoarea zarului la server)
                if (write (sd, &z, 4) <= 0){
                    perror ("[client]Eroare la write() spre server.\n");
                    return errno;
                }

                if(c == 'z'){

                    //citesc raspunsul de la server
                    bzero(msg, 100);
                    if(read(sd, msg, 100) < 0){
                        perror ("[client]Eroare la read() de la server.\n");
                        return errno;
                    }
                    ok = 0;

                    if(strstr(msg, "Nu")){
                        ok = 1;
                        printf("%s\n", msg);
                    }

                    while(ok == 0){
                        //citesc mutarea
                        cout << "Ce piesa vreti sa mutati?(1, 2, 3, 4) ";
                        cin >> c;
                        fflush (stdout);

                        m = c - '0';

                        if(m >= 1 && m <= 4){
                            //trimit la server
                            if (write (sd, &m, 4) <= 0){
                                perror ("[client]Eroare la write() spre server.\n");
                                return errno;
                            }
                            //citesc raspunsul
                            bzero(msg, 100);
                            if(read(sd, msg, 100) < 0){
                                perror ("[client]Eroare la read() de la server.\n");
                                return errno;
                            }

                            printf("%s\n", msg);

                            if(strstr(msg, "succes")){
                                if(strstr(msg, "terminat"))
                                    joc = 0;
                                ok = 1;
                                //citesc matricea
                                bzero(line, 450);
                                if(read(sd, line, 450) < 0){
                                    perror ("[client]Eroare la read() de la server.\n");
                                    return errno;
                                }
                                FromStrToMat();
                                PrintTable();
                                //printf("\n%s\n", msg);
                            }
                        }
                    }
                }
            }

            //astept randul
        }

        close (sd);
    }
}