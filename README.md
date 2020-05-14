2048
=====

# Development
[Open this project in 8bitworkshop](http://8bitworkshop.com/redir.html?platform=nes&githubURL=https%3A%2F%2Fgithub.com%2FG42makes%2Fnes_2048&file=2048.c).

I've done all of the code work on this in that IDE, and used or modified most of my resources from there. I do not have a build process in here, as it's all done by the IDE. It's free and web based, so feel free to take a look. Graphics work has been done in the NES Screen Tool 2.51.

Please note that I am not an expert programmer or NES guru. I'm just playing around and this seemed like a fun project to implement while learning a few things about the hardware.  

# Playing
If you have played 2048 anywhere else before, this should be familiar. If not, use the direction pad to merge matching numbers and try to get a block with a value of 2048. That's it.

The .rom file found in the bin folder should have the latest image of the game. It appears to work fine in most emulators, but is coded for the one in the IDE linked above. You may have to rename it to 2048.nes to get your emulator to like it.

I have also used the .rom file on an Everdrive in an original NES with the HiDefNES mod applied and it looks good and plays fine there too.

# TODO
These are items that I want to do to learn new things about coding on the NES, and will make the game better.

* Animate the moves/merges with sprites
* Add some sound
* Add more interesting stuff to the playfield(ie game name at the top, more interesting graphics for win/loss)
* Timer
* Options Page
  * Pallet choice
  * Tile choice
  * Background choice
  * Change W/H of playing board
  * Change win score
* Save score
* Makefile for local compile
* General code cleanups - it's a mess right now and could use some refactoring and re-arranging for logical ordering.


# Bugs
* Nothing comes to mind right now, but there are plenty
