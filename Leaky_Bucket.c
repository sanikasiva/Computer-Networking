#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int main()
{
    int bucketCapacity, leakRate, droppedBytes = 0, bufferLevel = 0;
    int totalIntervals, arrivals[20] = {0};
    int interval = 0, continueInput, overflowAmount, hasOverflow;

    printf("Enter bucket capacity (Bytes): ");
    scanf("%d", &bucketCapacity);

    printf("Enter leak rate (Bytes/sec): ");
    scanf("%d", &leakRate);

    do {
        printf("Enter packet size arriving at second %d: ", interval + 1);
        scanf("%d", &arrivals[interval]);
        interval++;
        printf("Continue entering packets? (1 = yes, 0 = no): ");
        scanf("%d", &continueInput);
    } while (continueInput && interval < 20);

    totalIntervals = interval;

    printf("\nTime\tArrived\tSent\tDropped\tRemaining\n");
    for (interval = 0; interval < totalIntervals || bufferLevel > 0; interval++) {
        hasOverflow = 0;

        int incoming = (interval < totalIntervals) ? arrivals[interval] : 0;
l
        printf("%2d\t%6d", interval + 1, incoming);

        if (incoming + bufferLevel > bucketCapacity) {
            hasOverflow = incoming;
            
            int sent = MIN(bufferLevel, leakRate);
            printf("\t%4d", sent);
            
            bufferLevel = (bufferLevel > leakRate) ? (bufferLevel - leakRate) : 0;
            
            droppedBytes = hasOverflow;
            printf("\t%7d\t%8d\n", droppedBytes, bufferLevel);
            continue;
        }

        int canSend = MIN(bufferLevel + incoming, leakRate);
        printf("\t%4d", canSend);

        overflowAmount = bufferLevel + incoming - leakRate;
        if (overflowAmount > 0) {
            if (overflowAmount > bucketCapacity) {
                bufferLevel   = bucketCapacity;
                droppedBytes  = overflowAmount - bucketCapacity;
            } else {
                bufferLevel  = overflowAmount;
                droppedBytes = 0;
            }
        } else {
            bufferLevel  = 0;
            droppedBytes = 0;
        }

        printf("\t%7d\t%8d\n", droppedBytes, bufferLevel);
    }

    return 0;
}
