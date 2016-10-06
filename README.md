# Trojan (Remote-Control, 远程控制工具)
> This project is a simple remote-control tool with GUI written by Qt. **It only works for Windows platform**. The project still remains some bugs. One of them is: **TRANSFER BIG FILE (eg. OVER 200MB) MAY CAUSE CRASH IN CLIENT**. If you have any good ideas, please [contact me](mailto:forec@bupt.edu.cn), or open your PR. I feel honored to learn from your help.

## Platform
* This project contains two parts. The **SERVER** , which should be executed in remote hosts, is in folder `server`, and the **CLIENT**, is in folder `client`,  used to control the remote hosts.
* The Visual Studio project file `remote.sln` is in folder `server`. I wrote this project with VS Ultimate 2013, version `12.0.21005.1 REL`.
* The Visual Studio project file `client.sln` is in folder `client`. I wrote this project with Qt-windows-x86-msvc2013_64, version `5.6.0`. The Qt Creator is 32bits based on MSVC2013, version `3.6.1`. Some configurations may have relationship with the tool.
* I compressed the `remote.exe` and `client.exe` into `compiled.tar`, which can be downloaded from my cloud storage: [here](http://7xktmz.com1.z0.glb.clouddn.com/compiled.tar)

## Usage
### SERVER
* Compile: I remove the `remote.sdf` from the repository since it's too large. If you have `VS2013` (or higher version), just open `remote.sln`, else please build a new project with other IDEs.
* Assume the program we get after released is `remote.exe`, it's exactly a CMD window, however, I hide the window at the entrance of function `main()`. You can change the first two lines code in function `main()` into comments, then its form will appear. Even it's hidding itself, you can stil find it by `CTRL+ALT+DEL`. I didn't hide the process from taskmgr.
* Usage: Just run the executable file `remote.exe`( you need to compile it yourself ). If you delete the code for hidding in `main.cpp`, you can see it print `Server run at IPv4 address <your-ip-address>...`.
* **Attention**: If you want to use `remote.exe` under **x64** system, I suggest that you should compile it as x64.
* Two files will be created under `E:\` in remote hosts, I didn't hide these files. You can use the function provided in `trojan.cpp` which definition is `bool hideFile(const char *path)`. The two files are `E:\key.log` and `E:\screen.tmp`, which will be created by `remote.exe` when client ask for keyboard record or screenshots. **If you want to hide them, remember to use function `hideFile` to hide `E:\screen.tmp` whenever you ask for screenshot.** Since the new screenshot overrides the past screenshot, but the keyboard record is an append operation.
* **TO HIDE**: I didn't do anything protecting `remote.exe`, any common anti-virus software like 360 or Avira can easily kill it. Good news is the Windows Defender doesn't notice this program. To pack the server program, I recommand the `iexpress`, since every Windows system has it as a built-in application, and it's easy, most important. However, **packer is not enough for protect this program from anti-virus software.** Instead of packer, you can also use DLL-Injection( only works for Windows lower than Windows7 ), which is also provided in this repository. But, **PLEASE REMEMBER SELF-DISCIPLINE**.
* The executable file is in `compiled.tar`, you can just run `remote.exe` to have a look.

### CLIENT
* Compile: I put all files needed in folder `client`, if you have `Qt Creator`, just open `client.pro`, else please build a new project with other IDEs.
* The executable file is in `compiled.tar`, you can just run `client.exe` to have a look.
* Assume the program we get after released is `client.exe`, it will show you a window like this. Buttons left are `Refresh` and `Create`. You can **click `Create` to add a remote host, click `Refresh` to refresh all hosts' status**.
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-1.png" width = "400px"/>

* You can right click under a remote host, several functions provided like the following picture. Since the remote host you choose is not connected yet, you cannot do any operation to the remote host before you connect it.
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-2.png" width = "400px"/>

* After connect to a remote host such as the picture followed, you can do some basic operations to the remote host.
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-3.png" width="400px"/>

* **Functions**
 * **Connect/Disconnect**: Build or release connections with selected remote host.
 * **Set IPv4 Address**: Set Ipv4 address for remote host. The IPv4 address must  be valid or the table won't accept.
 * **Modify Notes**: Make notes for the selected remote host.
 * **Get Username**: Get the remote host's username.
 * **Get File**: Input the **absolute path of file** you want to get in remote host, then click `Get File`. The client will ask you to confirm your operation  since **the client will change into disabled mode when transferring files**. That means, when receiving file from remote host, you cannot operate it. Also, this function remains a serious bug, **very big file (perhaps over 200MB) transmission will cause crash**. Be careful when using this function, if crashed, restart the client.
 * **Get Processes List**: Get all the processes running in remote host, the client will display these information in the `QTextEdit`, which is the widget with `>` in the last picture. That widget cannot be edited, but you can copy its content.
 * **Get ScreenShot**: Get the screenshot of remote host. After receving the screenshot, client will open it automaticly.
 * **Get Keyboard Record**: Get the remote host's keyboard record, records will be displayed in `QTextEdit` too.
 * **Export Logs**: Export your operation history for the selected remote host to a `.log` file.
 * **Delete**: Delete the remote host from your database.
 * **Send Shell Command**: You can input **shell commands** (not CMD commands, I use powershell to execute these commands) in the `QLineEdit` widget, and press `Enter` or click `Send` to execute the commands in remote hosts. The results will displayed in `QTextEdit`.

## Attentions
* **Project Configuration**: Since I use `QSqlDatabase` to query and store data, a line `Qt += sql` has been added to the project file `client.pro`.
* **Icon Configuration**: The client's icon can be changed, you just need to replace my `icon.ico` with your own `icon.ico`. However, my girlfriend likes `V for Vendetta`, I don't think other icons could be better.
* **Path**: Several files and directories will be created when `client.exe` is running, I list them below.
 * **files**: Directory for storing the files you get from remote hosts. They are stored just as they like in remote hosts. So remember when you want to get a new file from remote hosts, make sure there is no file with same name in dir  `files` already.
 * **screenshots**: Directory for storing the screenshots you get from remote hosts. They are stored with name format: `<remote-ip-address>-yyyy-MM-dd-hh-mm-ss.bmp`.
 * **logs**: Logs you exported will store in this directory. Their names are same with their related ip addresses.
 * **data.db**: The sqlite3 database we use to store all the remote hosts' informations( ip address, notes, status, names...).
 * **temp.tmp**: Used to buffer data. Don't care about it.

## Examples For Use
* Get Processes List. As you can see, I ran `remote.exe` in my PC to show how the client works. It can still work well in remote hosts.  
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-4.png" width="400px"/>

* Get KeyBoard Record. Here I hide my record in the picture.  
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-5.png" width="400px"/>

* Get File. Here I get `G:\Backup\Wireshark-win64-2.0.0.exe` from remote host. It's 38.9 MB, and the client saves it as `%PATH_TO_CLIENT%/files/Wireshark-win64-2.0.0.exe`.  
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-6.png" width="400px"/>

* After the client got the file.  
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-7.png" width="400px"/>

* Get Screenshot. The client opens the screenshot after it received.   
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-8.png" width="400px"/>

* Send Commands. I send `cd ..; ls; mkdir test`, then the remote creates a folder named `test` in the `%PATH_TO_REMOTE%/../`.  
<img src="http://7xktmz.com1.z0.glb.clouddn.com/remote-control-client-9.png" width="400px"/>

## Bugs
* When sending big files, the client and remote server are hard to synchronizate. **Temporarily, when sending data, I ask both the client and remote server waiting for a response to send next packet.** Obviously it's not a correct method. So crash happened when transmitting big files.
* **TODO**: It should be possible that user can get remote file and do other operations at the same time. However, to achieve this function, I need to start another thread, it's not hard, but the current function is enough for temporary use, so I didn't add this part. Maybe later.
* **TODO**: The shell function is not very convenient, since **the path `remote.exe` executes  commands is `%PATH_TO_REMOTE%`**, and each time you want to execute a new command, the path will go back to the dir. To change the path, add `cd PATH` at first, and use `;` to append other commands.

## Update-logs
* 2016-9-11~12: Write GUI for client, with some basic tests.
* 2016-9-14: Add `trojan.h/cpp`, with basic functions including `stringToLPCWSTR`, `sendFile`, `readFileIntoBuf`, etc.
* 2016-9-15: Add several main functions, deal with keyboard, get screenshot, etc.
* 2016-9-16: Finish all functions, fix some bugs, improve the client. Some basic tests passed.
* 2016-9-17: Fix some problems in `README.md`.
* 2016-9-23: Fix links and move `compiled.tar` to cloud storage.
* 2016-10-7: Build repository.