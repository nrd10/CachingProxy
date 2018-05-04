//#include <sys/stat.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <fstream>
//#include <iostream>
//
//int main(void) {
//  pid_t pid, sid;
//  pid = fork();
//  if (pid > 0) {
//    exit(EXIT_SUCCESS);
//  } else if (pid < 0) {
//    exit(EXIT_FAILURE);
//  }
//  // pid == 0
//  sid = setsid();
//  if (sid < 0) {
//    /* Log the failure */
//    exit(EXIT_FAILURE);
//  }
//  int fd = open("/dev/null",O_RDWR);
//  if (fd == -1) {
//    std::cout << "dev/null error" << std::endl;
//  }
//  dup2(fd,0);
//  dup2(fd,1);
//  dup2(fd,2);
//  if ((chdir("/")) < 0) {
//    exit(EXIT_FAILURE);
//  }
//  umask(0);
//  pid = fork();
//  if (pid == 0) {
//    const int dir_err = system("mkdir -p /var/log/erss/");
//    if (-1 == dir_err){
//        printf("Error creating directory!\n");
//        exit(1);
//    }
//    std::ofstream outfile ("/var/log/erss/proxy.log");
//    std::cout << outfile.fail() << std::endl;
//    outfile << "my text here!" << std::endl;
//    outfile.close();
//    while(1) {
//      // proxy code
//    }
//  }
//  exit(EXIT_SUCCESS);
//}
