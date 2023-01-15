#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TWEET_FILE "twitter-info/tweets.txt"
#define USERNAME_FILE "twitter-info/usernames.txt"

#define TWEET_SIZE 280 + 2
#define QUANT_TWEETS 50
#define USERNAME_SIZE 15 + 2
#define QUANT_USERNAMES 280

#define TRUE 1
#define PAUSA_PARA_LEITURA 10
#define QUANTIDADE_MAXIMA_TWEETS 20

char (*tweets)[TWEET_SIZE] = NULL;
char (*usernames)[QUANT_USERNAMES] = NULL;

int index_users = 0, index_tweets = 0;

int capacidade_atual = QUANTIDADE_MAXIMA_TWEETS;

void open_twitter_info();
void print_tweet(char *user, char *tweet);

int main() {

    open_twitter_info();

    // INICIALIZE THREADS AQUI
    // INICIALIZE THREADS AQUI

    free(tweets);
    free(usernames);

    return 0;
}

void open_twitter_info() {
    FILE *tweet_file = fopen(TWEET_FILE, "r");
    FILE *username_file = fopen(USERNAME_FILE, "r");

    if (!(tweets = malloc(QUANT_TWEETS * sizeof *tweets)) || !(usernames = malloc(QUANT_USERNAMES * sizeof *usernames)))
        exit(EXIT_FAILURE);

    while (index_tweets < QUANT_TWEETS && fgets(tweets[index_tweets], TWEET_SIZE, tweet_file))
    {
        char *p = tweets[index_tweets];
        for (; *p && *p != '\n'; p++) {}
        *p = 0;
        index_tweets++;
    }

    while (index_users < QUANT_USERNAMES && fgets(usernames[index_users], USERNAME_SIZE, username_file))
    {
        char *z = usernames[index_users];
        for (; *z && *z != '\n'; z++) {}
        *z = 0;
        index_users++;
    }

    fclose(tweet_file);
    fclose(username_file);
}

void print_tweet(char *user, char *tweet) {
    printf("%s tweetou:\n\n", user);
    printf("   '%s'\n\n", tweet);
}
