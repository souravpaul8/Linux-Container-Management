#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace std::chrono;

#define STACK 8192

int container(void *prog_name)
{
    ofstream outb;
    outb.open("/sys/fs/cgroup/cpu/nikhilcg1/cgroup.procs");

    if (!outb)
    {
        cerr << "error in opening the file" << endl;
        exit(1);
    }

    cout << "pid created for container - " << getpid() << endl;

    if (chdir("/home/test/assignments/CS695/22M0814-cs695-a4/root_directory"))
    {
        perror("error in changing directory: chdir /root_directory/");
        return 1;
    }

    if (chroot("/home/test/assignments/CS695/22M0814-cs695-a4/root_directory") != 0)
    {
        perror("error in changing directory: chroot /root_directory/");
        return 1;
    }

    for (int i = 0; i < 5; i++)
    {
        int pid_ch = fork();
        if (pid_ch != 0)
        {
            cout << "child pid is " << pid_ch << endl;
            char arr[100];
            sprintf(arr, "%d", pid_ch);

            outb << pid_ch << endl;
        }
        else
        {
            execv("./bin/tenp_task", NULL);
            perror("Execv error in executing program");
        }
    }

    for (int i = 0; i < 5; i++)
        wait(NULL);

    outb.close();

    return 0;
}

int main()
{

    void *stack = malloc(STACK); // Stack for new process

    if (!stack)
    {
        perror("Malloc Failed");
        exit(0);
    }
    cout << "Start" << endl;
    auto start = high_resolution_clock::now();
    pid_t child_pid = clone(&container, (char *)stack + STACK, CLONE_NEWPID | SIGCHLD | CLONE_NEWCGROUP, NULL);

    if (child_pid < 0)
    {
        perror("creating the container failed");
        exit(0);
    }

    printf("process id of parent  - %d\n", getpid());
    printf("process id of container- %d\n", child_pid);

    wait(NULL);
    auto stop = high_resolution_clock::now();
    auto total_time = duration_cast<microseconds>(stop - start);

    cout << "total time " << total_time.count() << endl;
    free(stack);

    return 0;
}
