// Dijkstra Algorithm
#include <stdio.h>
#define N 6
#define INF 1000

void initialization(int edges[N][N], int dist[N], int prev[N], int visited[N]){
    int i, j;\

    for (i = 0; i < N; i++) {
        dist[i] = 0;
        prev[i] = 0;
        visited[i] = 0;
        for (j = 0; j < N; j++) {
            edges[i][j] = -1;
        }
    }

    edges[0][1] = 14;
    edges[1][0] = 14;
    edges[0][2] = 9;
    edges[2][0] = 9;
    edges[0][3] = 7;
    edges[3][0] = 7;
    edges[1][2] = 2;
    edges[2][1] = 2;
    edges[1][4] = 10;
    edges[4][1] = 10;
    edges[2][3] = 10;
    edges[3][2] = 10;
    edges[2][5] = 11;
    edges[5][2] = 11;
    edges[3][5] = 15;
    edges[5][3] = 15;
    edges[4][5] = 6;
    edges[5][4] = 6;
}

void findCheapestPath (const int edges[N][N], int dist[N], int prev[N], int visited[N], int start) {
    int i, repeats = 0, curr, minDist = INF, Index, neigh[N];

    
    for (i = 1; i < N; i++) {
        dist[i] = INF;
        neigh[i] = 0;
    }
    curr = start;
    dist[start] = 0;
    do {
        for (i = 0; i < N; i++) {
            if (edges[curr][i] > 0 && visited[i] == 0) {
                neigh[i] = 1;
                if (edges[curr][i] + dist[curr] < dist[i]) {
                    dist[i] = edges[curr][i] + dist[curr];
                    prev[i] = curr;
                    printf("curr -> %d i -> %d dist[%d] -> %d prev[%d] -> %d\n", curr, i, i, dist[i], i, prev[i]);       
                }
            }
        }
        visited[curr] = 1;
        for (i = 0; i < N; i++) {
            if (neigh[i] && !visited[i]) {
                printf("dist[%d] -> %d\n", i, dist[i]);
                if (dist[i] < minDist) {
                    minDist = dist[i];
                    Index = i;
                    printf("Index -> %d\n", Index);
                }
            }    
        }
        printf("curr <- %d\n", Index);
        curr = Index;
        repeats++;
        minDist = INF;
    }
    while (repeats < N);
}

int main (int argc, char *argv[]) {
    int edges[N][N], dist[N], prev[N], visited[N], end, start;

    initialization (edges, dist, prev, visited);
    printf("Enter start point:\n");
    scanf(" %d", &start);
    findCheapestPath (edges, dist, prev, visited, start);
    
    prev[start] = -1;
    printf("Pick an end:\n");
    scanf(" %d", &end);
    
    do {
        printf("-> %d", end);
        end = prev[end];
    }
    while (end != -1);
    printf("\n");

    return 0;
}