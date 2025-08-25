
#include <stdio.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>

//#include <sys/wait.h>
//#include <stdlib.h>
#include <sstream>
#include <iostream>
using namespace std;
// #####################################################################
//      Network exist checking function
// #####################################################################
#define IS_NOT_FOUND    0 
#define IS_FILE         1
#define IS_DIRECTORY    2

int checkIfFileExists(const char* filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist != -1)
    {
        if (S_ISDIR(buffer.st_mode))
            return IS_DIRECTORY;
        else
            return IS_FILE;  // no path. but is file
    }
    else
        return IS_NOT_FOUND;
}

void network_check(bool& net_02, bool& net_03)
{
    const char* network_02_file = "/sys/bus/pci/drivers/igb/0000:02:00.0";
    const char* network_03_file = "/sys/bus/pci/drivers/igb/0000:03:00.0";

    if (checkIfFileExists(network_02_file) == IS_DIRECTORY)
        net_02 = true;
    else
        net_02 = false;


    if (checkIfFileExists(network_03_file) == IS_DIRECTORY)
        net_03 = true;
    else
        net_03 = false;
}


// #####################################################################
//      network bind, unbind function
// #####################################################################
#define  BUFF_SIZE      1024
int network_bind_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "echo {} | sudo -S sh -c \"echo %s | tee /sys/bus/pci/drivers/igb/bind\"", szNetIP.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error 
            {
                is_connected = true;         
                break;
            }          
        }
        pclose(fp);
    }
    return  is_connected;
}




// Warning!! Un-bind function!! 
int network_Un_bind_command(std::string szNetIP)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "echo {} | sudo -S sh -c \"echo %s | tee /sys/bus/pci/drivers/igb/unbind\"", szNetIP.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     // if result value => "1" exist.. ping success. else ping error 
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}

// #####################################################################
//      shell command
// #####################################################################

// sub return value -> only 0 or 1 only (integer)
// for firmware update
// 0 => fault,  1 => success
int shell_0_or_1(std::string shellCmd)
{
    int iReturn = 0;
    char    MakeCmd[BUFF_SIZE];

    sprintf(MakeCmd, "%s", shellCmd.c_str());    
    iReturn = system(MakeCmd);
    iReturn = WEXITSTATUS(iReturn);

    return iReturn;
}

std::string shell_str_rtn(std::string shellCmd)
{
    char    MakeCmd[BUFF_SIZE];    
    ostringstream output;

    sprintf(MakeCmd, "%s", shellCmd.c_str());
    FILE* stream = popen(MakeCmd, "r");

    while (!feof(stream) && !ferror(stream))
    {
        char buf[128];
        int bytesRead = fread(buf, 1, 128, stream);
        output.write(buf, bytesRead);
    }

    string result = output.str();
    // cout << "shell result" << result << endl;

    return result;
}


// #####################################################################
//      ecmaster process find & kill
// #####################################################################
int ecmaster_process_kill(int iProcessNum)
{
    char    buff[BUFF_SIZE];
    char    MakeCmd[BUFF_SIZE];
    bool     is_connected = false;
    FILE* fp;

    sprintf(MakeCmd, "sudo kill -9 %d", iProcessNum);
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        while (fgets(buff, BUFF_SIZE, fp))
        {
            if (NULL != index(buff, '1'))     
            {
                is_connected = true;
                break;
            }
        }
        pclose(fp);
    }
    return  is_connected;
}


int ecmaster_process_find(std::string strProcessName)
{
    FILE* fp;
    char    buff[BUFF_SIZE];

    // Step 1. make process list
    
    char    MakeCmd[BUFF_SIZE];

    sprintf(MakeCmd, "ps -ef | grep '%s' > ps_list.txt", strProcessName.c_str());
    fp = popen(MakeCmd, "r");

    if (NULL != fp)
    {
        fgets(buff, BUFF_SIZE, fp);
        pclose(fp);
    }

    // Step 2. process file line num check
    int iProcessLineCnt = 0;       
    char c;
    fp = fopen("ps_list.txt", "r");

    while ((c = fgetc(fp)) != EOF)
        if (c == '\n')
            iProcessLineCnt++;
    fclose(fp);

    // Step 3. process kill Go / No Go
    int iProcessNum = 0;

    if (iProcessLineCnt < 3)    // no process found
    {
        ;
    }
    else // process kill
    {
        fp = fopen("ps_list.txt", "r");
        fscanf(fp, "%s %d", buff, &iProcessNum);
        fclose(fp);

        ecmaster_process_kill(iProcessNum);
    }

    return 0;
}




int main(void)
{
    std::string strResult = shell_str_rtn("./a.out");
    
    printf("Shell result = %s \n", strResult);

#if 0
    const char* network_02_open = "0000:02:00.0";
    const char* network_03_open = "0000:03:00.0";

    bool net_02 = false;
    bool net_03 = false;

    ecmaster_process_find("ecmaster");

    network_check(net_02, net_03);
    printf("\n\t NET_02 = %d  NET_03 = %d \n", net_02, net_03);

    if (net_02 == false)
    {
        network_bind_command(network_02_open);
        printf("Network port (02) auto open \n");
    }
    else
    {
        printf("Network port (02) already exist \n");
    }

    if (net_03 == false)
    {
        network_bind_command(network_03_open);
        printf("Network port (03) auto open \n");
    }
    else
    {
        printf("Network port (03) already exist \n");
    }
#endif
    return 0;
}