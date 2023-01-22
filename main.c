#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define ZERO 0

/* OPEN TWEETS - INFORMAÇÕES */
#define TWEET_FILE "twitter-info/tweets.txt"
#define USERNAME_FILE "twitter-info/usernames.txt"
#define TWEET_SIZE 280 + 2
#define MAX_QUANT_TWEETS 100
#define USERNAME_SIZE 15 + 2
#define MAX_QUANT_USERNAMES 50

/* INFORMAÇÕES DO PROBLEMA */
#define QUANT_FUNCIONARIOS 3
#define QUANT_SLOTS 6
#define TAMANHO_DATABASE_TWITTER 20

#define PAUSA_LEITURA 4
#define PAUSA_PSICOLOGO 4

void open_twitter_info();

void* beatriz_e_seguidores(void* arg);
void* funcionarios(void* arg);
void* elon_musk(void* arg);

char (*tweets)[TWEET_SIZE] = NULL, (*usernames)[MAX_QUANT_USERNAMES] = NULL;

int quant_users = 0, quant_tweets = 0;

sem_t slots_disponiveis, slots_indisponiveis;

pthread_mutex_t database_twitter = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_elon_musk = PTHREAD_COND_INITIALIZER, cond_funcionarios = PTHREAD_COND_INITIALIZER;

int tweets_que_faltam = TAMANHO_DATABASE_TWITTER;

int main() {
    open_twitter_info();

    srand(time(NULL));

    for (int i=0; i < quant_users; i++)
        printf("%s\n", usernames[i]);

    sem_init(&slots_disponiveis, 0, QUANT_SLOTS);
    sem_init(&slots_indisponiveis, 0, 0);

    pthread_t beatriz_threads[quant_users];
    pthread_t funcionarios_thread[quant_users];
    pthread_t elon_musk_thread;

    int * id;

    for (int index = 0; index < quant_users; index++) {
        char* user_atual = usernames[index];

        pthread_create(&(beatriz_threads[index]), NULL, beatriz_e_seguidores, user_atual);
    }

    for (int index = 0; index < QUANT_FUNCIONARIOS; index++) {
        id = (int *) malloc(sizeof(int));
        *id = index;
        pthread_create(&(funcionarios_thread[index]), NULL, funcionarios, (void *) (id));
    }

    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&elon_musk_thread, NULL, elon_musk, (void *) (id));

    for (int index = 0; index < quant_users; index++) {
        pthread_join(beatriz_threads[index], NULL);
    }

    for (int index = 0; index < QUANT_FUNCIONARIOS; index++) {
        pthread_join(funcionarios_thread[index], NULL);
    }

    pthread_join(elon_musk_thread, NULL);

    free(tweets);
    free(usernames);
}

/* AÇÕES */

char* pensar_em_tweet(char* user) {
    printf("\e[1m%s\e[m está pensando em um tweet para escrever...\n\n", user);
    return tweets[rand() % quant_tweets];
}

void print_tweet(char *user, char *tweet) {
    sleep(rand()%3);
    printf("\e[1m%s\e[m tweetou:\n\n   '%s'\n\n", user, tweet);
}

int lendo_livro(char *user) {
    printf("\e[1m%s\e[m deu uma pausa para ler!\n\n", user);
    sleep(5);
    printf("\e[1m%s\e[m saiu de sua pausa de leitura!\n\n", user);

    return 0;
}

void mover_tweets_para_database(int funcionario) {
    printf("O funcionário %d está movendo o tweet para a database!\n\n", funcionario);
    sleep(3);
}

void terminou_de_mover_tweets(int funcionario, int tweets_que_faltam) {
    sleep(3);
    printf(
        "Funcionário %d moveu um tweet para a database. Espaço disponível na database: %d\n\n",
        funcionario,
        tweets_que_faltam
    );
}

int indo_pro_psicologo(int funcionario) {
    printf("O funcionário \e[1m%d\e[m saiu para ir ao psicólogo!\n\n", funcionario);
    sleep(5);
    printf("O funcionário \e[1m%d\e[m voltou do psicólogo!\n\n", funcionario);

    return 0;
}

void elon_musk_lendo_tweets() {
    printf("Elon Musk está lendo tweets da database e ficando bravo!\n\n");
    sleep(7);
}

void elon_musk_esvaziou_database() {
    sleep(10);
    printf("ATENÇÃO: Elon Musk se irritou e deletou todos os tweets da database!\n");
    printf("Os usuários podem voltar a tweetar!\n");
}

/* THREADS */

void * beatriz_e_seguidores(void *arg) {
    char* user = ((char *) arg);
    int quer_ler = 0;  // desejo de leitura do usuário zerado

    while (TRUE) {
        char * tweet = pensar_em_tweet(user);  // usuário pensa em tweet

        sem_wait(&slots_disponiveis);    // usuário pega um espaço disponível no Twitter
            print_tweet(user, tweet);    // usuário tweeta
        sem_post(&slots_indisponiveis);  // o usuário incrementa a quantidade de espaços ocupados

        quer_ler++;  // incrementa o desejo de leitura do usuário

        // se o desejo de leitura atingir a quantidade necessária para a pausa
        // de leitura, o usuário le um livro e o desejo é zerado
        if (quer_ler == PAUSA_LEITURA)
            quer_ler = lendo_livro(user);
    }
}

void * funcionarios(void *arg) {
    int funcionario = *((int *) arg);
    int psicologo = 0;  // necessidade de psicólogo do funcionário zerado

    while (TRUE) {
        pthread_mutex_lock(&database_twitter);
            sem_wait(&slots_indisponiveis);  // funcionário decrementa os espaço indisponíveis
        pthread_mutex_unlock(&database_twitter);

            pthread_mutex_lock(&database_twitter);  // lock para consultar variável

                while (tweets_que_faltam == 0) {  // caso a database esteja cheia
                    printf("O Twitter está cheio, funcionário %d vai dormir\n\n", funcionario);

                    pthread_cond_signal(&cond_elon_musk);                      // chama o Elon Musk
                    pthread_cond_wait(&cond_funcionarios, &database_twitter);  // funcionário dorme
                }

                mover_tweets_para_database(funcionario);  // funcionário move um tweet para a database

                // decrementa a quantidade de tweets que faltam para encher a database
                tweets_que_faltam--;

                // funcionário terminou de mover tweet para a database
                terminou_de_mover_tweets(funcionario, tweets_que_faltam);

            pthread_mutex_unlock(&database_twitter);  // libera o lock para consultar variável

        sem_post(&slots_disponiveis);  // funcionário incrementa os espaço disponíveis

        psicologo++;  // incrementa a necessidade de ir ao psicólogo

        // se a necessidade de ir ao psicólogo atingir a quantidade necessária
        // para ir ao psicólogo, o funcionário vai e a necessidade é zerada
        if (psicologo == PAUSA_PSICOLOGO)
            psicologo = indo_pro_psicologo(funcionario);
    }
}

void * elon_musk(void *arg) {
    while (TRUE) {
        sleep(20);  // Elon Musk chegando
        printf("Elon Musk apareceu na empresa!\n\n");
        
        pthread_mutex_lock(&database_twitter);  // lock para consultar variável

            while (tweets_que_faltam != ZERO) {  // caso a database não esteja cheia
                printf("A database ainda não está cheia! Elon Musk volta depois!\n\n");
                pthread_cond_wait(&cond_elon_musk, &database_twitter);  // Elon Musk dorme
            }

        pthread_mutex_unlock(&database_twitter);  // libera lock para consultar variável

        elon_musk_lendo_tweets();  // Elon Musk lê os tweets e se irrita

        pthread_mutex_lock(&database_twitter);  // lock para consultar variável

            // restaura a quantidade de tweets que faltam para o tamanho da database
            tweets_que_faltam = TAMANHO_DATABASE_TWITTER;

            // acorda todos os funcionários
            pthread_cond_broadcast(&cond_funcionarios);

        pthread_mutex_unlock(&database_twitter);  // libera lock para consultar variável

        elon_musk_esvaziou_database();  // Elon Musk avisa que esvaziou a database
    }
}

/* ABRIR ARQUIVO */

void open_twitter_info() {

    FILE *tweet_file = fopen(TWEET_FILE, "r");        // arquivo de tweets
    FILE *username_file = fopen(USERNAME_FILE, "r");  // arquivo de usuários

    // garantindo memória para array de tweets
    if (!(tweets = malloc(MAX_QUANT_TWEETS * sizeof *tweets)))
        exit(EXIT_FAILURE);

    // garantindo memória para array de usuários
    if (!(usernames = malloc(MAX_QUANT_USERNAMES * sizeof *usernames)))
        exit(EXIT_FAILURE);

    // abre o arquivo de tweets e armazena cada linha em array
    while (
        quant_tweets < MAX_QUANT_TWEETS && fgets(tweets[quant_tweets], TWEET_SIZE, tweet_file))
    {
        char *p = tweets[quant_tweets];
        for (; *p && *p != '\n'; p++) {}
        *p = 0;
        quant_tweets++;
    }

    // abre o arquivo de usuários e armazena cada linha em array
    while (
        quant_users < MAX_QUANT_USERNAMES && fgets(usernames[quant_users], USERNAME_SIZE, username_file))
    {
        char *z = usernames[quant_users];
        for (; *z && *z != '\n'; z++) {}
        *z = 0;
        quant_users++;
    }

    // fecha os arquivos
    fclose(tweet_file);
    fclose(username_file);
}
