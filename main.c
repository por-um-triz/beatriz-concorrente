#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define ZERO 0

/* OPEN TWEETS - INFORMATION */
#define TWEET_FILE "twitter-info/tweets.txt"
#define USERNAME_FILE "twitter-info/usernames.txt"
#define TWEET_SIZE 280 + 2
#define MAX_QUANT_TWEETS 100
#define USERNAME_SIZE 15 + 2
#define MAX_QUANT_USERNAMES 50

/* PROBLEM INFORMATION */
#define QUANT_FUNCIONARIOS 3

#define TAMANHO_DATABASE_TWITTER 20
#define QUANT_SLOTS 6

#define PAUSA_LEITURA 20
#define PAUSA_PSICOLOGO 10

void open_twitter_info();

void* beatriz_e_seguidores(void* arg);
void* funcionarios(void* arg);
// void* elon_musk();
// void* usuarios_que_retweetam(char* user);

char* pensar_em_tweet(char* user);
void lendo_livro(char *user);
void indo_no_psicologo(int funcionario);
void print_tweet(char *user, char *tweet);
void mover_tweets_para_twitter(int funcionario);
void elon_musk_lendo_tweets();
void elon_musk_esvaziou_twitter();
void usuarios_retweetando(char *user);

char (*tweets)[TWEET_SIZE] = NULL, (*usernames)[MAX_QUANT_USERNAMES] = NULL;

int quant_users = 0, quant_tweets = 0;

sem_t slots_disponiveis;
sem_t slots_indisponiveis;

pthread_mutex_t database_twitter = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_elon_musk = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_funcionarios = PTHREAD_COND_INITIALIZER;

int tweets_que_faltam = TAMANHO_DATABASE_TWITTER;

void * bia(void *user);

int main() {
    open_twitter_info();

    srand(time(NULL));

    sem_init(&slots_disponiveis, 0, QUANT_SLOTS);
    sem_init(&slots_indisponiveis, 0, 0);

    // INICIALIZE THREADS AQUI
    // INICIALIZE THREADS AQUI

    pthread_t beatriz_teste[quant_users];

    pthread_t beatriz_threads[quant_users];
    pthread_t funcionarios_thread[quant_users];

    // for (int index = 0; index < quant_users; index++) {
    //     char* user_atual = usernames[index];
    //     pthread_create(&(beatriz_teste[index]), NULL, bia, user_atual);
    // }

    for (int index = 0; index < quant_users; index++) {
        char* user_atual = usernames[index];
        pthread_create(&(beatriz_threads[index]), NULL, beatriz_e_seguidores, user_atual);
    }

    int * id;

    for (int index = 0; index < QUANT_FUNCIONARIOS; index++) {
        id = (int *) malloc(sizeof(int));
        *id = index;
        pthread_create(&(funcionarios_thread[index]), NULL, funcionarios, (void *) (id+1));
    }

    // pthread_create(&elon_musk_thread, NULL, elon_musk);

    // INICIALIZE THREADS EM CIMA
    // INICIALIZE THREADS EM CIMA

    for (int i = 0; i < quant_users; i++) 
        pthread_join(beatriz_threads[i], NULL);

    for (int i = 0; i < QUANT_FUNCIONARIOS; i++) 
        pthread_join(funcionarios_thread[i], NULL);

    // pthread_join(elon_musk_thread, NULL);

    free(tweets);
    free(usernames);
}

/* AÇÕES */

void lendo_livro(char *user) {
    printf("\e[1m%s\e[m deu uma pausa para ler!\n\n", user);
    sleep(5);
    printf("\e[1m%s\e[m saiu de sua pausa de leitura!\n\n", user);
}

void indo_no_psicologo(int funcionario) {
    printf("O funcionário \e[1m%d\e[m saiu para ir ao psicólogo!\n\n", funcionario);
    sleep(5);
    printf("O funcionário \e[1m%d\e[m voltou do psicólogo!\n\n", funcionario);
}

void print_tweet(char *user, char *tweet) {
    printf("\e[1m%s\e[m tweetou:\n\n   '%s'\n\n", user, tweet);
}

char* pensar_em_tweet(char* user) {
    printf("\e[1m%s\e[m está pensando em um tweet para escrever...\n\n", user);
    sleep(2);
    return tweets[rand() % quant_tweets];
}

void vai_mover_tweets(int funcionario) {
    printf("O funcionário %d vai mover um tweet para o Twitter!\n\n", funcionario);
}

void mover_tweets_para_twitter(int funcionario) {
    printf("O funcionário %d está movendo os tweets para o Twitter!\n\n", funcionario);
    sleep(10);
}

// void elon_musk_lendo_tweets() {
//     printf("Elon Musk está lendo tweets e ficando bravo!\n");
//     sleep(7);
// }

// void elon_musk_esvaziou_twitter() {
//     sleep(10);
//     printf("Elon Musk se irritou e deletou todos os tweets do Twitter!\n");
// }

// void usuarios_retweetando(char *user) {
//     printf("O usuário %s está retweetando um tweet!\n\n", user);
// }

/* THREADS */

void * bia(void *arg) {
    char* user = ((char *) arg);
    printf("thread() entered with argument '%s'\n", user);
    return NULL;
}

void * beatriz_e_seguidores(void* arg) {
    char* user = ((char *) arg);
    int cansaco = 0;

    while (TRUE) {
        sem_wait(&slots_disponiveis);
            char* tweet = pensar_em_tweet(user);
        sem_post(&slots_indisponiveis);

        print_tweet(user, tweet);
        cansaco++;

        if (cansaco == PAUSA_LEITURA) {
            lendo_livro(user);
            cansaco = 0;
        }
    }
}

void* funcionarios(void* arg) {
    int funcionario = *((int *) arg);
    int cansaco = 0;

    while (TRUE) {
        sem_wait(&slots_indisponiveis);
            vai_mover_tweets(funcionario);

        pthread_mutex_lock(&database_twitter);
            while (tweets_que_faltam == ZERO) {
                printf("A database está cheia! Chamamos o Elon Musk!\n\n");
                pthread_cond_signal(&cond_elon_musk);

                pthread_cond_wait(&cond_funcionarios, &database_twitter);
                printf("O funcionário %d vai dormir!", funcionario);
            }

            mover_tweets_para_twitter(funcionario);
            tweets_que_faltam--;
        pthread_mutex_unlock(&database_twitter);

        sem_post(&slots_disponiveis);
        cansaco++;

        if (cansaco == PAUSA_LEITURA) {
            indo_no_psicologo(funcionario);
            cansaco = 0;
        }
    }
}

// void* elon_musk() {
//     int cansaco = 0;

//     while (TRUE) {
//         pthread_mutex_lock(&database_twitter);
//             while (tweets_que_faltam != 0) {
//                 printf("O Twitter ainda não está cheio! O Elon Musk vai dormir!\n\n");
//                 pthread_cond_wait(&cond_elon_musk, &database_twitter);
//             }
//         pthread_mutex_unlock(&database_twitter);

//         elon_musk_lendo_tweets();

//         pthread_mutex_lock(&database_twitter);
//             tweets_que_faltam = TAMANHO_DATABASE_TWITTER;
//             pthread_cond_broadcast(&cond_funcionarios);
//         pthread_mutex_unlock(&database_twitter);

//         elon_musk_esvaziou_twitter();
//     }
// }

// void* usuarios_que_retweetam(char* user) {}

/* ABRIR ARQUIVO */

void open_twitter_info() {

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
