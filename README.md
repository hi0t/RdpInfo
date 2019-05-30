# RdpInfo

RdpInfo is 1C Native API library for information about Windows Remote Desktop session.

## Available properties
* IsRemoteSession - returns TRUE if the application is running in a remote ession
* RemoteAddress - the remote endpoint (IP address and port) of the client connected to the session
* ClientAddress - the IP address reported by the client
* UserName - the name of the user account that last connected to the session
* ClientName - the name of the machine last connected to this session

## How To Use
You can [download](https://github.com/hi0t/RdpInfo/releases) prebuilt library. A simple example of use:
```
AttachAddIn("RdpInfo.dll", "RdpAddIn", AddInType.Native);
rdp = New("AddIn.RdpAddIn.RdpInfo");

Message(rdp.IsRemoteSession, MessageStatus.Information);
Message(rdp.RemoteAddress, MessageStatus.Information);
Message(rdp.ClientAddress, MessageStatus.Information);
Message(rdp.UserName, MessageStatus.Information);
Message(rdp.ClientName, MessageStatus.Information);
```
