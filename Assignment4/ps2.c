#include <stdio.h>
#include <omp.h>

#define BUFFER_SIZE 5
#define ITEMS_TO_PRODUCE 10

int buffer[BUFFER_SIZE];
int count = 0; // no. of items in buffer
int in = 0;    // nxt wr index
int out = 0;   // nxt rd index

void produce_item(int item)
{
    printf("Produced: %d\n", item);
}

void consume_item(int item)
{
    printf("Consumed: %d\n", item);
}

int main()
{
    int item = 0;

#pragma omp parallel sections shared(buffer, count, in, out, item)
    {
// Producer section
#pragma omp section
        {
            for (int i = 0; i < ITEMS_TO_PRODUCE; i++)
            {
                // Wait until buffer has space
                while (count == BUFFER_SIZE)
                {
#pragma omp flush(count)
                }

#pragma omp critical
                {
                    buffer[in] = ++item;
                    in = (in + 1) % BUFFER_SIZE;
                    count++;
                    produce_item(item);
                }
#pragma omp flush(count)
            }
        }

// Consumer section
#pragma omp section
        {
            for (int i = 0; i < ITEMS_TO_PRODUCE; i++)
            {
                // Wait until buffer has data
                while (count == 0)
                {
#pragma omp flush(count)
                }

#pragma omp critical
                {
                    int data = buffer[out];
                    out = (out + 1) % BUFFER_SIZE;
                    count--;
                    consume_item(data);
                }
#pragma omp flush(count)
            }
        }
    }

    return 0;
}
