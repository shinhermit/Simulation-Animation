#ifndef SA_H
#define SA_H

void usage(char * cmd, const bool & terminate=true);
void parseCmd(const int & argc, char * argv[], int & debug, int * size, int & nbItems, bool & gpuMode);

#endif // SA_H

