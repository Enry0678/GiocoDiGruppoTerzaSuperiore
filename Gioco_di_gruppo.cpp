//*************************************************************
//Gruppo: Ghezzo, Rossetto, Valle
//*************************************************************

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>
#include <time.h>
#include <cmath>
#include <fstream>

using namespace std;

struct Powerup {
    int xangolo;
    int yangolo;
    int xcentro;
    int ycentro;
    int size;
    bool stato;
    int tempo_attesa;
    ALLEGRO_TIMER* attivazione;
};

struct Player {
    int xcentro;
    int ycentro;
    int xangolo;
    int yangolo;
    int direzione; //diventa = 1 se il player va verso destra
    int size;
    int passo;
    int vite;
};

struct Proiettile {
    int xvero;
    int yvero;
    int xcentro;
    int ycentro;
    int passo;
    int size;
    int iniziale;
    int quanti;
};

string leggi_punteggio() {
    fstream newfile;
    string punteggio;
    newfile.open("miglior_punteggio.txt", ios::in);
    if (newfile.is_open()) {

        getline(newfile, punteggio);
        newfile.close();
    }
    return punteggio;
}

void registra_punteggio(string score) {
    fstream newfile;

    newfile.open("miglior_punteggio.txt", ios::out);
    if (newfile.is_open()) {
        newfile << score;
        newfile.close();
    }
}

int spawnproiettili(int& x, int& y, int proiettilesize, int displayx, int displayy) {
    int proiettilecasuale = rand() % 4;
    //sinistra <-- destra
    if (proiettilecasuale == 0) {
        x = displayx;

        y = rand() % (displayy - proiettilesize);

        return 0;
    }

    //sinistra --> destra
    if (proiettilecasuale == 1) {
        x = -proiettilesize;

        y = rand() % (displayy - proiettilesize);

        return 1;
    }

    //sopra --> sotto
    if (proiettilecasuale == 2) {
        y = -proiettilesize;

        x = rand() % (displayx - proiettilesize);

        return 2;
    }

    //sopra <-- sotto
    if (proiettilecasuale == 3) {
        y = displayx;

        x = rand() % (displayx - proiettilesize);

        return 3;
    }

    return 0;
}
int main()
{
    al_init();
    al_init_primitives_addon();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    //DISPLAY
    int displayx = 1500;
    int displayy = 844;

    //PLAYER
    Player player;

    player.xcentro = displayx / 2;    //coordinate centro
    player.ycentro = displayy / 2;
    player.xangolo = 0;   //coordinate angolo alto sinistro
    player.yangolo = 0;
    player.direzione = 0; //diventa = 1 se il player va verso destra
    player.size = 150;  //dimensione del lato del player
    player.passo = 3;   //velocità del player
    player.vite = 3;

    //PROIETTILE
    Proiettile proiettile[8];
    for (int i = 0; i < 8; i++) {
        proiettile[i].xvero = 0;    //coordinate angolo alto sinistro
        proiettile[i].yvero = 0;
        proiettile[i].xcentro = 0;  //coordinate centro
        proiettile[i].ycentro = 0;
        proiettile[i].passo = 5;    //velocità dei proiettili
        proiettile[i].size = 105;   //dimensione del lato dei proiettili
        proiettile[i].iniziale = 0; //cambia in base a dove parte il proiettile
        proiettile[i].quanti = 5;   //quanti proiettili ci sono nello schermo
    }

    //POWERUP VITA
    Powerup pvita;

    pvita.size = 50;    //dimensione lato powerup
    pvita.xangolo = 0;  //coordinata x angolo in alto a sinistra
    pvita.yangolo = 0;  //coordinata y angolo in alto a sinistra
    pvita.xcentro = pvita.xangolo + (pvita.size / 2);   //coordinata x centro
    pvita.ycentro = pvita.yangolo + (pvita.size / 2);   //coordinata y centro
    pvita.stato = false;    //stato del powerup: attivo o non attivo
    pvita.tempo_attesa = 0; //quanto tempo attendere prima di far spawnare il powerup o di farlo sparire
    pvita.attivazione = al_create_timer(1); //timer che indica il tempo di spawn del powerup nell'area di gioco

    //POWERUP SCUDO
    Powerup pscudo;

    pscudo.size = 50;
    pscudo.xangolo = 0;
    pscudo.yangolo = 0;
    pscudo.xcentro = 0;
    pscudo.ycentro = 0;
    pscudo.stato = false;
    pscudo.tempo_attesa = 0;
    pscudo.attivazione = al_create_timer(1);

    //PUNTEGGIO
    string punteggios = "SCORE: 0"; //punteggio
    string miglior_punteggios = "BEST SCORE: 0";    //miglio punteggio
    int miglior_punteggio = 0;

    bool menucolora = false;    //usata per colorare il pulsante start
    bool menucolora1 = false;   //usata per colorare il pulsante exit

    bool exit = true;  //usata per uscire dal gioco e tornare al menu
    bool exit1 = false; //usata per uscire del tutto dal gioco

    bool flash_stato = false; //diventa dalso se il flash non e disponibile o vero se e disponibile

    ALLEGRO_DISPLAY* display = al_create_display(displayx, displayy);
    ALLEGRO_BITMAP* playerbit[2] = { al_load_bitmap("Personaggio/player.png"), al_load_bitmap("Personaggio/player1.png") };
    ALLEGRO_BITMAP* proiettileimmagine[4] = { al_load_bitmap("Proiettile/proiettile.png"), al_load_bitmap("Proiettile/proiettile1.png"), al_load_bitmap("Proiettile/proiettile2.png"), al_load_bitmap("Proiettile/proiettile3.png") };
    ALLEGRO_BITMAP* sfondo = al_load_bitmap("Sfondo/sfondo.png");
    ALLEGRO_BITMAP* menu = al_load_bitmap("Lobby/menu.png");
    ALLEGRO_BITMAP* vite[3] = { al_load_bitmap("Powerup e vite/1_vite.png"), al_load_bitmap("Powerup e vite/2_vite.png"), al_load_bitmap("Powerup e vite/3_vite.png") };
    ALLEGRO_BITMAP* pvitebit = al_load_bitmap("Powerup e vite/powerup_vite.png");
    ALLEGRO_BITMAP* pscudobit = al_load_bitmap("Powerup e vite/powerup_scudo.png");
    ALLEGRO_BITMAP* scudo = al_load_bitmap("Powerup e vite/scudo.png");
    ALLEGRO_BITMAP* flashbit = al_load_bitmap("Powerup e vite/flash.png");
    ALLEGRO_FONT* Testo_arial = al_load_ttf_font("Text/arial.ttf", 60, 0);
    ALLEGRO_FONT* Testo_gameplay = al_load_ttf_font("Text/Gameplay.ttf", 100, 0);
    ALLEGRO_FONT* Testo_lol = al_load_ttf_font("Text/lol.ttf", 30, 0);
    ALLEGRO_TIMER* timer_punteggio = al_create_timer(1);
    ALLEGRO_TIMER* timer_difficolta = al_create_timer(1);
    ALLEGRO_TIMER* timer_scudo = al_create_timer(1);
    ALLEGRO_MOUSE_STATE mouse;
    ALLEGRO_SAMPLE* vita_persa;
    ALLEGRO_SAMPLE* vita_aggiunta;
    ALLEGRO_SAMPLE* game_over;
    ALLEGRO_SAMPLE* suono_scudo;
    ALLEGRO_SAMPLE* suono_flash;
    ALLEGRO_SAMPLE_INSTANCE* musica_menu;
    ALLEGRO_SAMPLE_INSTANCE* musica_gioco;

    //INIZIALIZZAZIONE DELLA LIBRERIA PER I SUONI
    al_install_audio();
    al_init_acodec_addon();

    al_reserve_samples(5);  //riserva tot posti sul mixer principale

    vita_persa = al_load_sample("Audio/vita_persa.wav");
    vita_aggiunta = al_load_sample("Audio/vita_aggiunta.wav");
    game_over = al_load_sample("Audio/game_over.wav");
    suono_scudo = al_load_sample("Audio/suono_scudo.wav");
    suono_flash = al_load_sample("Audio/suono_flash.wav");

    musica_menu = al_create_sample_instance(al_load_sample("Audio/musica_menu.wav"));   //attacca la musica (gira in loop se finisce) al mixer principale
    musica_gioco = al_create_sample_instance(al_load_sample("Audio/musica_gioco.wav"));
    al_attach_sample_instance_to_mixer(musica_menu, al_get_default_mixer());
    al_attach_sample_instance_to_mixer(musica_gioco, al_get_default_mixer());

    srand(time(NULL));

    al_set_window_title(display, "League of legends di Wish");

    //**************************************************************************************************************************

    do {

        //MENU PRINCIPALE
        al_play_sample_instance(musica_menu);
        al_get_mouse_state(&mouse);

        miglior_punteggios = "BEST SCORE: " + leggi_punteggio();

        if ((mouse.x > 540) && (mouse.x < 731) && (mouse.y > 784) && (mouse.y < 819)) { //pulsante start
            menucolora = true;
            if (mouse.buttons & 1) {
                exit = false;

                //RESETTA LE VARIABILI
                //player
                player.xcentro = displayx / 2;
                player.ycentro = displayy / 2;
                player.xangolo = 0;
                player.yangolo = 0;
                player.direzione = 0;
                player.size = 150;
                player.passo = 3;
                player.vite = 3;

                //proiettili
                for (int i = 0; i < 8; i++) {
                    proiettile[i].xvero = 0;
                    proiettile[i].yvero = 0;
                    proiettile[i].xcentro = 0;
                    proiettile[i].ycentro = 0;
                    proiettile[i].passo = 5;
                    proiettile[i].size = 105;
                    proiettile[i].iniziale = 0;
                    proiettile[i].quanti = 5;
                }

                //powerup vite
                pvita.size = 50;
                pvita.xangolo = 0;
                pvita.yangolo = 0;
                pvita.xcentro = pvita.xangolo + (pvita.size / 2);
                pvita.ycentro = pvita.yangolo + (pvita.size / 2);
                pvita.stato = false;
                pvita.tempo_attesa = 0;
                al_set_timer_count(pvita.attivazione, 0);

                //powerup scudo
                pscudo.size = 50;
                pscudo.xangolo = 0;
                pscudo.yangolo = 0;
                pscudo.xcentro = 0;
                pscudo.ycentro = 0;
                pscudo.stato = false;
                pscudo.tempo_attesa = 0;
                pscudo.attivazione = al_create_timer(1);

                //punteggio
                punteggios = "SCORE: 0";
                flash_stato = false;
                al_set_timer_count(timer_punteggio, 0);
                al_set_timer_count(timer_difficolta, 0);
                al_set_timer_count(pvita.attivazione, 0);
                al_set_timer_count(pscudo.attivazione, 0);
                al_set_timer_count(timer_scudo, -1);

                al_stop_sample_instance(musica_menu); //ferma la musica menu
            }
        }
        else {
            menucolora = false;
        }


        if (sqrt(pow(mouse.x - 521, 2) + pow(mouse.y - 804, 2)) <= 18) {    //pulsante exit
            menucolora1 = true;
            if (mouse.buttons & 1) {
                exit = true;
                exit1 = true;
            }
        }
        else {
            menucolora1 = false;
        }

        if (menucolora) {   //colora il pulsante start oscurando il pulsante exit
            al_clear_to_color(al_map_rgb(89, 128, 145));
            al_draw_filled_circle(521, 804, 18, al_map_rgb(46, 46, 46));
            al_draw_bitmap(menu, 0, 0, 0);
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15, ALLEGRO_ALIGN_LEFT, punteggios.c_str());
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15 + 30 + 20, ALLEGRO_ALIGN_LEFT, miglior_punteggios.c_str());
            al_flip_display();
        }

        if (menucolora1) {  //colora il pulsante exit oscurando il pulsante start
            al_clear_to_color(al_map_rgb(89, 128, 145));
            al_draw_filled_rectangle(540, 784, 731, 819, al_map_rgb(46, 46, 46));
            al_draw_bitmap(menu, 0, 0, 0);
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15, ALLEGRO_ALIGN_LEFT, punteggios.c_str());
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15 + 30 + 20, ALLEGRO_ALIGN_LEFT, miglior_punteggios.c_str());
            al_flip_display();
        }

        if ((menucolora || menucolora1) == false) { //oscura tutti e due i pulsanti
            al_clear_to_color(al_map_rgb(46, 46, 46));
            al_draw_bitmap(menu, 0, 0, 0);
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15, ALLEGRO_ALIGN_LEFT, punteggios.c_str());
            al_draw_text(Testo_lol, al_map_rgb(255, 255, 255), 766 + 15, 113 + 15 + 30 + 20, ALLEGRO_ALIGN_LEFT, miglior_punteggios.c_str());
            al_flip_display();
        }

        //****************************************************************************************************************************************

        pvita.tempo_attesa = 10 + rand() % 10;  //tempo di attesa compreso tra 10 e 19 secondi
        al_start_timer(pvita.attivazione);  //attiva il timer
        al_start_timer(timer_difficolta);   //attiva il timer per aumentare la difficolta

        pscudo.tempo_attesa = 10 + rand() % 10;
        al_start_timer(pscudo.attivazione);
        al_set_timer_count(timer_scudo, -1);

        while (exit == false) {
            al_clear_to_color(al_map_rgb(123, 240, 10)); //cancella tutto
            al_draw_bitmap(sfondo, 0, 0, 0);    //disegna lo sfondo
            al_draw_bitmap(playerbit[player.direzione], player.xangolo, player.yangolo, 0); //disegna il player
            if (pvita.stato == true) {
                al_draw_bitmap(pvitebit, pvita.xangolo, pvita.yangolo, 0);
            }

            if (pscudo.stato == true) {
                al_draw_bitmap(pscudobit, pscudo.xangolo, pscudo.yangolo, 0);
            }

            if (al_get_timer_count(timer_scudo) >= 0) {
                al_draw_bitmap(scudo, player.xangolo - 10, player.yangolo - 10, 0);
            }
            for (int i = 0; i < proiettile[0].quanti; i++) {
                al_draw_bitmap(proiettileimmagine[proiettile[i].iniziale], proiettile[i].xvero, proiettile[i].yvero, 0);    //disegna tutti i proiettili
            }
            al_draw_text(Testo_arial, al_map_rgb(255, 255, 255), 10, 10, 0, punteggios.c_str());  //disegna il punteggio
            al_draw_bitmap(vite[player.vite - 1], displayx - 250 - 15, 15, 0); //disegna le vite
            al_draw_bitmap(flashbit, displayx / 2, 10, 0);
            if (flash_stato == false) {
                al_draw_text(Testo_arial, al_map_rgb(255, 255, 255), (displayx / 2) - 10, 5, ALLEGRO_ALIGN_RIGHT, to_string(60 - al_get_timer_count(timer_difficolta)).c_str());
            }
            else {
                al_draw_text(Testo_arial, al_map_rgb(255, 255, 255), (displayx / 2) - 10, 5, ALLEGRO_ALIGN_RIGHT, "READY");
            }
            al_flip_display();

            al_play_sample_instance(musica_gioco);

            //PUNTEGGIO
            al_start_timer(timer_punteggio);
            punteggios = "SCORE: " + to_string(al_get_timer_count(timer_punteggio));

            //MOVIMENTO PLAYER
            al_get_mouse_state(&mouse);

            if (player.xcentro < mouse.x) {
                player.xcentro += player.passo;
                player.direzione = 1;
            }

            if (player.xcentro > mouse.x) {
                player.xcentro -= player.passo;
                player.direzione = 0;
            }

            if (player.ycentro < mouse.y) {
                player.ycentro += player.passo;
            }

            if (player.ycentro > mouse.y) {
                player.ycentro -= player.passo;
            }

            //STATO FLASH
            if (al_get_timer_count(timer_difficolta) % 60 == 0 && al_get_timer_count(timer_difficolta) != 0) {
                flash_stato = true;
            }

            cout << flash_stato << endl;

            //FLASH
            if ((mouse.buttons & 1) && (flash_stato)) {
                for (int i = 0; i < 45; i++) {
                    al_get_mouse_state(&mouse);

                    if (player.xcentro < mouse.x) {
                        player.xcentro += player.passo;
                        player.direzione = 1;
                    }

                    if (player.xcentro > mouse.x) {
                        player.xcentro -= player.passo;
                        player.direzione = 0;
                    }

                    if (player.ycentro < mouse.y) {
                        player.ycentro += player.passo;
                    }

                    if (player.ycentro > mouse.y) {
                        player.ycentro -= player.passo;
                    }
                }
                al_play_sample(suono_flash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                flash_stato = false;
            }

            player.xangolo = player.xcentro - (player.size / 2);    //calcola le coordinate nelle quali deve essere stampato il player
            player.yangolo = player.ycentro - (player.size / 2);


            //CALCOLO COORDINATE PROIETTILI
            for (int i = 0; i < proiettile[i].quanti; i++) {

                if ((proiettile[i].xvero < -proiettile[i].size * 2 + rand() % proiettile[i].size) && (proiettile[i].iniziale == 0)) {
                    proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                }

                if ((proiettile[i].xvero > displayx + proiettile[i].size * 2 + rand() % proiettile[i].size) && (proiettile[i].iniziale == 1)) {
                    proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                }

                if ((proiettile[i].yvero > displayy + proiettile[i].size * 2 + rand() % proiettile[i].size) && (proiettile[i].iniziale == 2)) {
                    proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                }

                if ((proiettile[i].yvero < -proiettile[i].size * 2 + rand() % proiettile[i].size) && (proiettile[i].iniziale == 3)) {
                    proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                }

                proiettile[i].xcentro = proiettile[i].xvero + (proiettile[i].size / 2);
                proiettile[i].ycentro = proiettile[i].yvero + (proiettile[i].size / 2);
            }

            //MOVIMENTO PROIETTILI
            for (int i = 0; i < proiettile[0].quanti; i++) {
                if (proiettile[i].iniziale == 0) {
                    proiettile[i].xvero -= proiettile[i].passo;
                }
                if (proiettile[i].iniziale == 1) {
                    proiettile[i].xvero += proiettile[i].passo;
                }
                if (proiettile[i].iniziale == 2) {
                    proiettile[i].yvero += proiettile[i].passo;
                }
                if (proiettile[i].iniziale == 3) {
                    proiettile[i].yvero -= proiettile[i].passo;
                }

            }

            //comparsa powerup vite
            if ((al_get_timer_count(pvita.attivazione) >= pvita.tempo_attesa) && (pvita.stato == false)) {
                pvita.stato = true;
                pvita.tempo_attesa = 10;
                pvita.xangolo = rand() % (displayx - pvita.size);
                pvita.yangolo = rand() % (displayy - pvita.size);
                al_set_timer_count(pvita.attivazione, 0);
            }

            //comparsa powerup scudo
            if ((al_get_timer_count(pscudo.attivazione) >= pscudo.tempo_attesa) && (pscudo.stato == false)) {
                pscudo.stato = true;
                pscudo.tempo_attesa = 10;
                pscudo.xangolo = rand() % (displayx - pscudo.size);
                pscudo.yangolo = rand() % (displayy - pscudo.size);
                al_set_timer_count(pscudo.attivazione, 0);
            }

            //scomparsa powerup vite
            if ((al_get_timer_count(pvita.attivazione) >= pvita.tempo_attesa) && pvita.stato) {
                pvita.stato = false;
                pvita.tempo_attesa = 10 + rand() % 10;
                al_set_timer_count(pvita.attivazione, 0);
            }

            //scomparsa powerup scudo
            if ((al_get_timer_count(pscudo.attivazione) >= pscudo.tempo_attesa) && pscudo.stato) {
                pscudo.stato = false;
                pscudo.tempo_attesa = 10 + rand() % 10;
                al_set_timer_count(pscudo.attivazione, 0);
            }

            //sistema le coordinate del centro dei powerup
            pvita.xcentro = pvita.xangolo + (pvita.size / 2);
            pvita.ycentro = pvita.yangolo + (pvita.size / 2);

            pscudo.xcentro = pscudo.xangolo + (pscudo.size / 2);
            pscudo.ycentro = pscudo.yangolo + (pscudo.size / 2);

            //collisione player - powerup vite
            if ((sqrt(pow(player.xcentro - pvita.xcentro, 2) + pow(player.ycentro - pvita.ycentro, 2)) <= (player.size / 2) + (pvita.size / 2)) && (pvita.stato) && (player.vite < 3)) {
                player.vite++;  //incrementa le vite
                al_set_timer_count(pvita.attivazione, 1000); //mette il timer ad una cifra improponibile per far sparire il powerup al prossimo ciclo di gioco
                al_play_sample(vita_aggiunta, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            }

            //collisione player - powerup scudo
            if ((sqrt(pow(player.xcentro - pscudo.xcentro, 2) + pow(player.ycentro - pscudo.ycentro, 2)) <= (player.size / 2) + (pscudo.size / 2)) && (pscudo.stato)) {
                al_start_timer(timer_scudo);
                al_set_timer_count(timer_scudo, 0);
                al_set_timer_count(pscudo.attivazione, 1000); //mette il timer ad una cifra improponibile per far sparire il powerup al prossimo ciclo di gioco
                al_stop_timer(pscudo.attivazione);
                al_play_sample(suono_scudo, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            }

            //aumento difficolta ogni 60 secondi
            if ((al_get_timer_count(timer_difficolta) % 60 == 0) && (al_get_timer_count(timer_difficolta) != 0)) {
                al_set_timer_count(timer_difficolta, 0);
                for (int i = 0; i < proiettile[0].quanti; i++) {
                    proiettile[i].passo++;
                }
            }

            if (al_get_timer_count(timer_scudo) > 4) {
                al_start_timer(pscudo.attivazione);
                al_stop_timer(timer_scudo);
                al_set_timer_count(timer_scudo, -1);
            }

            //CONDIZIONE MORTE
            for (int i = 0; i < proiettile[0].quanti; i++) {

                if ((sqrt(pow(proiettile[i].xcentro - player.xcentro, 2) + pow(proiettile[i].ycentro - player.ycentro, 2)) < player.size / 2 + proiettile[i].size / 2)) {
                    //calcolo vite
                    if (al_get_timer_count(timer_scudo) >= 0) {
                        proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                    }
                    else {
                        player.vite--;
                        proiettile[i].iniziale = spawnproiettili(proiettile[i].xvero, proiettile[i].yvero, proiettile[i].size, displayx, displayy);
                    }


                    //morte e uscita dalla partita
                    if (player.vite <= 0) {

                        player.vite = 3;
                        exit = true;
                        al_stop_timer(timer_punteggio);
                        al_stop_timer(timer_difficolta);

                        if (al_get_timer_count(timer_punteggio) > atoi(leggi_punteggio().c_str())) {
                            miglior_punteggio = al_get_timer_count(timer_punteggio);
                            miglior_punteggios = "BEST SCORE: " + to_string(al_get_timer_count(timer_punteggio));
                            registra_punteggio(to_string(miglior_punteggio));
                        }

                        al_stop_sample_instance(musica_gioco);
                        al_play_sample(game_over, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        //schermata game over (2.55 secondi)
                        for (int i = 0; i < 256; i++) {
                            al_clear_to_color(al_map_rgb(0, 0, 0));
                            al_draw_text(Testo_gameplay, al_map_rgb(i, i, i), displayx / 2, displayy / 2 - 100 / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                            al_flip_display();
                            al_rest(0.005);
                        }

                        for (int i = 255; i >= 0; i--) {
                            al_clear_to_color(al_map_rgb(0, 0, 0));
                            al_draw_text(Testo_gameplay, al_map_rgb(i, i, i), displayx / 2, displayy / 2 - 100 / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                            al_flip_display();
                            al_rest(0.005);
                        }

                    }
                    else {  //se si perde una vita ma non si muore si sente un suono
                        if (!(al_get_timer_count(timer_scudo) >= 0)) {
                            al_play_sample(vita_persa, 1.5, 0, 2, ALLEGRO_PLAYMODE_ONCE, 0);
                        }

                    }

                }

            }
            al_rest(0.01);
        }
        //FINE GIOCO

    //********************************************************************************************************************************
    } while (exit1 == false);
}