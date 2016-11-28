namespace cpp cs


exception error
{
    1: string reason
}

service AuthServe
{
    binary createAccount(1: string userName, 2: map<string,string> keyValues)  throws (1: error er),

}
