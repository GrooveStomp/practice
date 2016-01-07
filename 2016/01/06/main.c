//#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>

int main()
{
  int cfd;
  int fbfd = 0;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;
  int x = 0, y = 0;
  long int location = 0;

  // Open the file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (fbfd == -1) {
    perror("Error: cannot open framebuffer device");
    exit(1);
  }
  printf("The framebuffer device was opened successfully.\n");

  // Get fixed screen information
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
    perror("Error reading fixed information");
    exit(2);
  }

  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    perror("Error reading variable information");
    exit(3);
  }

  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

  // Figure out the size of the screen in bytes
  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

  // Map the device to memory
  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if ((long)fbp == -1) {
    perror("Error: failed to map framebuffer device to memory");
    exit(4);
  }
  printf("The framebuffer device was mapped to memory successfully.\n");


  // Set console to graphics mode
  if ((cfd = open("/dev/tty0", O_RDWR)) < 0) {
    fprintf(stderr, "Error - cannot open tty0\n");
    exit(2);
  }

  ioctl(cfd, KDSETMODE, KD_GRAPHICS);

  // Figure out where in memory to put the pixel
  for (y = 0; y < 600; y++) {
    for (x = 0; x < 800; x++) {

      location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
        (y+vinfo.yoffset) * finfo.line_length;

      if (x % 2 == 0) {
        *(fbp + location) = (char)255; // blue
        *(fbp + location + 1) = 0;
        *(fbp + location + 2) = 0;
        *(fbp + location + 3) = 0;      // No transparency
      }
      else {
        *(fbp + location) = 0;
        *(fbp + location + 1) = 0;
        *(fbp + location + 2) = (char)255; // red
        *(fbp + location + 3) = 0;      // No transparency
      }
    }
  }
  munmap(fbp, screensize);
  close(fbfd);

  struct timespec sleep_time;
  sleep_time.tv_sec = 1;
  sleep_time.tv_nsec = 0;

  struct timespec remaining_time;

  nanosleep(&sleep_time, &remaining_time);

  // Return from graphics mode.
  ioctl(cfd, KDSETMODE, KD_TEXT);

  return 0;
}
