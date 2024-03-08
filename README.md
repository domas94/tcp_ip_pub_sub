# TCP/IP Publish Subscribe

This project will demonstrate the basic example for the publish-subscribe communication protocol on the TCP/IP layer.

We will use `server.cpp` nad `client.cpp` for this purpose. `client_testing.cpp` was used only to speed up testing and it can be ignored.

Contents
========

* [Compilation](#Compilation)
* [Installation](#installation)
  * [Method 1: <a href="https://en.wikipedia.org/wiki/APT_(software)" rel="nofollow">apt install</a>](#method-1-apt-install)


### Installation
---

We will be using the GNU c++ compiler - [`g++`](https://www.geeksforgeeks.org/compiling-with-g-plus-plus/)

> If by any chance g++ is not preinstalled on your system, here is a way to install it:
#### Method 1: apt install


```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install g++
```


### Compilation

Now when we have the compiler installed in our system, we can compile the source files. First the server application:

```bash
$ g++ server.cpp -o server
```

And then the client application:
```bash
$ g++ client.cpp -o client
```

### Usage
---

1. Run the server app `$ ./server`
2. Run the client app `$ ./client`

`shallow-backup` was built with scripting in mind. Every feature that's supported in the interactive program is supported with command line arguments.

```shell
Usage: shallow-backup [OPTIONS]

  Easily back up installed packages, dotfiles, and more.
  You can edit which files are backed up in ~/.shallow-backup.

  Written by Aaron Lichtman (@alichtman).

Options:

  --add-dot TEXT               Add a dotfile or dotfolder to config by path.
  --backup-all                 Full back up.
  --backup-configs             Back up app config files.
  --backup-dots                Back up dotfiles.
  --backup-fonts               Back up installed fonts.
  --backup-packages            Back up package libraries.
  --delete-config              Delete config file.
  --destroy-backup             Delete backup directory.
  --dry-run                    Don't backup or reinstall any files, just give
                               verbose output.

  --new-path TEXT              Input a new back up directory path.
  --no-new-backup-path-prompt  Skip setting new back up directory path prompt.
  --no-splash                  Don't display splash screen.
  --reinstall-all              Full reinstallation.
  --reinstall-configs          Reinstall configs.
  --reinstall-dots             Reinstall dotfiles and dotfolders.
  --reinstall-fonts            Reinstall fonts.
  --reinstall-packages         Reinstall packages.
  --remote TEXT                Set remote URL for the git repo.

  --show                       Display config file.
  -v, --version                Display version and author info.
  -h, -help, --help            Show this message and exit.
```




### What can I back up?
---

By default, `shallow-backup` backs these up.

1. Dotfiles and dotfolders
    * `.bashrc`
    * `.bash_profile`
    * `.gitconfig`
    * `.pypirc`
    * `.config/shallow-backup.conf`
    * `.ssh/`
    * `.vim/`
    * `.zshrc`

2. App Config Files
    * Atom
    * VSCode
    * Sublime Text 2/3
    * Terminal.app

3. Installed Packages
    * `brew` and `cask`
    * `cargo`
    * `gem`
    * `pip`
    * `pip3`
    * `npm`
    * `macports`
    * `VSCode` Extensions
    * `Sublime Text 2/3` Packages
    * System Applications

4. User installed `fonts`.

### Configuration

If you'd like to modify which files are backed up, you have to edit the `JSON` config file, located at `~/.config/shallow-backup.conf`. There are two ways to do this.

1. Select the appropriate option in the CLI and follow the prompts.
2. Open the file in a text editor and make your changes.

Editing the file in a text editor will give you more control and be faster.






