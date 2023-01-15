#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define TWEET_FILE "twitter-info/tweets.txt"
#define USERNAME_FILE "twitter-info/usernames.txt"

#define TWEET_SIZE 280 + 2
#define MAX_QUANT_TWEETS 50
#define USERNAME_SIZE 15 + 2
#define MAX_QUANT_USERNAMES 280

#define TRUE 1
#define PAUSA_PARA_LEITURA 10
#define QUANTIDADE_MAXIMA_TWEETS 20

char (*tweets)[TWEET_SIZE] = NULL, (*usernames)[MAX_QUANT_USERNAMES] = NULL;

int quant_users = 0, quant_tweets = 0;

int capacidade_atual = QUANTIDADE_MAXIMA_TWEETS;

void open_twitter_info();
void print_tweet(char *user, char *tweet);
char* pensar_em_tweet();

int main() {
    open_twitter_info();

    // INICIALIZE THREADS AQUI
    // INICIALIZE THREADS AQUI

    char* tweet = pensar_em_tweet();
    print_tweet("@beatriz", tweet);

    free(tweets);
    free(usernames);

    return 0;
}

void open_twitter_info() {
    FILE *tweet_file = fopen(TWEET_FILE, "r");
    FILE *username_file = fopen(USERNAME_FILE, "r");

    if (!(tweets = malloc(MAX_QUANT_TWEETS * sizeof *tweets)))
        exit(EXIT_FAILURE);

    if (!(usernames = malloc(MAX_QUANT_USERNAMES * sizeof *usernames)))
        exit(EXIT_FAILURE);

    while (quant_tweets < MAX_QUANT_TWEETS && fgets(tweets[quant_tweets], TWEET_SIZE, tweet_file))
    {
        char *p = tweets[quant_tweets];
        for (; *p && *p != '\n'; p++) {}
        *p = 0;
        quant_tweets++;
    }

    while (quant_users < MAX_QUANT_USERNAMES && fgets(usernames[quant_users], USERNAME_SIZE, username_file))
    {
        char *z = usernames[quant_users];
        for (; *z && *z != '\n'; z++) {}
        *z = 0;
        quant_users++;
    }

    fclose(tweet_file);
    fclose(username_file);
}

void print_tweet(char *user, char *tweet) {
    printf("\e[1m%s\e[m tweetou:\n\n   '%s'\n\n", user, tweet);
}

char* pensar_em_tweet() {
    printf("fulano está pensando em um tweet para escrever...\n\n");
    srand(time(NULL));
    sleep(2);
    return tweets[rand() % quant_tweets];
}
