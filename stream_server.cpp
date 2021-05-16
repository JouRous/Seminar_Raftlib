#include <iostream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#include <raft>


using namespace std;

int clintListn = 0, clintConnt = 0;
struct sockaddr_in ipOfServer;

string readInputFile(string path)
{
    string line;
    ifstream myfile(path);
    if (myfile.is_open())
    {
        string newLine = "";
        while (getline(myfile, newLine))
        {
            line += newLine + "\n";
        }
        myfile.close();
    }
    return line;
}

vector<string> splitString(string input, string delimiter)
{
    vector<string> pieces;

    size_t pos = 0;
    string token;
    while ((pos = input.find(delimiter)) != string::npos)
    {
        token = input.substr(0, pos);
        pieces.push_back(token);
        input.erase(0, pos + delimiter.length());
    }

    return pieces;
}

class producer : public raft::kernel
{
private:
    int i = 0;
    string inputFile = "";
    vector<string> inputLine;

public:
    producer() : raft::kernel()
    {
        cout << ">> Constructer of producer() run ... " << endl;

        output.addPort<string>("out");

        this->inputFile = readInputFile("input.txt");
        this->inputLine = splitString(this->inputFile, "\n");

        /* cout << ">> Splitted size is " << this->inputLine.size() << endl; */
    }

    virtual raft::kstatus run()
    {
        if (i < this->inputLine.size())
        {
            string outputString = this->inputLine[i];
            cout << ">> Generate " << i << ": " << outputString << endl;
            i++;

            output["out"].push(outputString);

            return (raft::proceed);
        }
        else
        {
            return (raft::stop);
        }
    };
};

class consumer : public raft::kernel
{
public:
    consumer() : raft::kernel()
    {
        cout << ">> Constructer of consumer() run ... " << endl;

        input.addPort<string>("in");
    }

    virtual raft::kstatus run()
    {
        string portInput = input["in"].peek<string>();
        input["in"].recycle();

        cout << ">> Write: " << portInput << " ... " << endl;

        portInput = portInput + "\r\n";
        write(clintConnt, portInput.c_str(), strlen(portInput.c_str()));
        sleep(1);

        return (raft::proceed);
    }
};

int main()
{
    cout << ">> Start initialize Socket ... ";
    clintListn = socket(AF_INET, SOCK_STREAM, 0);

    memset(&ipOfServer, '0', sizeof(ipOfServer));

    ipOfServer.sin_family = AF_INET;
    ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);
    ipOfServer.sin_port = htons(9999);

    bind(clintListn, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer));
    listen(clintListn, 20);

    clintConnt = accept(clintListn, (struct sockaddr *)NULL, NULL);
    cout << "Done" << endl;

    cout << ">> Start initialize process ... " << endl;
    producer a;
    consumer b;
    raft::map m;
    m += a >> b;
    cout << "Done" << endl;

    cout << ">> Execute process ... " << endl;
    m.exe();

    cout << ">> Execute done, quit ... ";
    return (EXIT_SUCCESS);
}
