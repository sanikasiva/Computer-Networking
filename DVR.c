#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 20

typedef struct {
    unsigned cost[MAX_NODES];
    unsigned nextHop[MAX_NODES];
} RouterEntry;

RouterEntry routingTable[MAX_NODES];

int main() {
    int costMatrix[MAX_NODES][MAX_NODES];
    int numRouters, a, b, c, updates;

    printf("Enter the number of routers: ");
    scanf("%d", &numRouters);

    printf("Enter the cost matrix:\n");
    for (a = 0; a < numRouters; a++) {
        for (b = 0; b < numRouters; b++) {
            scanf("%d", &costMatrix[a][b]);
            if (a == b) costMatrix[a][b] = 0;
            routingTable[a].cost[b]    = costMatrix[a][b];
            routingTable[a].nextHop[b] = b;
        }
    }

    // Iteratively update until no more changes
    do {
        updates = 0;
        for (a = 0; a < numRouters; a++) {
            for (b = 0; b < numRouters; b++) {
                for (c = 0; c < numRouters; c++) {
                    unsigned viaCost = costMatrix[a][c] + routingTable[c].cost[b];
                    if (routingTable[a].cost[b] > viaCost) {
                        routingTable[a].cost[b]    = viaCost;
                        routingTable[a].nextHop[b] = c;
                        updates++;
                    }
                }
            }
        }
    } while (updates != 0);

    // Print final tables
    for (a = 0; a < numRouters; a++) {
        printf("\nRouting table for router %c:\n", 'A' + a);
        printf(" Destination | Next Hop | Distance\n");
        printf("-------------+----------+---------\n");
        for (b = 0; b < numRouters; b++) {
            char nh = (routingTable[a].nextHop[b] == b)
                      ? '-' 
                      : ('A' + routingTable[a].nextHop[b]);
            printf("     %c       |    %c     |   %u\n",
                   'A' + b, nh, routingTable[a].cost[b]);
        }
    }
    printf("\n");
    return 0;
}
