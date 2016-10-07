# ProducerConsumer
3 Producer 1 Consumer without using semaphore.h


4 pthreads, one for consumer, 3 for producers.
The producers are identified by their color (Black, White, Red).
Each are to produce 1000 products each (COLOR_TIMESEC.TIMEMS,
ex. BLACK_5465788.21321).
Also they write on individual files of their produces 
(producer_RED, producer_BLACK, producer_WHITE).
A shared Buffer is used, and the given solutions helps the
individual threads to be in the critical region one at a time.
