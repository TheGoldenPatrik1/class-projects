#include <stdio.h>
#include <stdlib.h>

// define values
#define NUM_PAGES 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define TLB_SIZE 16

// TLB linked list item
struct TLBslot {
    struct TLBslot* next;
    int page;
    int frame;
};

// TLB linked list struct
struct TLB {
    struct TLBslot* head;
    struct TLBslot* tail;
    int size;
};

// LRU queue item
struct queueItem {
    struct queueItem* next;
    int frame;
};

// LRU queue struct
struct queue {
    struct queueItem* head;
    struct queueItem* tail;
    int size;
};

int main(int argc, char** argv) {
    // verify args
    if (argc < 3) {
        printf("Format: ./a.out <file> <frame size>");
        return 1;
    }

    // open files
    FILE* fp = fopen(argv[1], "r");
    FILE* bs = fopen("BACKING_STORE.bin", "rb");
    FILE* out = fopen("output.txt", "w");

    // read in number of frames
    int NUM_FRAMES = atoi(argv[2]);
    fprintf(out, "# of frames: %d \n", NUM_FRAMES);

    // define counters
    int total = 0;
    int pageFaults = 0;
    int tlbHits = 0;

    // set up TLB
    struct TLB tlb;
    tlb.head = NULL;
    tlb.tail = NULL;
    tlb.size = 0;

    // set up page table
    int pageTable[NUM_PAGES];
    for (int i = 0; i < NUM_PAGES; i++) pageTable[i] = -1;

    // set up memory
    char mem[NUM_FRAMES][FRAME_SIZE];

    // set up LRU queue
    struct queue q;
    q.head = NULL;
    q.tail = NULL;
    q.size = 0;

    // define variables
    int virtualAddress, physicalAddress;
    char value;
    int frameCounter = 0;

    // read in addresses
    fscanf(fp, "%d", &virtualAddress);
    while (!feof(fp)) {
        total++;

        // decode virtual address
        int pageNumber = virtualAddress >> 8;
        int offset = virtualAddress & 0x00FF;
        int frame = -1;

        // check TLB
        struct TLBslot* curr = tlb.head;
        while (curr != NULL) {
            if (curr->page == pageNumber) {
                tlbHits++;
                frame = curr->frame;
                break;
            }
            curr = curr->next;
        }

        // not in TLB
        if (frame == -1) {
            // check page table
            if (pageNumber > NUM_PAGES) pageNumber = pageNumber % NUM_PAGES;
            frame = pageTable[pageNumber];
            if (frame == -1) {
                // check if queue is full
                if (q.size < NUM_FRAMES) {
                    // queue has space - create new frame
                    frame = frameCounter++ % NUM_PAGES;

                    // add to queue
                    struct queueItem* newItem = malloc(sizeof(struct queueItem));
                    newItem->next = NULL;
                    newItem->frame = frame;
                    if (q.head == NULL) {
                        q.head = newItem;
                        q.tail = newItem;
                    } else {
                        q.tail->next = newItem;
                        q.tail = newItem;
                    }
                    q.size++;
                    
                } else {
                    // queue is full - steal least recently used frame
                    frame = q.head->frame;

                    // update page table
                    for (int i = 0; i < NUM_PAGES; i++) {
                        if (pageTable[i] == frame) {
                            pageTable[i] = -1;
                            break;
                        }
                    }

                    /*
                    // update TLB?
                    curr = tlb.head;
                    struct TLBslot* prev = NULL;
                    while (curr != NULL) {
                        if(curr->frame==frame) curr->frame = -1;
                        if (curr->frame == frame) {
                            if (curr == tlb.head) {
                                tlb.head = curr->next;
                            } else if (curr == tlb.tail) {
                                prev->next = NULL;
                                tlb.tail = prev;
                            } else {
                                prev->next = curr->next;
                            }
                            free(curr);
                            tlb.size--;
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                    */
                }

                // update page table
                pageTable[pageNumber] = frame;
                pageFaults++;

                // read in from backing store
                if (fseek(bs, pageNumber * NUM_PAGES, SEEK_SET) != 0) {
                    printf("error in fseek\n");
                }
                if (fread(mem[frame], sizeof(signed char), PAGE_SIZE, bs) == 0) {
                    printf("error in fread\n");
                }
            }
            
            // FIFO TLB removal
            if (tlb.size == TLB_SIZE) {
                struct TLBslot* oldHead = tlb.head;
                tlb.head = oldHead->next;
                free(oldHead);
                tlb.size--;
            }
            
            // create new TLB item
            struct TLBslot* newItem = malloc(sizeof(struct TLBslot));
            newItem->next = NULL;
            newItem->page = pageNumber;
            newItem->frame = frame;

            // add item to TLB
            if (tlb.size == 0) {
                tlb.head = newItem;
                tlb.tail = newItem;
            } else {
                tlb.tail->next = newItem;
                tlb.tail = newItem;
            }
            tlb.size++;
        }

        // determine physical address and value
        value = mem[frame][offset];
        physicalAddress = frame << 8;
        physicalAddress = physicalAddress | offset;

        // update placement within LRU queue
        struct queueItem* currI = q.head;
        struct queueItem* prevI = NULL;
        while (currI != q.tail) {
            if (currI->frame == frame) {
                // remove from position
                if (currI == q.head) {
                    q.head = currI->next;
                } else {
                    prevI->next = currI->next;
                }

                // add to tail
                currI->next = NULL;
                q.tail->next = currI;
                q.tail = currI;

                // exit while loop
                break;
            }

            // iterate to next value
            prevI = currI;
            currI = currI->next;
        }

        // print values
        fprintf(out, "Virtual address: %d Physical address: %d, Value: %d\n", virtualAddress, physicalAddress, value);

        // read next address
        fscanf(fp, "%d", &virtualAddress);
    }

    // print statistics
    fprintf(out, "Number of Translated Addresses = %d\n", total);
    fprintf(out, "Page Faults = %d\n", pageFaults);
    fprintf(out, "Page Fault Rate = %.3f\n", pageFaults / (double)total);
    fprintf(out, "TLB Hits = %d\n", tlbHits);
    fprintf(out, "TLB Hit Rate = %.3f\n", tlbHits / (double)total);

    // close files
    fclose(fp);
    fclose(bs);
    fclose(out);

    // final message
    printf("Program finished - see output.txt\n");

    return 0;
}