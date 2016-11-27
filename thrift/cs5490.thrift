namespace cpp cs


service authServe
{
    binary createAccount(1: string userName, 2: map<string,string> keyValues),

}
