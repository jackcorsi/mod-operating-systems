# mod-operating-systems
Some C programming exercises including a linux character device driver. Below are the specifications given for each.

# Exercise 1
sort_simple (5 points)
Write a program that reads string from the standard input. All strings will contain no 0-byte characters and will be terminated by a \n newline. The last line of input might not be terminated by a newline character. After reading all strings, these strings should be sorted according of the order of the strcmp function as provided by libc. After all strings have been sorted, these string should be printed to STDOUT in the new order. Every string should be terminated by a \n newline character when writing it to STDOUT.

In this task, there will be at most 20 strings in the input, each one of them at most 20 characters long including the terminating \n newline.

Your solution must be saved in sort.c. You are not allowed to add additional files or change the Makefile.

There is a test in test.sh, that checks your compiled code against a few test cases. Feel free to add more tests. When the tests are passed with success, then test.sh will print "OK".

sort_advanced (5 points)
This is the same task as in sort_simple, but your implementation must be able to handle an arbitrary number of input lines of arbitrary length. Should your implementation fail to allocate sufficient memory, then your program should exit with exit code 1 and should not generate any output.

linked_list (5 points)
Your task is to implement a linked list for integers (int). In linkedlist.h, there is already a header file that defines the interface of that list. You will define the type of your list there with typedef struct list and you may define additional data type in this file. You must implement your list in linkedlist.c. In test_list.c is a simple test for your code that tests your code.

Again, there is a script in test.sh, that runs test_list and compares the output with a reference output.

# Exercise 2
Your task in exercise 2 is to implement a logging server and client.

# Client

The client is a program that takes 2 arguments:

* A hostname of a logging server, for example log.cs.bham.ac.uk
* A port number of the logging server, for example 5555

After it started, it will connect to the server and read messages from
STDIN. Every message is a string without 0 bytes. Messages are
terminated by a newline character \n. When there is nothing to read
anymore, the client will close the
connection and terminate with return code 0.

When there is an internal problem, such as the connection to the server breaks, an input cannot be parsed or a similar problem, the return code should be 1. Also you should print an error message that describes the internal error to STDERR.

# Server

The server is a program that takes 2 arguments:

* The port number to bind to, for example 5555
* A filename of a file to write the logs to, for example: /var/log/messages.log

After the server is started, it will bind to the port and listen for
incoming connections. For every connection, the server will read
messages and write them to the logfile. The format of the logfile is
always a line number in decimal followed by a space followed by the
message itself with a \n newline character at the end. When the server
cannot bind to the port, it will return with return code 1. When a
certain file cannot be opened or writing to a file fails, you should
write an error message to STDERR but continue to operate. When the
server starts it creates a new logfile if it does not exist, and
appends the data from the client to the logfile if it already exists.
When the server starts the first line added to the logfile always has the line number 0 even if the logfile is not empty.

The server should be able to handle inputs that are not properly formatted and should never crash. The client should also be able to handle any kind of unexpected responses from the server or invalid lines in the input or command line argument.

# Language and project format

You should implement your project in C and compile it with gcc from
the course VM. You are required to use `-D_POSIX_SOURCE -Wall -Werror
-pedantic -std=c99 -D_GNU_SOURCE -pthread` as command line
arguments. You will write a Makefile that will compile the project to
three binaries, namely "serverSingle", "serverThreaded" and "client"
as the default target. Do not use any files or directory with the
prefix "test", since we will use such files for running tests. Your
solution must be contained in a folder exercise2 in your project on
the School's git server. The folder exercise2 must be at the top level
your project for the marking scripts to work.
We will run the command make in the folder exercise2 in order to obtain
all required binaries.

# Parallelism

This tasks consists of two sub-tasks.

* In the first task, you just need to implement a server that handles a single connection at a time. For a perfect implementation, you will receive at most 10 points. Your result must compile to serverSingle.
In the second task, you will implement a multi-threaded server that
 handles in general an arbitrary number connections in parallel, as
 long as system resources are available. For a perfect
 implementation, you will receive at most 10 points. Your result must
 compile to serverThreaded. 

Of course, every correct solution of serverThreaded is also a correct solution for serverSingle. So when you are confident, feel free to start with serverThreaded directly and just copy the result to serverSingle.

# Remarks

* The critical section in serverThreaded should be as short as possible.
* The server must not leak memory.
* For marking we will use additional, more advanced, test scripts which check whether your program satisfies the specification. If the provided test scripts fail, all the more advanced test scripts are likely to fail as well.
* Any code which does not compile on the virtual machine provided will
  be awarded 0 marks and not be reviewed.
  
# Exercise 3
The Task:

Write a device driver for a character device which implements a simple way of message passing. The kernel maintains a list of messages. To limit memory usage, we impose a limit of 4KiB = 4*1024 bytes for each message, and also impose a limit of the size of all messages, which is initially 2MiB = 2*1024*1024 bytes.

Your device driver should perform the following operations:

Creating the device, which has to be /dev/opsysmem, creates an empty list of messages.
Removing the device deallocates all messages and removes the list of messages.
Reading from the device returns one message, and removes this message from the kernel list. If the list of messages is empty, the reader returns -EAGAIN.
Writing to the device stores the message in kernel space and adds it to the list if the message is below the maximum size, and the limit of the size of all messages wouldn't be surpassed with this message. If the message is too big, -EINVAL is returned, and if the limit of the size of all messages was surpassed, -EAGAIN is returned.
You should also provide an ioctl which sets a new maximum size of all messages. This operation should succeed only if the new maximum is bigger than the size of all messages currently held. The ioctl number for this operationg should be 0, and the ioctl parameter should be the new maximum size. The ioctl should return 0 on success and -EINVAL on failure.
The kernel module which implements this driver must be called charDeviceDriver.ko.
Your solution must not use the kernel fifo.
You need to ensure that your code deals with multiple attempts at reading and writing at the same time. Your code should minimise the time spent in critical sections. The reader should obtain the messages in the same order as they were written.

Extra task (Bonus)
For 5 bonus marks you should implement blocking reads and writes: instead of returning -EAGAIN when reading and writing from the device, you should block the reader until a message is available. Similarly, you should block the writer until there is room for the message (in case of the writer). For the available kernel mechanisms to achieve blocking, see the section ``Wait queues and Wake events'' in the device driver documentation (https://www.linuxtv.org/downloads/v4l-dvb-internals/device-drivers/ch01s04.html). If you do this bonus part you will need to produce two kernel modules, one named charDeviceDriver.ko for the non-bonus part, and one named charDeviceDriverBlocking.ko for the bonus part. You may assume that only one of these two modules will be loaded at any given time.
