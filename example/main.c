/*
 * Copyright (c) 2019 by Alexandru - George Rusu. All Rights Reserved.
 */
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define CHAR_DEV_NAME "/dev/teps_char_dev"

#define LEN 1024

char buff_recv[LEN];
char buff_send[LEN];

int main(void)
{
  int ret, fd;

  fd = open(CHAR_DEV_NAME, O_RDWR);
  if(fd < 0)
  {
    perror("Failed to open the device. ");
    return errno;
  }
  else
  {
    printf("The device %s has been successfully opened.\n", CHAR_DEV_NAME);
  }
  printf("Start typing messages or 'exit' to close the device\n\n");

  while(strcmp(buff_recv, "exit"))
  {
    memset(buff_recv, 0, LEN);
    printf("\nType a message:\n");
    scanf("%s", buff_send);
    printf("Message '%s' written to %s.\n", buff_send, CHAR_DEV_NAME);
    ret = write(fd, buff_send, strlen(buff_send));

    if(ret < 0)
    {
      perror("Failed to write to the device");
      return errno;
    }
    else
    {
      memset(buff_send, 0, LEN);
    }

    ret = read(fd, buff_recv, LEN);

    if(ret < 0)
    {
      perror("Failed to read from the device");
      return errno;
    }
    else
    {
      printf("Echo message '%s' from %s.\n", buff_recv, CHAR_DEV_NAME);
    }
  }

  if(close(fd) < 0)
  {
    perror("Failed to close the device");
    return errno;
  }
  else
  {
    printf("\nThe device %s has been successfully closed.\n\n", CHAR_DEV_NAME);
  }

  return 0;
}