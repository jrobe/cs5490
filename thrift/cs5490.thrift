namespace cpp cs


exception error
{
    1: string reason
}

struct PermissionRequest
{
    1: optional string user;
    2: optional i64 requestID;
    3: optional bool granted;
    4: optional string reason;
    5: optional map<string,string> results;
}

service AuthServe
{
    //Create the account (from the portal); returns the key
    binary createAccount(1: string userName, 2: map<string,string> keyValues)  throws (1: error er),

    //Not used
    map<string,string> retrieveWithKey(1: string userName, 2: binary key) throws (1: error err),

    // Demo app will call this to request permission
    PermissionRequest requestPermission(1: string userName) throws (1: error err),  

    //Portal will call this for requests
    list<PermissionRequest> checkForPermissionRequests() throws (1: error err), 

    //Portal decides what to do and informs server
    void decideRequest(1: PermissionRequest request, 2: bool decision, 3: string reason, 4: string key) throws (1: error err), 
    
    //Demo app polls this attempting to get it's requested permission
    PermissionRequest checkForPermissionGranted(1: i64 requestID) throws (1: error err),



}
