# ECS160Linux
ECS 160 Project Linux

# Getting Started
You will need [Git](https://git-scm.com/download/) and Linux installed.
We suggest downloading [Ubuntu](https://www.ubuntu.com/download/desktop).

You will also need the following installed
- portaudio19
- libgtk-3
- libsndfile1
- libmpg123
- make

For Mac users you can download a Virtual Machine or Install a Boot Camp software

Ubuntu Environment Setup Procedure:
```
$ git clone https://github.com/UCDClassNitta/ECS160Linux.git
$ cd ECS160Linux
$ sudo apt-get install build-essential libmpg123-dev portaudio19-dev libgtk-3-dev libsndfile1-dev make liblua5.2-dev
```

Fedora Environment Setup Procedure:
```
$ git clone https://github.com/UCDClassNitta/ECS160Linux.git
$ cd ECS160Linux/
$ sudo yum install gcc-c++ libsndfile-devel.* mpg123-devel.* portaudio-devel
$ sudo dnf install gtk3-devel gstreamer-devel clutter-devel webkitgtk3-devel libgda-devel gobject-introspection-devel
```


# Platforms
[Ubuntu (64-bit)](https://www.ubuntu.com/download/desktop): An open source software operating system(64-bit) that runs from the desktop
[Ubuntu (32-bit)](https://www.ubuntu.com/download/desktop): An open source software operating system(32-bit) that runs from the desktop
[Fedora](https://getfedora.org/en/workstation/download/)

# Language
[C++11](http://www.cplusplus.com/info/description/): A version of the standard for the programming language C++

# Introduction
This is simplified version of the actual WarCraft game: SJACraft 2: MOSS of Darkness.

A cumulative contribution project that was created to understand and study Software Programming concepts such as different types of architectures and designs.
### Current Features
Single Player
Single Player Save/Load
Multiplayer

###Code
The main file for setting up the game is ApplicationData.cpp. It sets up all the files to be read and prepares all the sound files to be rendered at the respective event.
ApplicationData.cpp calls the Activate() function to start the game. When you start playing the game, the code calls BattleMode.cpp code to calculate what it needs to render next.
So, the program renders the screen at every 15ms(Timestep).


# Running the Game
After setting up the Environment, compile the game with 'make'
The Makefile creates a executable in the bin folder 'thegame'
You can run the game with ```./bin/thegame```

```
$ make
$ ./bin/thegame
```

For `$ make`you can run
`$ make O=1` <- Optimization: should be turned on when you are trying to increase the performance of the generated code with some trade-off in compilation time
or
`$ make V=1` <- Verbose: you can see all the g++ command and which files are being compiled

# Reporting Issues
You can report [Issues](https://github.com/UCDClassNitta/ECS160Linux/issues)
```
Click "New Issue"
Write a short and descriptive Title
In the write component include the following:
        - Description : Describe in detail the exact problem
        - Steps to Reproduce: label a step by step implementation of how to reproduce the bug/issue.

**If it is not reproduceble with the same steps we will not be able to fix it.

Apply a Label on the right-hand side:
        - bug (the red label)
        - The Platform (iOS, OSX, Windows, Linux)

Then click "Submit New Issue"
```

# Suggesting Features
You can suggest Features by going to [Issues](https://github.com/UCDClassNitta/ECS160Linux/issues)
```
Click "New Issue"
Write a short and descriptive Title
In the write component include the following:
        - Description : Describe in detail the exact feature

Apply a Label on the right-hand side:
        - Feature (The Peach label)
        - The team that is affiliated (iOS, OSX, Windows, Linux)

Then click "Submit New Issue"
```
Disclaimer: Features will be implemented when they can.

# Contribution

`master` is the main branch of the project, we do not currently have a development branch. `master` is a protected branch that prevents direct pushing.
In order to merge into `master` you must first push a new branch with your changes to GitHub and submit a Pull Request (PR). Someone will then review your code and approve the PR for merging.

### Fixing Issues

If you are fixing an Issue that was reported, please [close the Issue](https://help.github.com/articles/closing-issues-using-keywords/) using your Pull Request.

### Code Formatting

Keeping a consistent code format is important and reduces the occurrences of merge conflicts.
So please take a look at the current code to get an idea of the format. Any discrepancies will be fixed when reviewing the pull request.


### Generating documentation using the command line
The following command is available to generate documentation reflecting the code:
Follow the directions in our [Linux Team Documentation](https://github.com/UCDClassNitta/ECS160Docs/tree/master/LinuxDocumentation) to generate the documentation on the code.

### Sample workflow for making new features/changes:
  1. Checkout `master` and pull the latest changes from GitHub
     - `git checkout master`
     - `git pull https://github.com/UCDClassNitta/ECS160Linux.git`
  2. Make a new branch for your changes
     - `git checkout -b <branch-name>`
  3. Make your changes
  4. Make your commits
     - See what files you changed:
       - `git status`  - Shows the modified files
       - `git diff [<file-name>]` - Shows the modified lines of a particular file
     - Stage the changes (Choose what changes will be part of the commit)
       - `git add <file-name>` - Add a specific file or pattern
       - `git add -p` - Interactively choose chunks of code to add (My favorite)
       - `git add .` - Add all changes (Use sparingly as it may lead to unwanted files being added)
     - Make the commit
       - `git commit -m "<commit-message>"` - Commit message should be short
  5. Get any new changes from `dev` (It is a good idea to do this throughout the development of your feature as well.)
     - `git checkout master`
     - `git pull https://github.com/UCDClassNitta/ECS160Linux.git`
     - `git checkout<branch-name>`
     - `git rebase master` - will add your commits on top of master branch latest changes
  6. Deal with Merge conflicts
     - `git mergetool` -launches a tool to help merge
  5. Push your branch to GitHub
     - `git push origin <branch-name>`
  6. Create a pull request for the ECS160Linux repository on GitHub.
     - `GitHub` > `Pull requests` >  `New pull request` > `compare: <branch-name>` > `Create pull request` > Fill in the description > `Create pull request`

      Follow this format:
      ![Alt image](https://cdn.discordapp.com/attachments/365626067500793856/368570684059811850/unknown.png)
