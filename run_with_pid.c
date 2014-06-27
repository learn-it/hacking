// From: http://efiop-notes.blogspot.ru/2014/06/how-to-set-pid-using-nslastpid.html

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 int fd, pid;
 char buf[32];

 if (argc != 2)
  return 1;

 printf("Opening ns_last_pid...\n");
 fd = open("/proc/sys/kernel/ns_last_pid", O_RDWR | O_CREAT, 0644);
 if (fd < 0) {
  perror("Can't open ns_last_pid");
  return 1;
 }
 printf("Done\n");

 printf("Locking ns_last_pid...\n");
 if (flock(fd, LOCK_EX)) {
  close(fd);
  printf("Can't lock ns_last_pid\n");
  return 1;
 }
 printf("Done\n");

 pid = atoi(argv[1]);
 snprintf(buf, sizeof(buf), "%d", pid - 1);

 printf("Writing pid-1 to ns_last_pid...\n");
 if (write(fd, buf, strlen(buf)) != strlen(buf)) {
  printf("Can't write to buf\n");
  return 1;
 }
 printf("Done\n");

 printf("Forking...\n");
 int new_pid;
 new_pid = fork();
 if (new_pid == 0) {
  printf("I'm child!\n");
  exit(0);
 } else if (new_pid == pid) {
  printf("I'm parent. My child got right pid!\n");
 } else {
  printf("pid does not match expected one\n");
 }
 printf("Done\n");

 printf("Cleaning up...");
 if (flock(fd, LOCK_UN)) {
  printf("Can't unlock");
 }

 close(fd);

 printf("Done\n");

 return 0;
}
