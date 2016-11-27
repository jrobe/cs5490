namespace cpp cs


service AuthServe
{
    binary createAccount(1: string userName, 2: map<string,string> keyValues),

}
