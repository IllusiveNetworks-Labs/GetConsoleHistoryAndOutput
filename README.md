# GetConsoleHistoryAndOutput
GetConsoleHistoryAndOutput is designed to be used as part of "Live Response" forensic investigations.

The ability to retrieve <b>command history</b> and <b>console output buffer</b> of suspicious processes could enable IR teams to gain visibility into attackers actions on a victim's system.

The GetConsoleHistoryAndOutput receives a PID of a console process and retrieves the console command history and screen output buffer.<br />

Additional reading material on the tool, can be found in our blog [Windows Console Command History: Valuable Evidence for Live Response Investigation](https://blog.illusivenetworks.com/windows-console-command-history-live-response-investigation).

## How to use
```
Usage: GetConsoleHistoryAndOutput.exe pid_of_console_process outputfile_path
```
## Examples
The PID of the following console is 6364
![alt tag](https://image.ibb.co/iQG6K6/cmd.png "cmd.exe")<br /> 
Running:
```
GetConsoleHistoryAndOutput.exe 6364 output.txt
```

Will result with the following data at output.txt:
```
### Display Output of PID: 6364###

## Process' Command History ##
H0: net group "domain admins" /domain

## Console Output Buffer ##
L0: Microsoft Windows [Version 6.1.7601]                                                                                    
L1: Copyright (c) 2009 Microsoft Corporation.  All rights reserved.                                                         
L3: C:\Users\lex>net group "domain admins" /domain                                                                          
L4: The request will be processed at a domain controller for domain DCOMIC.LOCAL.                                           
L6: Group name     Domain Admins                                                                                            
L7: Comment        Designated administrators of the domain                                                                  
L9: Members                                                                                                                 
L11: -------------------------------------------------------------------------------                                         
L12: Administrator            aquaman                  captaina                                                              
L13: lex                                                                                                                     
L14: The command completed successfully.                                                                                     
L17: C:\Users\lex>                                                  
                                                         
###        Output End         ###
```
## Authors

* Tom Sela - [@4x6hw](https://twitter.com/4x6hw)
* Liav Teichner

## License 

This project is licensed under the  BSD 3-clause license - see the [LICENSE](LICENSE) file for details

## Contributors
[Illusive Networks](https://www.illusivenetworks.com/) Research & Dev team members:
* Tomer Shamul
* Tom Kahana
* Dolev Ben Shushan
* Hadar Yudovich
