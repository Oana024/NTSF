#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

#define PORT 1111

//extern int errno;

char * conv_addr (struct sockaddr_in address){
	static char str[25];
	char port[7];

	strcpy (str, inet_ntoa (address.sin_addr));	

	bzero (port, 7);
	sprintf (port, ":%d", ntohs (address.sin_port));	
	strcat (str, port);
	return (str);
}

char table[15][30], table_copy[15][30], line_table[450];
int used[4] = {0, 0, 0, 0}, Clasament[4] = {0, 0, 0, 0};

void CopyTable(){
    int i, j;

    for(i = 0; i <= 14; i++)
        for(j = 0; j <= 29; j++)
            table_copy[i][j] = table[i][j];
    
    table_copy[3][4] = table_copy[3][6] = table_copy[4][4] = table_copy[4][6] = '_';
    table_copy[11][4] = table_copy[11][6] = table_copy[12][4] = table_copy[12][6] = '_';
    table_copy[3][22] = table_copy[3][24] = table_copy[4][22] = table_copy[4][24] = '_';
    table_copy[11][22] = table_copy[11][24] = table_copy[12][22] = table_copy[12][24] = '_';
}

void CreateTable(){
    int i, j;

    for(i = 0; i <= 14; i++)
        for(j = 0; j <= 29; j++)
            table[i][j] = ' ';

    for(i = 1; i <= 14; i++)
        table[i][0] = table[i][29] = '|';

    for(i = 1; i <= 28; i++)
        table[0][i] = table[14][i] = '_';

     for(i = 10;  i <= 18; i++)
        if(i % 2 == 0)
            table[1][i] = table[13][i] = '_';
    
    for(i = 5;  i <= 9; i++)
        table[i][2] = table[i][26] = '_';

    for(i = 4; i <= 10; i += 2)
        table[5][i] =  table[5][i + 14] = table[9][i] =  table[9][i + 14] = '_';

    for(i = 1; i <= 3; i++)
        table[i + 1][10] =  table[i + 9][10] = table[i + 1][18] = table[i + 9][18] = '_';

    for(i = 1; i <= 4; i++){
        table[i + 1][14] = 'g';
        table[i + 8][14] = 'v';
    }

    for(i = 4; i <= 10; i++)
        if(i % 2 == 0){
            table[7][i] = 'a';
            table[7][i + 14] = 'r';
    }

    table[3][4] = table[3][6] = table[4][4] = table[4][6] = 'A';
    table[11][4] = table[11][6] = table[12][4] = table[12][6] = 'V';
    table[3][22] = table[3][24] = table[4][22] = table[4][24] = 'G';
    table[11][22] = table[11][24] = table[12][22] = table[12][24] = 'R';
   
    table[1][18] = table[5][2] = table[9][26] = table[13][10] = '*';

    CopyTable();
}

void PrintTable(){
    int i, j;

    for(i = 0; i <= 14; i++){
        for(j = 0; j <= 29; j++)
            cout << table[i][j];
        cout << '\n';
    }
}

void FromMatToStr(){
	int i, j, cnt = 0;
    
    bzero(line_table, 450);
	for(i = 0; i <= 14; i++){
        for(j = 0; j <= 29; j++)
			line_table[cnt++] = table[i][j];
		}

	line_table[cnt] = '\0';
}

void FromStrToMat(){
	int i, l = 0, c = 0;
	for(i = 0; i <= 450; i++){
        if(i % 30 == 0 && i){
			l ++;
			c = 0;
		}
		table[l][c] = line_table[i];
		c ++;
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int moveGi[50] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 
                -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 1, 1, 1, 1};

int moveGj[50] = {0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, -2, -2, -2, -2, 
                    0, 0, 0, 0, -2, -2 , -2, -2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int moveRi[50] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 
                    0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};

int moveRj[50] = {-2, -2, -2, -2, 0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, -2, -2, -2, -2, 
                    0, 0, 0, 0, 2, 2 , 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, -2, -2, -2, -2};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int moveVi[50] = {-1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 
                    1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, -1, -1, -1, -1};

int moveVj[50] = {0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, 2, 2 , 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 
                    0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, -2, -2, 0, 0, 0, 0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int moveAi[50] = {0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 
                    0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0};

int moveAj[50] = {2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 
                    -2, -2, -2, -2, 0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, -2, -2, -2, -2, 0, 0, 2, 2, 2, 2};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Final{
    int x, y;
};
struct piese{
    int xi, yi, xs, ys, x, y, poz;
};
struct jucatori{
    char symbol;
    piese piesa[4];
    Final pozitie_final[4];
};

jucatori player[4] = {
    {//G
        'G',
        piese{3, 22, 1, 18, 3, 22, -1},
        piese{3, 24, 1, 18, 3, 24, -1},
        piese{4, 22, 1, 18, 4, 22, -1},
        piese{4, 24, 1, 18, 4, 24, -1},
        Final{2, 14},
        Final{3, 14},
        Final{4, 14},
        Final{5, 14}
    },
    {//R
        'R',
        piese{11, 22, 9, 26, 11, 22, -1},
        piese{11, 24, 9, 26, 11, 24, -1},
        piese{12, 22, 9, 26, 12, 22, -1},
        piese{12, 24, 9, 26, 12, 24, -1},
        Final{7, 18},
        Final{7, 20},
        Final{7, 22},
        Final{7, 24}
    },
    {//V
        'V',
        piese{11, 4, 13, 10, 11, 4, -1},
        piese{11, 6, 13, 10, 11, 6, -1},
        piese{12, 4, 13, 10, 12, 4, -1},
        piese{12, 6, 13, 10, 12, 6, -1},
        Final{9, 14},
        Final{10, 14},
        Final{11, 14},
        Final{12, 14}
    },
    {//A
        'A',
        piese{3, 4, 5, 2, 3, 4, -1},
        piese{3, 6, 5, 2, 3, 6, -1},
        piese{4, 4, 5, 2, 4, 4, -1},
        piese{4, 6, 5, 2, 4, 6, -1},
        Final{7, 4},
        Final{7, 6},
        Final{7, 8},
        Final{7, 10}
    }
};


struct{
	char nume[64];
	int punctaj;
	int id; //0 - galben | 1 - rosu | 2 - verde | 3 - albastru
    int nou = 1;
}jucatorul[4];

int movei[50], movej[50];

int pozitie_jucator = 1, nr_clienti = 0;

void Punctaje(){
    int i, puncte[5];
    puncte[1] = 25 * nr_clienti;
    puncte[2] = 15 * nr_clienti;
    puncte[3] = 5 * nr_clienti;
    puncte[4] = 1 * nr_clienti;
    for(i = 0; i < nr_clienti; i++){
        if(Clasament[i] >= 0)
            jucatorul[i].punctaj = puncte[Clasament[i]];
    }
}

void Win(){
    int i, j, nr_piese;
    for(i = 0; i < 4; i++){
        if(Clasament[i] == 0){//daca jucatorul nu a terminat jocul
            nr_piese = 0;
            for(j = 0; j < 4; j++)
                if(table[player[i].pozitie_final[j].x][player[i].pozitie_final[j].y] == player[i].symbol)
                    nr_piese++;

            if(nr_piese == 4){
                Clasament[i] = pozitie_jucator;
                pozitie_jucator ++;
            }
        }
    }
}

bool CanMove(int p, int z, int mutarei[50], int mutarej[50]){
    int newX, newY;

    for(int i = 0; i < 4; i++)
        if(z == 6 && player[p].piesa[i].poz == -1)
            return true;

    for(int i = 0; i < 4; i++)
        if((player[p].piesa[i].poz != -1) && (player[p].piesa[i].poz + z <= 50)){
            newX = player[p].piesa[i].x;
            newY = player[p].piesa[i].y;
            for(int j = player[p].piesa[i].poz; j < player[p].piesa[i].poz + z; j++){
                newX += mutarei[j];
                newY += mutarej[j];
            }
            if(strchr("1234", table[newX][newY]) == 0)
                return true;
        }

    return false;
}

bool VerificarePozitie(int jucator, int p, int newX, int newY, int zar){
    int i, j;
    if(strchr("1234", table[newX][newY]) == 0){
        if(strchr("AGVR", table[newX][newY]) != 0){
            for(i = 0; i < 4; i++)
                if(player[i].symbol == table[newX][newY]){
                    for(j = 0; j < 4; j++)
                        if(player[i].piesa[j].x == newX && player[i].piesa[j].y == newY){
                            player[i].piesa[j].x = player[i].piesa[j].xi;
                            player[i].piesa[j].y = player[i].piesa[j].yi;
                            player[i].piesa[j].poz = -1;
                            table[player[i].piesa[j].x][player[i].piesa[j].y] = player[i].symbol;
                        }
                }
        }

        player[jucator].piesa[p - 1].x = newX;
        player[jucator].piesa[p - 1].y = newY;
        player[jucator].piesa[p - 1].poz += zar;
        return true;
    }
    return false;
}

int PlayerTurn(int jucator, int fd);

struct{
    char username[64];
    int puncte;
}ClasamentAllTime[1001];

void ActualizeazaClasament(){
    int loc = 0, i;
    ifstream fin("punctaje.txt");
    while(fin >> ClasamentAllTime[loc].username >> ClasamentAllTime[loc].puncte){
       
        for(i = 0; i < 4; i++)
            if(strcmp(jucatorul[i].nume, ClasamentAllTime[loc].username) == 0){
                ClasamentAllTime[loc].puncte += jucatorul[i].punctaj;
                jucatorul[i].nou = 0;
            }
        loc ++;
    }

    for(i = 0; i < nr_clienti; i++)
        if(jucatorul[i].nou == 1){
            strcpy(ClasamentAllTime[loc].username, jucatorul[i].nume);
            ClasamentAllTime[loc].puncte = jucatorul[i].punctaj;
            loc ++;
        }

    cout << "Clasamentul tuturor jucatorilor: \n";

    for(i = 0; i < loc; i++)
        cout << ClasamentAllTime[i].username << " " << ClasamentAllTime[i].puncte << '\n';

    ofstream fout("punctaje.txt");
    for(i = 0; i < loc; i++)
        fout << ClasamentAllTime[i].username << " " << ClasamentAllTime[i].puncte << '\n';
    
}

int main (){
	struct sockaddr_in server;	
	struct sockaddr_in from;
	fd_set readfds;	
	fd_set actfds;
	struct timeval tv;
	int sd, client;	
	int optval=1; 
	int fd;	
	int nfds;
	unsigned int len;

	int nr_max_clienti = 0, in_game = 0, joc = 1;
	int playersFD[4];
    char mesaj[100];

    while(!(nr_max_clienti > 1 && nr_max_clienti <= 4)){
        printf("Cati clienti acceptam?(2-4) ");
        scanf("%d", &nr_max_clienti);
    }

	if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
		perror ("[server] Eroare la socket().\n");
		return errno;
	}

	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

	bzero (&server, sizeof (server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	server.sin_port = htons (PORT);

	if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1){
		perror ("[server] Eroare la bind().\n");
		return errno;
	}

	if (listen (sd, 5) == -1){
		perror ("[server] Eroare la listen().\n");
		return errno;
	}

	FD_ZERO (&actfds);
	FD_SET (sd, &actfds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	nfds = sd;

	printf ("[server] Asteptam la portul %d...\n", PORT);
	fflush (stdout);
			
	while (joc == 1){
      	bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

    	if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0){
	 		perror ("[server] Eroare la select().\n");
	  		return errno;
		}

     	if (FD_ISSET (sd, &readfds)){
	  		len = sizeof (from);
	  		bzero (&from, sizeof (from));

	  		client = accept (sd, (struct sockaddr *) &from, & len);

	  		if (client < 0){
	      		perror ("[server] Eroare la accept().\n");
	     		continue;
	   		}
			else{
					char msg[100];

					if(nr_clienti < nr_max_clienti){

                        bzero(msg, 100);
						strcpy(msg, "Asteptam ceilalti jucatori...");
						if (write (client, msg, 100) <= 0){
							perror ("[Server]Eroare la write() spre client.\n");
							return errno;
						}

						playersFD[nr_clienti] = client;
						nr_clienti ++;
					}
					else{
                        bzero(msg, 100);
						strcpy(msg, "Jocul a inceput deja!");
						if (write (client, msg, 100) <= 0){
							perror ("[Server]Eroare la write() spre client.\n");
							return errno;
						}
					}
			}

			if (nfds < client) 
				nfds = client;
			
			FD_SET (client, &actfds);

			printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));
			fflush (stdout);
		}

		if(in_game == 1){
			close(client);
			FD_CLR(client, &actfds);
		}

		if(nr_clienti == nr_max_clienti && in_game == 0){
			in_game = 1;

            CreateTable();

			int pid = fork();
			if(pid == -1){
				perror("Eroare la fork\n");
				return errno;
			}
			else if(pid == 0){
				int nr_jucator = 0;
				while(nr_jucator < nr_clienti){
					int ok = 0;
					int fd = playersFD[nr_jucator], bytes, k = -1;
					char culoare[101], rasp[101], culoare_jucator[1], msg[100];

					strcpy(msg, "Incepe jocul! A venit randul sa alegeti username-ul si culoarea cu care veti juca!");
					if (write (fd, msg, 100) <= 0){
						perror ("[Server]Eroare la write() spre client.\n");
						return errno;
					}

                    bzero(jucatorul[nr_jucator].nume, 64);
                    if (read (fd, jucatorul[nr_jucator].nume, 64) <= 0){
                        perror ("[Server]Eroare la read() spre client.\n");
                        return errno;
                    }

                    cout << "S-a conectat " << jucatorul[nr_jucator].nume << '\n';

                    while(ok == 0){

                        if (read (fd, culoare_jucator, 1) < 0){
                            perror ("[server] Eroare la read() de la client.\n");
                            return errno;
                        }

                        if(strchr("RGVA", culoare_jucator[0]) != 0){
                            if(culoare_jucator[0] == 'G'){
                                k = 0;
                                strcpy(culoare, "galben");
                            }
                            else
                                if(culoare_jucator[0] == 'R'){
                                    k = 1;
                                    strcpy(culoare, "rosu");
                                }
                                else
                                    if(culoare_jucator[0] == 'V'){
                                        k = 2;
                                        strcpy(culoare, "verde");
                                    }
                                    else 
                                        if(culoare_jucator[0] == 'A'){
                                            k = 3;
                                            strcpy(culoare, "albastru");
                                        }
							
							if(used[k] == 0){
								ok = 1;
								strcpy(rasp, "Ati ales culoarea ");
								strcat(rasp, culoare);
								used[k] = 1;

								jucatorul[nr_jucator].id = k;
							}
							else{
								strcpy(rasp, "Culoarea a fost deja aleasa!");
							}
                        }
                        else{
                            strcpy(rasp, "Nu ati ales o culoare valida!");
                        }

                        printf("[server]Jucatorul %d a ales culoarea %s\n", nr_jucator + 1, culoare);

                        if (write (fd, rasp, 100) <= 0){
                            perror ("[Server]Eroare la write() spre client.\n");
                            return errno;
                        }
                    }
					nr_jucator ++;
                }//am ales culorile
				//incepe jocul

				int avem_jucatori = nr_max_clienti, i, val;
				while(avem_jucatori > 1){
					for(i = 0; i < nr_max_clienti; i++)
						if(Clasament[i] == 0){
                            strcpy(mesaj, "Este randul tau!");
                            if (write (playersFD[i], mesaj, 100) <= 0){
                                perror ("[Server]Eroare la write() spre client.\n");
                                return errno;
                            }

							val = PlayerTurn(jucatorul[i].id, playersFD[i]);

                            if(val == -1)
                                Clasament[i] = -1;

                            if(Clasament[i] != 0){
                                close(playersFD[i]);
			                    FD_CLR(playersFD[i], &actfds);
                            }
						}
					
					avem_jucatori = 0;
					for(i = 0; i < nr_max_clienti; i++)
						if(Clasament[i] == 0)
							avem_jucatori ++;
                    //cout << avem_jucatori << '\n';
				}
                for(i = 0; i < nr_max_clienti; i++)
					if(Clasament[i] == 0){
                        strcpy(mesaj, "Nu ai reusit sa termini jocul!");
                        if (write (playersFD[i], mesaj, 100) <= 0){
                            perror ("[Server]Eroare la write() spre client.\n");
                            return errno;
                        }
                        close(playersFD[i]);
			            FD_CLR(playersFD[i], &actfds);
                    }

                cout << "S-a terminat jocul\n";
                Punctaje();
                ActualizeazaClasament();

                return 0;
			}
		}	
    }				
}	


int PlayerTurn(int jucator, int fd){
    int i, zar, p, ok = 0, j, pozX, pozY, in = 0, newX, newY;
    char c = ' ', var, z[1], rasp[100], ps[1];
    
    for(i = 0 ; i < 4; i++){
        table[player[jucator].piesa[i].x][player[jucator].piesa[i].y] = i + '1';
        if(player[jucator].piesa[i].poz >= 0)
            in ++;
    }

	FromMatToStr();

    //trimit matricea
    if (write (fd, line_table, 450) <= 0){
        perror ("[Server]Eroare la write() spre client.\n");
        return errno;
    }

    if(jucator == 0)
        for(i = 0; i < 50; i++){
            movei[i] = moveGi[i];
            movej[i] = moveGj[i];
        }
    else
        if(jucator == 1)
            for(i = 0; i < 50; i++){
                movei[i] = moveRi[i];
                movej[i] = moveRj[i];
            }
        else
            if(jucator == 2)
                for(i = 0; i < 50; i++){
                    movei[i] = moveVi[i];
                    movej[i] = moveVj[i];
                }
            else
                if(jucator == 3)
                    for(i = 0; i < 50; i++){
                        movei[i] = moveAi[i];
                        movej[i] = moveAj[i];
                    }
    
    //primesc valoarea zarului
    if (read (fd, &zar, 4) < 0){
        perror ("[server] Eroare la read() de la client.\n");
        return errno;
    }

    if(zar == 7)
        return -1;

    if(CanMove(jucator, zar, movei, movej)){
        strcpy(rasp, "Se poate muta");
        if (write (fd, rasp, 100) <= 0){
            perror ("[Server]Eroare la write() spre client.\n");
            return errno;
        }

        while(ok == 0){

            if (read (fd, &p, 4) < 0){
                perror ("[server] Eroare la read() de la client.\n");
                return errno;
            }
                
            pozX = player[jucator].piesa[p - 1].x;
            pozY = player[jucator].piesa[p - 1].y;

            if(zar == 6){
                if(player[jucator].piesa[p - 1].poz < 0){ // scot piesa

                    newX = player[jucator].piesa[p - 1].xs;
                    newY = player[jucator].piesa[p - 1].ys;

                    if(VerificarePozitie(jucator, p, newX, newY, 1)){
                        ok = 1;
                    }
                }
                else{
                    if(player[jucator].piesa[p - 1].poz + 6 <= 50){ //mut piesa 6 pozitii
                        newX = player[jucator].piesa[p - 1].x;
                        newY = player[jucator].piesa[p - 1].y;
                        for(j = player[jucator].piesa[p - 1].poz; j < player[jucator].piesa[p - 1].poz + zar; j++){
                            newX += movei[j];
                            newY += movej[j];
                        }

                        if(VerificarePozitie(jucator, p, newX, newY, zar)){
                            ok = 1;
                        }
                    }
                }
            }
            else{
                if(player[jucator].piesa[p - 1].poz >= 0){
                    if(player[jucator].piesa[p - 1].poz + zar <= 50){

                        newX = player[jucator].piesa[p - 1].x;
                        newY = player[jucator].piesa[p - 1].y;
                        for(j = player[jucator].piesa[p - 1].poz; j < player[jucator].piesa[p - 1].poz + zar; j++){
                            newX += movei[j];
                            newY += movej[j];
                        }

                        if(VerificarePozitie(jucator, p, newX, newY, zar)){
                            ok = 1;
                        }
                        
                    }
                }
            }

            if(ok == 0){
                strcpy(rasp, "Alegeti alta piesa!");
                if (write (fd, rasp, 100) <= 0){
                    perror ("[Server]Eroare la write() spre client.\n");
                    return errno;
                }
            }
            else{
                    table[pozX][pozY] = table_copy[pozX][pozY];

                    table[player[jucator].piesa[p - 1].x][player[jucator].piesa[p - 1].y] = p + '0';

                    for(i = 0 ; i < 4; i++)
                        table[player[jucator].piesa[i].x][player[jucator].piesa[i].y] = player[jucator].symbol;

                    Win();

                    if(Clasament[jucator] != 0){
                        strcpy(rasp, "Mutare efectuata cu succes si ai terminat jocul!");
                    }
                    else{
                        strcpy(rasp, "Mutare efectuata cu succes!");
                    }

                    if (write (fd, rasp, 100) <= 0){
                        perror ("[Server]Eroare la write() spre client.\n");
                        return errno;
                    }

                    FromMatToStr();

                    if (write (fd, line_table, 450) <= 0){
                        perror ("[Server]Eroare la write() spre client.\n");
                        return errno;
                    }
                }
        }
    }
    else{
        for(i = 0 ; i < 4; i++)
            table[player[jucator].piesa[i].x][player[jucator].piesa[i].y] = player[jucator].symbol;

        strcpy(rasp, "Nu exista mutare posibila!");
        if (write (fd, rasp, 100) <= 0){
            perror ("[Server]Eroare la write() spre client.\n");
            return errno;
        }
    }

    if(Clasament[jucator] != 0){
        cout << "Jucatorul " << jucator << " a terminat jocul\n";
    }

    return 1;
}