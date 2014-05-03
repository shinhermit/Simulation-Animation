#ifndef SA_H
#define SA_H

/// \brief Help on command line arguments
void usage(char * cmd, const bool & terminate=true);
/// \brief Fetches the values from the command line
void parseCmd(const int & argc, char * argv[], int * size, int & nbItems);

#endif // SA_H

