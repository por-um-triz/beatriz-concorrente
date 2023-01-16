#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1

/* TWEET INFORMATION */
#define TWEET_FILE "twitter-info/tweets.txt"
#define USERNAME_FILE "twitter-info/usernames.txt"
#define TWEET_SIZE 280 + 2
#define MAX_QUANT_TWEETS 100
#define USERNAME_SIZE 15 + 2
#define MAX_QUANT_USERNAMES 50

/* PROBLEM INFORMATION */
#define NUMERO_REDES 5

#define PAUSA_LEITURA 20
#define PAUSA_PSICOLOGO 10

void* open_twitter_info();

void* beatriz_e_seguidores(char* user);
void* funcionarios(char* user);
void* elon_musk(char* user);
void* usuarios_que_retweetam(char* user);

char* pensar_em_tweet(char* user);
void* lendo_livro(char *user);
void* indo_no_psicologo(char *user);
void* print_tweet(char *user, char *tweet);
void* mover_tweets_para_twitter(char *funcionario);
void* elon_musk_rindo_de_tweets(char *user);
void* elon_musk_esvaziando_twitter(char *user);
void* usuarios_retweetando(char *user);

char (*tweets)[TWEET_SIZE] = NULL, (*usernames)[MAX_QUANT_USERNAMES] = NULL;

int quant_users = 0, quant_tweets = 0;

sem_t redes_sociais_disponiveis;
sem_t redes_sociais_em_manutencao;

pthread_mutex_t database_twitter = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_elon_musk    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_funcionarios = PTHREAD_MUTEX_INITIALIZER;

int tweets_que_faltam = 0;

int main() {

    open_twitter_info();

    srand(time(NULL));

    sem_init(&redes_sociais_disponiveis, 0, NUMERO_REDES);
    sem_init(&redes_sociais_em_manutencao, 0, 0);

    // INICIALIZE THREADS AQUI
    // INICIALIZE THREADS AQUI

    char* tweet = pensar_em_tweet("@beatriz");
    print_tweet("@beatriz", tweet);

    free(tweets);
    free(usernames);

    return 0;
}

/* AÇÕES */

void* lendo_livro(char *user) {
    printf("\e[1m%s\e[m deu uma pauda para ler!\n\n", user);
    sleep(5);
    printf("\e[1m%s\e[m saiu de sua pausa de leitura!\n\n", user);
}

void* indo_no_psicologo(char *user) {
    printf("O funcionário \e[1m%s\e[m saiu para ir ao psicólogo!\n\n", user);
    sleep(5);
    printf("O funcionário \e[1m%s\e[m voltou do psicólogo!\n\n", user);
}

void* print_tweet(char *user, char *tweet) {
    printf("\e[1m%s\e[m tweetou:\n\n   '%s'\n\n", user, tweet);
}

char* pensar_em_tweet(char* user) {
    printf("\e[1m%s\e[m está pensando em um tweet para escrever...\n\n", user);
    sleep(2);
    return tweets[rand() % quant_tweets];
}

void* mover_tweets_para_twitter(char *funcionario) {
    printf(
        "O funcionário %s está movendo os tweets de uma das redes para a rede principal!\n\n",
        funcionario
    );
    sleep(10);
}

void* elon_musk_rindo_de_tweets(char *user) {
    sleep(10);
}

void* elon_musk_esvaziando_twitter(char *user) {
    sleep(10);
}

void* usuarios_retweetando(char *user) {
    printf("O usuário %s está retweetando um tweet!\n\n", user);
}

/* THREADS */

// Pega slot disponíveis de tweet e post tweet
// Não faz nada enquanto não houverem espaços de tweet disponíveis
// Param para descansar as vezes

// void* beatriz_e_seguidores(char* user) {
//     int cansaco = 0;

//     while (TRUE) {
//         sem_wait(&redes_sociais_disponiveis);
//             // FAZER TWEETS
//             while (!FULL) {
//                 // incrementar quantidade de tweets da rede

//                 int tweet = pensar_em_tweet(user);
//                 print_tweet(user, tweet);

//                 cansaco++;
//                 if (cansaco == PAUSA_LEITURA) {
//                     lendo_livro(user);
//                     cansaco = 0;
//                 }
//             }
//         sem_post(&redes_sociais_em_manutencao);
//     }
// }

void* beatriz_e_seguidores(char* user) {
    int cansaco = 0;

    while (TRUE) {
        sem_wait(&redes_sociais_disponiveis);
            int tweet = pensar_em_tweet(user);
            print_tweet(user, tweet);

        sem_post(&redes_sociais_em_manutencao);
            cansaco++;

        if (cansaco == PAUSA_LEITURA) {
            lendo_livro(user);
            cansaco = 0;
        }
    }
}


// Pegam os tweets das redes sociais que entraram em mamutenção
void* funcionarios(char* funcionario) {
    int cansaco = 0;

    while (TRUE) {
        sem_wait(&redes_sociais_em_manutencao);
            printf("Mover um tweet para o Twitter.");

        pthread_mutex_lock(&database_twitter);
            while (tweets_que_faltam == NULL) {
                printf("Database está cheio!");
                pthread_cond_signal(&cond_elon_musk);

                pthread_cond_wait(&cond_funcionarios, &database_twitter);
            }

            mover_tweets_para_twitter(funcionario);
        pthread_mutex_unlock(&database_twitter);

        sem_post();
        cansaco++;

        if (cansaco == PAUSA_LEITURA) {
            indo_no_psicologo(funcionario);
            cansaco = 0;
        }
    }
}

// Aparece para rir dos tweets
// Deleta os tweets da database principal
void* elon_musk(char* user) {
    int cansaco = 0;

    while (TRUE) {
        pthread_mutex_lock(&database_twitter);
        
        pthread_mutex_unlock(&database_twitter);
    }
}

// C
void* usuarios_que_retweetam(char* user) {
    while (TRUE) {
        
    }
}

/* ABRIR ARQUIVO */

void* open_twitter_info() {

    FILE *tweet_file = fopen(TWEET_FILE, "r");
    FILE *username_file = fopen(USERNAME_FILE, "r");

    if (!(tweets = malloc(MAX_QUANT_TWEETS * sizeof *tweets)))
        exit(EXIT_FAILURE);

    if (!(usernames = malloc(MAX_QUANT_USERNAMES * sizeof *usernames)))
        exit(EXIT_FAILURE);

    while (
        quant_tweets < MAX_QUANT_TWEETS && fgets(tweets[quant_tweets], TWEET_SIZE, tweet_file))
    {
        char *p = tweets[quant_tweets];
        for (; *p && *p != '\n'; p++) {}
        *p = 0;
        quant_tweets++;
    }

    while (
        quant_users < MAX_QUANT_USERNAMES && fgets(usernames[quant_users], USERNAME_SIZE, username_file))
    {
        char *z = usernames[quant_users];
        for (; *z && *z != '\n'; z++) {}
        *z = 0;
        quant_users++;
    }

    fclose(tweet_file);
    fclose(username_file);
}
