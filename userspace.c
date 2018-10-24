#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/capability.h>

void handle_error(char* erstr){
    fprintf(stderr,"ERROR: %s\nusage:\n\tmyioctl [-w] path command [argument] [CAP_FORMAT]\n",erstr);
}

int main(int argc, char **argv){
    int write_enable = 0;
    char command[30];
    char output[30];
    char capset[200];
    char AppName[30];
    char path[100];
    char newargv[200];
        
    //extracting all arguments
    if(argc<3){
	handle_error("number of entries are less than 3");
	return -1;
    }
    strcpy(AppName,*argv);
    argv++;argc--;
    if (!strcmp(*argv, "-w")) {
	if(argc<4){
	    handle_error("argument passing by ioctl command missing.");
	    return -1;
	}
	write_enable = 1;
	argv++;argc--;
    }
    strcpy(path,*argv);
    argv++;argc--;
    strcpy(command,*argv);
    argv++;argc--;
    if(write_enable){
	strcpy(output,*argv);
	argv++;argc--;
    }
    //setting caps section
    if(argc>0){
	//concatinating all caps
	strcpy(capset, "CAP_SETFCAP+eip");
	while(argc>0){
	    strcat(capset," ");
	    strcat(capset,*argv);
	    argv++;argc--;
	}
	cap_t caps = cap_from_text(capset);
	if (caps == NULL)
	    handle_error("caps format is not acceptable.");
	if ( cap_set_file(AppName, caps) ){
		fprintf(stderr,"error while setting capabilities\n");
		return -1;
	}
	//re-executing program after setting new capabilities
	if(write_enable)
	    execv(AppName, (char *[]){ AppName,"-w", path , command ,output, NULL});
	else
	    execv(AppName, (char *[]){ AppName, path , command , NULL});
    }
    //opening device ram file
    int fd = open(path, O_RDONLY);
    //sending ioctl command
    ioctl(fd, atoi(command), output);
    //if ioctl command was read command, then output will be shown
    if(!write_enable && output)
	printf("output = %s\n",output);
    close(fd);
    //resetting caps
    strcpy(capset, "CAP_SETFCAP+eip");
    cap_t caps = cap_from_text(capset);
    cap_set_file(AppName, caps);
    //exit successfully
    return 0;
}